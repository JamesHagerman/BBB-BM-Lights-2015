#include "animation.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "p9813.h"
#include "TCLControl.h"
#include "configurations.h"


// Size of onscreen display:
// Define the sizes of the image that shows the lights:
#define WIDTH 12
#define HEIGHT 50
#define TCrgb(R, G, B) (((R) << 16) | ((G) << 8) | (B))

typedef struct {
    ClutterActor *rotatingActor;
    ClutterActor *infoDisplay;
    TCLControl *tcl;
    int *animationNumber;
} AnimationData;

typedef struct {
    ClutterActor *lightDisplay;
    ClutterEffect *shaderEffect;
    TCLControl *tcl;
    int *animationNumber;
    gfloat *input_x;
    gfloat *input_y;
} TouchData;

typedef struct  {
    TCLControl *tcl;
} EventDataAfterPaint;

gdouble rotation = 0;
int cutoff = 0;

// Hold on to the Color Display:
ClutterContent *colors;
ClutterActor *lightDisplay;

//============
// GLSL Shader related:
// ToDo: Uncomment Shader stuff:
ClutterEffect *shaderEffect;
guint8 *shaderBuffer;
gfloat animationTime = 100.0; // A variable to hold the value of iGlobalTime

//  NOT WORKING:
//const gchar *fragShader = ""
//"void main(void) {\n"
//"   cogl_color_out = cogl_tex_coord_in[0];\n" //cogl_color_out = vec4(0.0, 1.0, 0.0, 1.0);
//"}";

const gchar *fragShader = "" //"#version 110\n\n"
        "uniform float iGlobalTime;\n"
        "void main(void) {\n"
        "   cogl_color_out = vec4(cogl_tex_coord_in[0].x+sin(iGlobalTime*0.05), cogl_tex_coord_in[0].y+cos(iGlobalTime*0.01), sin(cogl_tex_coord_in[0].y*50.0), 1.0);\n"
        "}";

// End shaders
//============

GdkPixbuf *pixbuf;
unsigned char *pixels;
int rowstride;
// We need an error object to store errors:
GError *error;

gfloat osd_scale = 16;

int AnimationID = 0;

//=======================
// Helpful color tools:

// Unpack and packing colors into and out of int
int pack(int a, int b, int c) { return (a << 16) | (b << 8) | c; }

int unpackA(int color) { return (color >> 16) & 0xff; }

int unpackB(int color) { return (color >> 8) & 0xff; }

int unpackC(int color) { return (color) & 0xff; }


// convert HSB to RGB:
int rgb_colors[3]; // holder for rgb color from hsb conversion
int hue;
int saturation;
//int brightness; // defined above, set using AN3
const unsigned char dim_curve[] = {
        0, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6,
        6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8,
        8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11,
        11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15,
        15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20,
        20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26,
        27, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35,
        36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 46, 47,
        48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
        63, 64, 65, 66, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 81, 82,
        83, 85, 86, 88, 90, 91, 93, 94, 96, 98, 99, 101, 103, 105, 107, 109,
        110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
        146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
        193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

int getRGB(int hue, int sat, int val) {
    // These values should come in in the range of 0-255, not 0-360!
    hue = hue * 359 / 255;
    /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
       The dim_curve is used only on brightness/value and on saturation (inverted).
       This looks the most natural.
    */

    val = dim_curve[val];
    sat = 255 - dim_curve[255 - sat];

    int colors[3];

    int r = 0;
    int g = 0;
    int b = 0;
    int base;

    if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
        colors[0] = val;
        colors[1] = val;
        colors[2] = val;
    } else {

        base = ((255 - sat) * val) >> 8;

        switch (hue / 60) {
            case 0:
                r = val;
                g = (((val - base) * hue) / 60) + base;
                b = base;
                break;

            case 1:
                r = (((val - base) * (60 - (hue % 60))) / 60) + base;
                g = val;
                b = base;
                break;

            case 2:
                r = base;
                g = val;
                b = (((val - base) * (hue % 60)) / 60) + base;
                break;

            case 3:
                r = base;
                g = (((val - base) * (60 - (hue % 60))) / 60) + base;
                b = val;
                break;

            case 4:
                r = (((val - base) * (hue % 60)) / 60) + base;
                g = base;
                b = val;
                break;

            case 5:
                r = val;
                g = base;
                b = (((val - base) * (60 - (hue % 60))) / 60) + base;
                break;
        }

        colors[0] = r;
        colors[1] = g;
        colors[2] = b;
    }

    // unsigned char toRet = TCrgb(r, g, b);
    return TCrgb(colors[0], colors[1], colors[2]);

}

