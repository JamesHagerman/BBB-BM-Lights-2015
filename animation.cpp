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
#define TCrgb(R,G,B) (((R) << 16) | ((G) << 8) | (B))

typedef struct  {
    ClutterActor *rotatingActor;
    ClutterActor *infoDisplay;
    TCLControl *tcl;
    int *animationNumber;
} AnimationData;

typedef struct  {
    ClutterActor *lightDisplay;
    TCLControl *tcl;
    int *animationNumber;
} TouchData;

gdouble rotation = 0;
int cutoff = 0;

// Hold on to the Color Display:
ClutterContent *colors;
ClutterActor *lightDisplay;
GdkPixbuf *pixbuf;
unsigned char* pixels;
int rowstride;
// We need an error object to store errors:
GError *error;

int osd_scale = 16;

int AnimationID = 0;


//=======================
// Helpful color tools:

// convert HSB to RGB:
int rgb_colors[3]; // holder for rgb color from hsb conversion
int hue;
int saturation;
//int brightness; // defined above, set using AN3
const unsigned char dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};
int getRGB(int hue, int sat, int val) { 
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/value and on saturation (inverted).
     This looks the most natural.      
  */

  val = dim_curve[val];
  sat = 255-dim_curve[255-sat];

  int colors[3];

  int r = 0;
  int g = 0;
  int b = 0;
  int base;

  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;  
  } else  { 

    base = ((255 - sat) * val)>>8;

    switch(hue/60) {
    case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
    break;

    case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
    break;

    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
    break;

    case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
    break;

    case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
    break;

    case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
    break;
    }

    colors[0]=r;
    colors[1]=g;
    colors[2]=b; 
  }  

  // unsigned char toRet = TCrgb(r, g, b);
  return TCrgb(r, g, b);

}

// Unpack and packing colors into and out of int
int pack(int a, int b, int c) { return (a<<16) | (b<<8) | c; }
int unpackA(int color) { return (color>>16)&0xff; }
int unpackB(int color) { return (color>>8)&0xff; }
int unpackC(int color) { return (color)&0xff; }


// Random functions: 
int getrand(int min,int max){
     return (rand()%(max-min)+min);
}  
int randColor() {
    return getrand(0,255);
}
int getRandomColor(){
    return TCrgb(randColor(), randColor(), randColor());
}
int getBetterRandomColor(){
    return getRGB(randColor(), 255, 255);
}
int getHSVRandomColor(){
    return pack(randColor(), 255, 255);
}
int jitter(int toJitter, int jitterAmount) {
    return getrand(toJitter-jitterAmount, toJitter+jitterAmount);
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

    toRet = (r<<16) | (g<<8) | b;

    return toRet;
}

int HSVJitter(int hsvColor, int amt) {
    return pack(jitter(unpackA(hsvColor), amt), unpackB(hsvColor), unpackC(hsvColor));
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



//===================
// Animation code!
//
// The theory is that all of these animations will need to take in some input from the touch screen.
// We will need global variables that all of these functions can see.
// We will need pointers to these global objects that can be handed in to the event listeners so that
// the event data can be sent over to these functions.

int totalPixels = TCLControl::nStrands * TCLControl::pixelsPerStrand;
int memSize = totalPixels * sizeof(TCpixel);
TCpixel *pixelBackupBuf = (TCpixel *)malloc(memSize);

void animation10(TCLControl *tcl) {

}
void animation9(TCLControl *tcl) {

}
void animation8(TCLControl *tcl) {

}
void animation7(TCLControl *tcl) {

}

void animation6(TCLControl *tcl) {

    int rainbowPass = popHSVRainbow(10);
    int jitterAmount = 40;

    int index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            int hsvColor = HSVJitter(rainbowPass, jitterAmount);
            int jitColor = getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));

            if (y>=1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index-1];
            } else {
                tcl->pixelBuf[index] = jitColor;
            }

            index += 1;
        }
    }
}


void animation5(TCLControl *tcl) {

    int temp = popRainbow(10);

    int index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            if (y>=1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index-1];
            } else {
                tcl->pixelBuf[index] = temp;
            }

            index += 1;
        }
    }
}

void animation4(TCLControl *tcl) {

    int temp = popRainbow(10);

    int index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            if (y == 0) {
                tcl->pixelBuf[index] = temp;
            } else if (y == 49) {
                tcl->pixelBuf[index] = getBetterRandomColor();
            } else if (x == 10) {
                tcl->pixelBuf[index] = tcl->pixelBuf[index+x];
            } else {
                tcl->pixelBuf[index] = tcl->pixelBuf[index+x];
            }

            // tcl->pixelBuf[index] = temp;

            index += 1;
        }
    }
}


