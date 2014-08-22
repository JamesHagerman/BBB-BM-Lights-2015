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
    TCLControl *tcl;
} AnimationData;

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

int osd_scale = 15;

int AnimationID = 0;

int getrand(int min,int max){
     return (rand()%(max-min)+min);
}  
int randColor() {
    return getrand(0,255);
}
uint getRandomColor(){
    return TCrgb(randColor(), randColor(), randColor());
}


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


int h_angle = 0;
int popRainbow(int h_rate) {
    h_angle += h_rate;

    int toRet = getRGB(h_angle, 255, 255);

    if (h_angle >= 255) {
        h_angle = 0;
    }
    return toRet;
}




int totalPixels = TCLControl::nStrands * TCLControl::pixelsPerStrand;
int memSize = totalPixels * sizeof(TCpixel);
TCpixel *pixelBackupBuf = (TCpixel *)malloc(memSize);

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
                tcl->pixelBuf[index] = getRandomColor();
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
    if (cutoff > 10) {
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


void animation1(TCLControl *tcl) {
    int temp = getRandomColor();

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

void animation2(TCLControl *tcl) {
    

    cutoff += 1;
    if (cutoff > 10) {
        for(int i=0; i < tcl->totalPixels; i++) {
            // if ( i > cutoff) {
            //     tcl->pixelBuf[i] = TCrgb(0,255,0);
            // } else {
                tcl->pixelBuf[i] = getRandomColor();
            // }
        }
        cutoff = 0;
    }
}







void handleNewFrame (ClutterActor *timeline, gint frame_num, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    AnimationData *data;
    data = (AnimationData *)user_data;

    ClutterActor *rotatingActor = CLUTTER_ACTOR (data->rotatingActor);
    TCLControl *tcl = data->tcl;


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

    animation1(tcl);

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

Animation::Animation(ClutterActor *stage, ClutterActor *rotatingActor, TCLControl *tcl){ //TCLControl tcl
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
    clutter_actor_set_reactive(lightDisplay, TRUE); // Allow for UI events on this crazy thing!

    clutter_actor_set_content(lightDisplay, colors);

    clutter_actor_add_child(stage, lightDisplay);


    // Get ready to hand this display chunk in to the animation event:
    AnimationData *data;
    data = g_slice_new (AnimationData); // reserve memory for it...
    data->rotatingActor = rect;
    data->tcl = tcl;

    timeline = clutter_timeline_new(120);
    g_signal_connect(timeline, "new-frame", G_CALLBACK(handleNewFrame), data);
    clutter_timeline_set_repeat_count(timeline, -1);
    clutter_timeline_start(timeline);

}
Animation::~Animation(){
    g_object_unref(colors);
    g_object_unref(pixbuf);
}