int getRGB(int hsvColor) {
    return getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));
}

// Random functions: 
int getrand(int min, int max) {
    if (max == min) {
        max += 1;
    }
    return (rand() % (max - min) + min);
}

int randColor() {
    return getrand(0, 255);
}

int getRandomColor() {
    return TCrgb(randColor(), randColor(), randColor());
}

int getBetterRandomColor() {
    return getRGB(randColor(), 255, 255);
}

int getBetterRandomColor(int sat) {
    return getRGB(randColor(), sat, 255);
}

int getHSVRandomColor() {
    return pack(randColor(), 255, 255);
}

int jitter(int toJitter, int jitterAmount) {
    // printf("Derp: %i   %i\n", toJitter-jitterAmount, toJitter+jitterAmount);
    return getrand(toJitter - jitterAmount, toJitter + jitterAmount);
}

int colorJitter(int color, int jitterAmount) {
    int toRet = color;

    int r = (toRet >> 16) & 0xff;
    int g = (toRet >> 8) & 0xff;
    int b = toRet & 0xff;

    // int new_r = jitter(r, jitterAmount);
    // int new_g = jitter(r, jitterAmount);
    // int new_b = jitter(r, jitterAmount);

    r = jitter(r, jitterAmount);
    g = jitter(r, jitterAmount);
    b = jitter(r, jitterAmount);

    toRet = (r << 16) | (g << 8) | b;

    return toRet;
}

int HSVJitter(int hsvColor, int amt) {
    return pack(jitter(unpackA(hsvColor), amt), unpackB(hsvColor), unpackC(hsvColor));
}

int HSVShift(int hsvColor, int amt) {
    return pack(unpackA(hsvColor) + amt, unpackB(hsvColor), unpackC(hsvColor));
}

int h_angle = 0;

int popRainbow(int h_rate) {
    h_angle += h_rate;

    int toRet = getRGB(h_angle, 255, 255);

    if (h_angle >= 255) {
        h_angle = 0;
    }
    return toRet;
}

int popHSVRainbow(int h_rate) {
    h_angle += h_rate;

    int toRet = pack(h_angle, 255, 255);

    if (h_angle >= 255) {
        h_angle = 0;
    }
    return toRet;
}

int invertHSV(int hsvColor) {
    int temp = unpackA(hsvColor);
    temp = temp + 126;
    if (temp > 255) {
        temp -= 255;
    }
    return pack(temp, unpackB(hsvColor), unpackC(hsvColor));
}

int cycle = 0;

int popCycle(int rate) {
    cycle += rate;
    return cycle;
}



//===================
// Animation code!
//
// The theory is that all of these animations will need to take in some input from the touch screen.
// We will need global variables that all of these functions can see.
// We will need pointers to these global objects that can be handed in to the event listeners so that
// the event data can be sent over to these functions.

int totalPixels = TCLControl::nStrands * TCLControl::pixelsPerStrand;
int memSize = totalPixels * sizeof(TCpixel);
TCpixel *pixelBackupBuf = (TCpixel *) malloc(memSize);

gfloat finput_x;
gfloat finput_y;
int input_x = 1;
int input_y = 1;
int old_x, old_y;

int W, H;
int timeout = 0;
bool clearing = false;

void animation10(TCLControl *tcl) {
    cutoff += 1;
    if (cutoff > (input_y * 50 / 255)) {
        cutoff = 0;
        W = getrand(0, WIDTH);
        H = getrand(0, HEIGHT);
    }

    timeout += 1;
    if (timeout > (input_x * 100 / 255)) {
        timeout = 0;
        clearing = true;
    }

    // int thresh = 4;

    int temp = popHSVRainbow(1);
    int inverted_temp = invertHSV(temp);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            if (abs(W - x) < 2 && abs(H - y) < 2) {
                tcl->pixelBuf[index] = getRGB(temp);
            } else {
                if (clearing) {
                    tcl->pixelBuf[index] = getRGB(inverted_temp);
                }
            }
            index += 1;
        }
    }
    if (clearing) {
        clearing = false;
    }
}

void animation9(TCLControl *tcl) {
    // printf("input_x: %i \ninput_y: %i\n\n", input_x, input_y);
    // int jitterAmount = input_y*50/255;
    // printf( "Jitter: %i\n", jitterAmount);

    int rainbowPass = popHSVRainbow(input_y * 50 / 255);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            pixelBackupBuf[index] = tcl->pixelBuf[index];
            index += 1;
        }
    }

    index = 0;
    for (int x = 0; x < WIDTH; x++) {
        // int wandOffset = input_x*11/255;
        for (int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            int hsvColor = rainbowPass; //HSVJitter(rainbowPass, jitterAmount);
            int jitColor = getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));

            if (y >= 1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index - 1];
            } else {
                if (input_x == old_x && input_y == old_y) {
                    tcl->pixelBuf[index] = tcl->pixelBuf[index + 49];
                } else {
                    if (x == input_x * 12 / 255) {
                        tcl->pixelBuf[index] = jitColor;
                    }
                }


            }

            index += 1;
        }
    }
    old_x = input_x;
    old_y = input_y;
}