void animation3(TCLControl *tcl) {
    cutoff += 1;
    if (cutoff > 5) {
        cutoff = 0;

        for(int i=0; i < tcl->totalPixels; i++) {
            // int temp = TCrgb(200,200,10);
            // int temp = (200<<16)|(200<<8)|10;
            // int temp = getRGB(0,255,255);
            int temp = popRainbow(1);
            tcl->pixelBuf[i] = temp;
        }
    }
}

// int seedHSVColor = getHSVRandomColor();

TCpixel *HSVBuf = (TCpixel *)malloc(memSize);
bool set = false;

void animation2(TCLControl *tcl) {

    if (!set) {
        for(int i=0; i < tcl->totalPixels; i++) {
            HSVBuf[i] = pack(255,255,255);
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
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            HSVBuf[index] = HSVJitter(HSVBuf[index], 10);

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

    for(int i=0; i < tcl->totalPixels; i++) {
        tcl->pixelBuf[i] = getRGB(unpackA(HSVBuf[i]), unpackB(HSVBuf[i]), unpackC(HSVBuf[i]));
    }

}


void animation1(TCLControl *tcl) {
    int temp = getBetterRandomColor();

    int index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            if (y>=1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index-1];
            } else {
                tcl->pixelBuf[index] = temp;
            }

            index += 1;
        }
    }
}

// End Animation Code
//===================

gboolean handleTouchEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    TouchData *data;
    data = (TouchData *)user_data;

    // ClutterActor *lightDisplay = CLUTTER_ACTOR (data->lightDisplay);
    // TCLControl *tcl = data->tcl;
    // int *animation_number = data->animationNumber;

    ClutterEventType eventType = clutter_event_type(event);
    gfloat stage_x, stage_y;
    gfloat actor_x = 0, actor_y = 0;

    if (eventType == CLUTTER_TOUCH_END) {
        // printf("Touch end!\n");

    } else if (eventType == CLUTTER_TOUCH_UPDATE || eventType == CLUTTER_MOTION) {
        clutter_event_get_coords (event, &stage_x, &stage_y);
        clutter_actor_transform_stage_point (actor, stage_x, stage_y, &actor_x, &actor_y);

        // Now we have some x,y coordinates we can throw back at those animations!
        //  but we should probably scale them now since we have all the stuff we need
        //  to do so in this block...
        //
        // This will scale both to 0.0 <-> 1.0:
        actor_x = actor_x/clutter_actor_get_width(actor);
        actor_y = actor_y/clutter_actor_get_height(actor);

        printf("Touch Move!!\nx: %f\ny: %f\n\n", actor_x, actor_y );

    } else {
        // printf("Some other touch event %i\n", eventType);
    }

    return CLUTTER_EVENT_STOP;
}


void handleNewFrame (ClutterActor *timeline, gint frame_num, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    AnimationData *data;
    data = (AnimationData *)user_data;

    ClutterActor *rotatingActor = CLUTTER_ACTOR (data->rotatingActor);
    // ClutterActor *infoDisplay = CLUTTER_ACTOR (data->infoDisplay);
    TCLControl *tcl = data->tcl;
    int *animation_number = data->animationNumber;


    // Update the spinning rectangle:
    rotation += 0.2;
    clutter_actor_set_rotation_angle(rotatingActor, CLUTTER_Z_AXIS, rotation * 5);


    // Update the actual lights with some new color (Gotta figure out how to build animations out...):
    // cutoff += 1;
    // if (cutoff > 600) {
    //     cutoff = 0;
    // }

    // for(int i=0; i < tcl->totalPixels; i++) {
    //     if ( i > cutoff) {
    //         tcl->pixelBuf[i] = TCrgb(0,255,0);
    //     } else {
    //         tcl->pixelBuf[i] = TCrgb(255,0,255);
    //     }
    // }

    switch(*animation_number){
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
           animation6(tcl);
    }

    

    // Send the updated buffer to the strands
    if (tcl->enabled) {
        tcl->Update();
    }


    // Update the on screen color display:
    // Draw onscreen color map display:
    // Try changing the color right after setting it to red:
    int index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // This next line grabs the address of single pixel out of the pixels char buffer
            // and points a char at it so that it's value can be set:
            unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            TCpixel thisPixel = tcl->pixelBuf[index];
            // #define TCrgb(R,G,B) (((R) << 16) | ((G) << 8) | (B))
            pixel[0] = ((thisPixel) >> 16) & 0xff;//red
            pixel[1] = ((thisPixel) >> 8) & 0xff;//green
            pixel[2] = (thisPixel) & 0xff;//blue
            index += 1;
        }
    }
    if (pixbuf != NULL) {
        clutter_image_set_data(CLUTTER_IMAGE(colors),
                            gdk_pixbuf_get_pixels (pixbuf),
                            COGL_PIXEL_FORMAT_RGB_888,
                            gdk_pixbuf_get_width (pixbuf),
                            gdk_pixbuf_get_height (pixbuf),
                            gdk_pixbuf_get_rowstride (pixbuf),
                            &error);
    }

    clutter_actor_set_content(lightDisplay, colors);


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
}