void animation8(TCLControl *tcl) {
    // printf("input_x: %i \ninput_y: %i\n\n", input_x, input_y);
    // int jitterAmount = input_y*50/255;
    // printf( "Jitter: %i\n", jitterAmount);

    int rainbowPass = popHSVRainbow(input_x * 10 / 255);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            int hsvColor = rainbowPass; //HSVJitter(rainbowPass, jitterAmount);
            int jitColor = getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));

            if (y >= 1 && y < input_y * 50 / 255) {
                tcl->pixelBuf[index] = pixelBackupBuf[index - 1];
            } else if (y >= input_y * 50 / 255) {
                if (x == 11) {
                    tcl->pixelBuf[index] = pixelBackupBuf[index - 49];
                } else {
                    tcl->pixelBuf[index] = pixelBackupBuf[index + 1];
                }
            } else {
                tcl->pixelBuf[index] = jitColor;
            }

            index += 1;
        }
    }
}

void animation7(TCLControl *tcl) {

    // Handle inputs:
    int rate = input_y * 50 / 255;
    int skew = popCycle(input_x * 50 / 255);
    rate = rate - rate / 2;

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        // h_angle = 0;
        for (int y = 0; y < HEIGHT; y++) {


            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            tcl->pixelBuf[index] = getRGB(HSVShift(popHSVRainbow(rate), skew));

            index += 1;
        }
    }
}

// Old light update:
// Calculate the new values for the pixelBuf:
// Update the lights:
// x += (double)tcl.pixelsPerStrand / 20000.0;
// s1 = sin(x                 ) *  11.0;
// s2 = sin(x *  0.857 - 0.214) * -13.0;
// s3 = sin(x * -0.923 + 1.428) *  17.0;
// for(i=0;i<tcl.totalPixels;i++)
// {
//     r   = (int)((sin(s1) + 1.0) * 127.5);
//     g   = (int)((sin(s2) + 1.0) * 127.5);
//     b   = (int)((sin(s3) + 1.0) * 127.5);
//     tcl.pixelBuf[i] = TCrgb(r,g,b);
//     s1 += 0.273;
//     s2 -= 0.231;
//     s3 += 0.428;
// }

void animation6(TCLControl *tcl) {
    // printf("input_x: %i \ninput_y: %i\n\n", input_x, input_y);
    int jitterAmount = input_y * 50 / 255;
    // printf( "Jitter: %i\n", jitterAmount);

    int rainbowPass = popHSVRainbow(input_x * 10 / 255);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            int hsvColor = HSVJitter(rainbowPass, jitterAmount);
            int jitColor = getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));

            if (y >= 1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index - 1];
            } else {
                tcl->pixelBuf[index] = jitColor;
            }

            index += 1;
        }
    }
}

int tempHSV;

void animation5(TCLControl *tcl) {
     // printf("input_x: %i \ninput_y: %i\n\n", input_x, input_y);
     if (input_x != old_x || input_y != old_y) {
         tempHSV = pack(input_y, 255, 255);
     } else {
         tempHSV = HSVJitter(tempHSV, input_x*5/255);
     }


     int index = 0;
     for(int x = 0; x < WIDTH; x++) {
         for(int y = 0; y < HEIGHT; y++) {

             // // This next line grabs the address of single pixel out of the pixels char buffer
             // // and points a char at it so that it's value can be set:
             // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

             tcl->pixelBuf[index] = getRGB(tempHSV);

             index += 1;
         }
     }

     old_x = input_x;
     old_y = input_y;
}

void animation4(TCLControl *tcl) {

    int temp = popRainbow(input_y * 100 / 255);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            if (y == input_y * WIDTH / 255) {
                tcl->pixelBuf[index] = temp;
            } else {
                tcl->pixelBuf[index] = tcl->pixelBuf[index ^ (input_x * HEIGHT / 255)] + temp;
            }

            // tcl->pixelBuf[index] = temp;

            index += 1;
        }
    }
}


void animation3(TCLControl *tcl) {
    cutoff += 1;
    if (cutoff > (input_y * 50 / 255)) {
        cutoff = 0;

        for (int i = 0; i < tcl->totalPixels; i++) {
            // int temp = TCrgb(200,200,10);
            // int temp = (200<<16)|(200<<8)|10;
            // int temp = getRGB(0,255,255);
            int temp = popRainbow(input_x * 10 / 255);
            tcl->pixelBuf[i] = temp;
        }
    }
}

// int seedHSVColor = getHSVRandomColor();

TCpixel *HSVBuf = (TCpixel *) malloc(memSize);
bool set = false;

void animation2(TCLControl *tcl) {

    if (!set) {
        for (int i = 0; i < tcl->totalPixels; i++) {
            HSVBuf[i] = pack(255, 255, 255);
        }
        set = true;
    }

    // int nextSeed = getrand(0,tcl->totalPixels);
    // int tempColor;

    // for(int i=0; i < tcl->totalPixels; i++) {
    //     tempColor = HSVJitter(seedHSVColor, 1);
    //     tcl->pixelBuf[i] = getRGB(unpackA(tempColor), unpackB(tempColor), unpackC(tempColor));
    // }

    // seedHSVColor = tcl->pixelBuf[nextSeed];

    // int temp = getBetterRandomColor();

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            HSVBuf[index] = HSVJitter(HSVBuf[index], input_y * 50 / 255);

            index += 1;
        }
    }

    // index = 0;
    // for(int x = 0; x < WIDTH; x++) {
    //     for(int y = 0; y < HEIGHT; y++) {

    //         // pixelBackupBuf[index] = tcl->pixelBuf[index];

    //         if (y>=1) {
    //             tcl->pixelBuf[index] = pixelBackupBuf[index-1];
    //         } else {
    //             tcl->pixelBuf[index] = temp;
    //         }

    //         index += 1;
    //     }
    // }

    for (int i = 0; i < tcl->totalPixels; i++) {
        tcl->pixelBuf[i] = getRGB(unpackA(HSVBuf[i]), unpackB(HSVBuf[i]), unpackC(HSVBuf[i]));
    }

}


int tempColor;
int counter = 5;

void animation1(TCLControl *tcl) {

    counter += 1;
    if (counter > (input_y * 50 / 255)) {
        int sat = input_x;
        tempColor = getBetterRandomColor(sat);
        counter = 0;
    }

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            if (y >= 1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index - 1];
            } else {
                tcl->pixelBuf[index] = tempColor;
            }

            index += 1;
        }
    }
}

// End Animation Code
//===================

gboolean handleTouchEvents(ClutterActor *actor, ClutterEvent *event, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    TouchData *data;
    data = (TouchData *) user_data;

    // ClutterActor *lightDisplay = CLUTTER_ACTOR (data->lightDisplay);
    // TCLControl *tcl = data->tcl;
    // int *animation_number = data->animationNumber;
    // gfloat *input_x = data->input_x;
    // gfloat *input_y = data->input_y;

    ClutterEventType eventType = clutter_event_type(event);
    gfloat stage_x, stage_y;
    gfloat actor_x = 0, actor_y = 0;

    if (eventType == CLUTTER_TOUCH_END) {
        // printf("Touch end!\n");

    } else if (eventType == CLUTTER_TOUCH_UPDATE || eventType == CLUTTER_MOTION) {
        clutter_event_get_coords(event, &stage_x, &stage_y);
        clutter_actor_transform_stage_point(actor, stage_x, stage_y, &actor_x, &actor_y);

        // Now we have some x,y coordinates we can throw back at those animations!
        //  but we should probably scale them now since we have all the stuff we need
        //  to do so in this block...
        //
        // This will scale both to 0.0 <-> 255.0:
        actor_x = actor_x / clutter_actor_get_width(actor) * 255;
        actor_y = actor_y / clutter_actor_get_height(actor) * 255;

        // printf("Touch Move!!\nx: %f\ny: %f\n\n", actor_x, actor_y );
        input_x = static_cast<int>(actor_x);
        input_y = static_cast<int>(actor_y);

    } else {
        // printf("Some other touch event %i\n", eventType);
    }

    return CLUTTER_EVENT_STOP;
}