Animation::Animation(ClutterActor *stage, ClutterActor *rotatingActor, TCLControl *tcl, ClutterActor *infoDisplay){ //TCLControl tcl
    printf("Building animation tools...\n");

    rect = rotatingActor;


    // Add a colored texture to the app:
    //
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
    pixels = gdk_pixbuf_get_pixels(pixbuf);

    rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // This next line grabs the address of single pixel out of the pixels char buffer
            // and points a char at it so that it's value can be set:
            unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixel[0] = 255;//red
            pixel[1] = 0x0;//green
            pixel[2] = 0x0;//blue
        }
    }

    if (pixbuf != NULL) {
        clutter_image_set_data(CLUTTER_IMAGE(colors),
                            gdk_pixbuf_get_pixels (pixbuf),
                            COGL_PIXEL_FORMAT_RGB_888,
                            gdk_pixbuf_get_width (pixbuf),
                            gdk_pixbuf_get_height (pixbuf),
                            gdk_pixbuf_get_rowstride (pixbuf),
                            &error);
    }


    #define THUMBNAIL_SIZE 30
    clutter_actor_set_x_expand(lightDisplay, TRUE);
    clutter_actor_set_y_expand(lightDisplay, TRUE);
    clutter_actor_set_position(lightDisplay, 0, 50); 
    clutter_actor_set_size(lightDisplay, WIDTH * osd_scale, HEIGHT * osd_scale);
    clutter_actor_set_rotation_angle(lightDisplay, CLUTTER_Z_AXIS, -90);
    clutter_actor_set_rotation_angle(lightDisplay, CLUTTER_Y_AXIS, 180);
    
    // Wire up the event listener on this lightDisplay actor:
    clutter_actor_set_reactive(lightDisplay, TRUE); // Allow for UI events on this crazy thing!
    // typedef struct  {
    //     ClutterActor *lightDisplay;
    //     TCLControl *tcl;
    //     int *animationNumber;
    // } TouchData;
    // Build a pointer to a struct that we can pass through the g_signal_connect function:
    TouchData *touch_data;
    touch_data = g_slice_new (TouchData); // reserve memory for it...
    touch_data->lightDisplay = lightDisplay; // Place the button actor itself inside the struct
    touch_data->tcl = tcl;
    touch_data->animationNumber = &currentAnimation;

    g_signal_connect(lightDisplay, "touch-event", G_CALLBACK (handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "button-press-event", G_CALLBACK (handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "motion-event", G_CALLBACK (handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "button-release-event", G_CALLBACK (handleTouchEvents), touch_data);

    clutter_actor_set_content(lightDisplay, colors);

    clutter_actor_add_child(stage, lightDisplay);


    // Get ready to hand this display chunk in to the animation event:
    AnimationData *data;
    data = g_slice_new (AnimationData); // reserve memory for it...
    data->rotatingActor = rect;
    data->infoDisplay = infoDisplay;
    data->tcl = tcl;
    data->animationNumber = &currentAnimation;

    timeline = clutter_timeline_new(120);
    g_signal_connect(timeline, "new-frame", G_CALLBACK(handleNewFrame), data);
    clutter_timeline_set_repeat_count(timeline, -1);
    clutter_timeline_start(timeline);

}
Animation::~Animation(){
    g_object_unref(colors);
    g_object_unref(pixbuf);
}

Animation::Animation() {
}

void Animation::switchAnimation(int animationNumber, ClutterActor *infoDisplay) {
    printf("Changing to animation: %i\n", animationNumber);
    currentAnimation = animationNumber;
    clutter_text_set_text(CLUTTER_TEXT(infoDisplay), g_strdup_printf("Current sensor input: %i",currentAnimation));
}

int Animation::getCurrentAnimation() {
    printf("Called! Current animation is: %i\n", currentAnimation);
    return currentAnimation;
}