// handleNewFrame is the function that is called ever 120 milliseconds via the timeline!
// This is where ALL animation updates will happen.
void handleNewFrame(ClutterActor *timeline, gint frame_num, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    AnimationData *data;
    data = (AnimationData *) user_data;

    ClutterActor *rotatingActor = CLUTTER_ACTOR(data->rotatingActor);
    // ClutterActor *infoDisplay = CLUTTER_ACTOR (data->infoDisplay);
    TCLControl *tcl = data->tcl; // tcl is STILL a pointer to the main TCLControl object

    // We hand in the ADDRESS of the current Animation:
    int *animation_number = data->animationNumber;

    // Update the spinning rectangle:
    rotation += 0.2;
    clutter_actor_set_rotation_angle(rotatingActor, CLUTTER_Z_AXIS, rotation * 5);

    // Run which ever animation we're on:
    switch (*animation_number) {
        case 1  :
            animation1(tcl);
            break;
        case 2  :
            animation2(tcl);
            break;
        case 3  :
            animation3(tcl);
            break;
        case 4  :
            animation4(tcl);
            break;
        case 5  :
            animation5(tcl);
            break;
        case 6  :
            animation6(tcl);
            break;
        case 7  :
            animation7(tcl);
            break;
        case 8  :
            animation8(tcl);
            break;
        case 9  :
            animation9(tcl);
            break;
        case 10  :
            animation10(tcl);
            break;

        default :
            animation5(tcl);
    }


    int fbWidth = 2;//(10*4);
    int fbHeight = 2;
    int pixelLength = 4; // 4 bytes per pixel in the FB (r,g,b,a)
    cogl_read_pixels(   0, // start x
                        50, // stary y
                        fbWidth,  // width (4 bytes per pixel)
                        fbHeight, // height in pixels
                        COGL_READ_PIXELS_COLOR_BUFFER,
                        COGL_PIXEL_FORMAT_RGBA_8888,
                        shaderBuffer);

    printf("Here's the data we pulled from the FB:\n");
    for (int i = 0; i < (fbWidth*fbHeight)*pixelLength; i++) {
        if (i%pixelLength == 0 && i != 0) {
            printf(", ");
        }
        if (i%(fbWidth*pixelLength) == 0 && i != 0) {
            printf("\n");
        }

        printf("%i ", shaderBuffer[i]); // Print out the value of this byte in the pixel
    }
    printf("\nDone!\n");



    // Send the updated color buffer to the strands
    if (tcl->enabled) {
        tcl->Update();
    }

//    // Update the on screen color display using the colors FROM THE PRE-LIGHT ARRAY ITSELF!
//    // Draw onscreen color map display:
//    int index = 0;
//    for (int x = 0; x < WIDTH; x++) {
//        for (int y = 0; y < HEIGHT; y++) {
//
//            // This next line grabs the address of single pixel out of the pixels char buffer
//            // and points a char at it so that it's value can be set:
//            unsigned char *pixel = &pixels[y * rowstride + x * 3];
//
//            TCpixel thisPixel = tcl->pixelBuf[index];
//            // #define TCrgb(R,G,B) (((R) << 16) | ((G) << 8) | (B))
//            pixel[0] = ((thisPixel) >> 16) & 0xff;//red
//            pixel[1] = ((thisPixel) >> 8) & 0xff;//green
//            pixel[2] = (thisPixel) & 0xff;//blue
//            index += 1;
//        }
//    }
//    if (pixbuf != NULL) {
//        // THIS actually draws the image on the screen.
//       clutter_image_set_data(CLUTTER_IMAGE(colors),
//                           gdk_pixbuf_get_pixels (pixbuf),
//                           COGL_PIXEL_FORMAT_RGB_888,
//                           gdk_pixbuf_get_width (pixbuf),
//                           gdk_pixbuf_get_height (pixbuf),
//                           gdk_pixbuf_get_rowstride (pixbuf),
//                           &error);
//    }
//    clutter_actor_set_content(lightDisplay, colors);

    animationTime += 1.0;
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iGlobalTime", G_TYPE_FLOAT, 1,
                                      animationTime);
}


Animation::Animation(ClutterActor *stage, ClutterActor *rotatingActor, TCLControl *tcl,
                     ClutterActor *infoDisplay) { //TCLControl tcl
    printf("Building animation tools...\n");

    rect = rotatingActor;

    // First, we need some Actor to actually display the light colors:
    colors = clutter_image_new();
    lightDisplay = clutter_actor_new();

    error = NULL;

    // Load image data from some other data source...:
    // guchar *data =
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(data);

    // Load image data from a file:
    // const char *img_path = "./wut.png";
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size (img_path, WIDTH, HEIGHT, &error);


    // Load image data from nothing. Build it manually.
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, WIDTH, HEIGHT);
    // Then grab all of the pixels from that blank pixel buffer:
    pixels = gdk_pixbuf_get_pixels(pixbuf);
    // And figure out how many bytes wide that pixel buffer actually is.
    rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    // ToDo: Uncomment shader stuff
    // Allocate the memory for the shader output buffer:
    // Figure out how big our buffer needs to be. *3 because three bytes per pixel (r, g, b)
    int shaderBufferSize = ceil(clutter_actor_get_width(stage)) * ceil(clutter_actor_get_height(stage)) * 3;
    // Actually malloc the buffer!!
    shaderBuffer = (guint8 *) malloc(shaderBufferSize);

    // Double check that we built the buffer correctly.
    if (shaderBuffer == NULL) {
        printf("OOPS! malloc error!\n");
    } else {
        printf("We malloc'd %i bytes for your shaderBuffer. It points at: %p\n", shaderBufferSize, shaderBuffer);
        printf(" The Rowstride on the shaderBuffer is %i", rowstride);
    }

    // Loop through the (blank) image we just made...
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // Find the address of each pixel in turn...
            // This next line grabs the address of single pixel out of the pixels char buffer
            // and points a char at it so that it's value can be set:
            unsigned char *pixel = &pixels[y * rowstride + x * 3];

            // And set that specific pixel's color to red.
            pixel[0] = 255;//red
            pixel[1] = 0x0;//green
            pixel[2] = 0x0;//blue
        }
    }

    // Assuming the buffer is defined, throw the image at the on screen display:
    if (pixbuf != NULL) {
//        clutter_image_set_data(CLUTTER_IMAGE(colors),
//                            gdk_pixbuf_get_pixels (pixbuf),
//                            COGL_PIXEL_FORMAT_RGB_888,
//                            gdk_pixbuf_get_width (pixbuf),
//                            gdk_pixbuf_get_height (pixbuf),
//                            gdk_pixbuf_get_rowstride (pixbuf),
//                            &error);
    }


    // Actually build the on screen color display/Shader output display Actor:
#define THUMBNAIL_SIZE 30
    clutter_actor_set_x_expand(lightDisplay, TRUE);
    clutter_actor_set_y_expand(lightDisplay, TRUE);
    clutter_actor_set_position(lightDisplay, 0, 50);
    clutter_actor_set_size(lightDisplay, WIDTH * osd_scale, HEIGHT * osd_scale);
    clutter_actor_set_rotation_angle(lightDisplay, CLUTTER_Z_AXIS, -90);
    clutter_actor_set_rotation_angle(lightDisplay, CLUTTER_Y_AXIS, 180);


    // Allow for UI events on this crazy thing!
    clutter_actor_set_reactive(lightDisplay, TRUE);

    // Wire up the event listener on this lightDisplay actor. This is the data object layout:
    // typedef struct  {
    //     ClutterActor *lightDisplay;
    //     TCLControl *tcl;
    //     int *animationNumber;
    // } TouchData;
    //
    // Build a pointer to a struct in that format that we can pass through the g_signal_connect function
    // and into the event handler:
    TouchData *touch_data;
    touch_data = g_slice_new(TouchData); // reserve memory for it...
    touch_data->lightDisplay = lightDisplay; // Place the button actor itself inside the struct
    touch_data->tcl = tcl; // TCLControl *tcl is just a POINTER here (unlike in main.cpp)
    touch_data->animationNumber = &currentAnimation;

    // To get touch values back OUT of the callback, we will use variables in the global scope.
    // This is what was left over from my attempts at handing data back out... but, per usual,
    // I mess up my callback structures and forgot that calling things async = LOSING CONTEXT!!
    // input_x = 0;
    // input_y = 0;
    // touch_data->input_x = input_x;
    // touch_data->input_y = input_y;

    // Actually wire up the events and set up the data structs that the events need to operate:
    g_signal_connect(lightDisplay, "touch-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "button-press-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "motion-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "button-release-event", G_CALLBACK(handleTouchEvents), touch_data);



    // Set the on screen light display to the image that we hard coded to red earlier in this function..
    clutter_actor_set_content(lightDisplay, colors);

    //=============
    // NOW we can just IGNORE that "color" image/object that we built... because we're just going to blow
    // it all away with colors from a shader anyways!
    //
    // Setup the GLSL Fragment shaders that we'll use to generate colors
    // Build a GLSL Fragment shader to affect the color output (to the screen at least for now)
    shaderEffect = clutter_shader_effect_new(CLUTTER_FRAGMENT_SHADER);
    clutter_shader_effect_set_shader_source(CLUTTER_SHADER_EFFECT(shaderEffect), fragShader);

    // Bind uniforms to the shader so we can hand variables into them
    // ToDo: Figure out what uniforms we'll need to implement to get Shader Toys to import cleanly!!
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iGlobalTime", G_TYPE_FLOAT, 1, 100.0);
//    clutter_shader_effect_set_uniform (CLUTTER_SHADER_EFFECT (shaderEffect), "factor", G_TYPE_FLOAT, 1, 0.66);

    // Set the effect live on the on screen display actor... and then add that actor to the stage!
    clutter_actor_add_effect(lightDisplay, shaderEffect);
    clutter_actor_add_child(stage, lightDisplay);

    // End shader stuff
    //=============


    // Once we have all that set up, we still need to START THE ACTUAL ANIMATION!!
    // To do that, we'll need to use the event chain/callback system we have been using so far. This
    // will take in some actor instances that the event handler can then update/modify in real time.
    //
    // Get ready to hand this display chunk in to the animation event:
    AnimationData *data;
    data = g_slice_new(AnimationData); // reserve memory for it...
    data->rotatingActor = rect;
    data->infoDisplay = infoDisplay;
    data->tcl = tcl; // tcl is an pointer to the main TCLControl object.
    data->animationNumber = &currentAnimation;

    // The clutter timeline object takes a "duration" in milliseconds...
    timeline = clutter_timeline_new(120);
    g_signal_connect(timeline, "new-frame", G_CALLBACK(handleNewFrame), data);

    // which will just continue repeating:
    clutter_timeline_set_repeat_count(timeline, -1);

    // This actually starts the timeline animation!
    clutter_timeline_start(timeline);



    // But we ALSO want to go in and start pulling data FROM the screen and dumping it into the lights:
    //
    // The animation loop itself will have to know when the frame buffers are swapping. To do this
    // we apparently nead to listen to the "after-paint" event on the main scene object.
    //
    // Once the "after-paint" event fires, we will need to set a state on the Animation to tell it that
    // there's data to read from the shaderBuffer. The Animation object will grab the pixel data from
    // the shadders buffer, then shove it into a buffer we can hold on to until the Animation loop is
    // ready to draw the scene and to dump the colors to the lights.
    //
    // Set up the data storage to hand a pointer to the main Animation object into the event handler:
//    EventDataAfterPaint *dataAfterPaint;
//    dataAfterPaint = g_slice_new(EventDataAfterPaint); // reserve memory for it...
////    dataAfterPaint->animation = &animation; // Place the current Animation into the struct that will be handed to the event handler
//    dataAfterPaint->tcl = tcl; // pointer TO the main tcl object.
//
//    // Setup the listener for the after-paint event so we know when we can read from the shader texture:
//    g_signal_connect(stage, "after-paint", G_CALLBACK(handleAfterPaint), dataAfterPaint);
}

Animation::~Animation() {
    // These fail for some reason. Not sure why...
    g_object_unref(colors);
    g_object_unref(pixbuf);
//    g_object_unref(shaderBuffer);
}

Animation::Animation() {
}

void Animation::switchAnimation(int animationNumber, ClutterActor *infoDisplay) {
    printf("Changing to animation: %i\n", animationNumber);
    currentAnimation = animationNumber;
    clutter_text_set_text(CLUTTER_TEXT(infoDisplay), g_strdup_printf("Current sensor input: %i", currentAnimation));
}

int Animation::getCurrentAnimation() {
    printf("Called! Current animation is: %i\n", currentAnimation);
    return currentAnimation;
}

gboolean Animation::handleAfterPaint (ClutterActor *actor,
                                   ClutterEvent *event,
                                   gpointer      user_data) {

    // Rebuild the struct from the pointer we handed in:
    EventDataAfterPaint *dataAfterPaint;
    dataAfterPaint = (EventDataAfterPaint *)user_data;

//    Animation *animation = dataAfterPaint->animation;
    TCLControl *tcl = dataAfterPaint->tcl;

    //========================
    // Start of shaderGrab rewrite:

    // This grabs from the WHOLE screen!
    // maybe we can use that to our advantage...?
    //
    // The shader is dumping to the screen at (x,y)=(0,50)
    // This is the onscreen display...
    // On screen display size is:
    // WIDTH * osd_scale, HEIGHT * osd_scale
    int fbWidth = 2;//(10*4);
    int fbHeight = 2;
    int pixelLength = 4; // 4 bytes per pixel in the FB (r,g,b,a)
    cogl_read_pixels(   0, // start x
                        50, // stary y
                        fbWidth,  // width (4 bytes per pixel)
                        fbHeight, // height in pixels
                        COGL_READ_PIXELS_COLOR_BUFFER,
                        COGL_PIXEL_FORMAT_RGBA_8888,
                        shaderBuffer);

    printf("Here's the data we pulled from the FB:\n");
    for (int i = 0; i < (fbWidth*fbHeight)*pixelLength; i++) {
        if (i%pixelLength == 0 && i != 0) {
            printf(", ");
        }
        if (i%(fbWidth*pixelLength) == 0 && i != 0) {
            printf("\n");
        }

        printf("%i ", shaderBuffer[i]); // Print out the value of this byte in the pixel
    }
    printf("\nDone!\n");


    // When we pull data from the on screen display, we will need to
    // know the full size of the on screen shader display so we know where
    // to grab the pixel colors from.
    //
    // Full screen size data is located in configurations.h
    // On screen display size is defined in main.cpp

//    int ledIndex = 0; // This is the pixel offset in the actual color array. uint32_t
//    int fbIndex = 0;  // This is the pixel BYTE offset in the
//    for (int x = 0; x < WIDTH; x++) {
//        for (int y = 0; y < HEIGHT; y++) {
//            uint32_t nextColor = pack(shaderBuffer[fbIndex],shaderBuffer[fbIndex+1],shaderBuffer[fbIndex+2]);
//            fbIndex += 4;
//
////            tcl->pixelBuf[ledIndex] = nextColor;
//            tcl->pixelBuf[ledIndex] = getRandomColor();
//            ledIndex += 1;
//        }
//    }

//    tcl->pixelBuf[0] = getRandomColor();
    printf("TCL = %i\n", tcl->pixelsPerStrand);
    printf("TCL = %u\n", tcl->pixelBuf[0]);

    return CLUTTER_EVENT_STOP;
}

// ToDo: Uncomment shader stuff:
void Animation::grabShaderColors(TCLControl *tcl) {
//    // This grabs from the WHOLE screen!
//    // maybe we can use that to our advantage...?
//    //
//    // The shader is dumping to the screen at (x,y)=(0,50)
//    // This is the onscreen display...
//    // On screen display size is:
//    // WIDTH * osd_scale, HEIGHT * osd_scale
//    int fbWidth = 2;//(10*4);
//    int fbHeight = 2;
//    int pixelLength = 4; // 4 bytes per pixel in the FB (r,g,b,a)
//    cogl_read_pixels(   0, // start x
//                        50, // stary y
//                        fbWidth,  // width (4 bytes per pixel)
//                        fbHeight, // height in pixels
//                        COGL_READ_PIXELS_COLOR_BUFFER,
//                        COGL_PIXEL_FORMAT_RGBA_8888,
//                        shaderBuffer);
//
//    printf("Here's the data we pulled from the FB:\n");
//    for (int i = 0; i < (fbWidth*fbHeight)*pixelLength; i++) {
//        if (i%pixelLength == 0 && i != 0) {
//            printf(", ");
//        }
//        if (i%(fbWidth*pixelLength) == 0 && i != 0) {
//            printf("\n");
//        }
//
//        printf("%i ", shaderBuffer[i]); // Print out the value of this byte in the pixel
//    }
//    printf("\nDone!\n");


//    // When we pull data from the on screen display, we will need to
//    // know the full size of the on screen shader display so we know where
//    // to grab the pixel colors from.
//    //
//    // Full screen size data is located in configurations.h
//    // On screen display size is defined in main.cpp
//
//    int ledIndex = 0; // This is the pixel offset in the actual color array. uint32_t
//    int fbIndex = 0;  // This is the pixel BYTE offset in the
//    for (int x = 0; x < WIDTH; x++) {
//        for (int y = 0; y < HEIGHT; y++) {
//             uint32_t nextColor = pack(shaderBuffer[fbIndex],shaderBuffer[fbIndex+1],shaderBuffer[fbIndex+2]);
//             fbIndex += 4;
//
//             tcl->pixelBuf[ledIndex] = nextColor;
//             ledIndex += 1;
//        }
//    }

//    tcl->pixelBuf[0] = getRandomColor();








    // IGNORE ALL THIS:
// Trying to be all smart 'n shit:
//    ClutterOffscreenEffect *offscreen_effect = CLUTTER_OFFSCREEN_EFFECT (shaderEffect);
//    CoglHandle shaderBufferHandle = clutter_offscreen_effect_get_texture(offscreen_effect);

//    int copySize = 0;

//    cogl_read_pixels(   0, // start x
//                        0, // stary y
//                        cogl_texture_get_width (shaderBufferHandle),  // width
//                        cogl_texture_get_height (shaderBufferHandle), // height
//                        COGL_READ_PIXELS_COLOR_BUFFER,
//                        COGL_PIXEL_FORMAT_RGBA_8888,
//                        shaderBuffer);



//    copySize = cogl_texture_get_data(shaderBufferHandle,
//                        COGL_PIXEL_FORMAT_RGB_888,
//                        1,
//                        shaderBuffer);

//    printf("copy-size: %i\n", copySize);
}

