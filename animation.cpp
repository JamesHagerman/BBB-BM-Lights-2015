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


typedef struct  {
    ClutterActor *rotatingActor;
    TCLControl *tcl;
} AnimationData;

gdouble rotation = 0;
int cutoff = 0;

// Size of onscreen display:
// Define the sizes of the image that shows the lights:
#define WIDTH 49
#define HEIGHT 12

void handleNewFrame (ClutterActor *timeline, gint frame_num, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    AnimationData *data;
    data = (AnimationData *)user_data;

    ClutterActor *rotatingActor = CLUTTER_ACTOR (data->rotatingActor);
    TCLControl *tcl = data->tcl;


    rotation += 0.3;
    // printf("Update... %f\n", rotation);

    clutter_actor_set_rotation_angle(rotatingActor, CLUTTER_Z_AXIS, rotation * 5);


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

    // Draw onscreen color map display:
    // Try changing the color right after setting it to red:
    // for(int x = 0; x < WIDTH; x++) {
    //     for(int y = 0; y < HEIGHT; y++) {

    //         // This next line grabs the address of single pixel out of the pixels char buffer
    //         // and points a char at it so that it's value can be set:
    //         unsigned char* pixel =  &pixels[y * rowstride + x * 3];

    //         pixel[0] = 0x0;//red
    //         pixel[1] = 255;//green
    //         pixel[2] = 0x0;//blue
    //     }
    // }
    // if (pixbuf != NULL) {
    //     clutter_image_set_data(CLUTTER_IMAGE(colors),
    //                         gdk_pixbuf_get_pixels (pixbuf),
    //                         COGL_PIXEL_FORMAT_RGB_888,
    //                         gdk_pixbuf_get_width (pixbuf),
    //                         gdk_pixbuf_get_height (pixbuf),
    //                         gdk_pixbuf_get_rowstride (pixbuf),
    //                         &error);
    // }

    // clutter_actor_set_content(lightDisplay, colors);



    cutoff += 1;

    if (cutoff > 50) {
        cutoff = 0;
    }

    for(int i=0; i<tcl->totalPixels; i++) {
        if ( i > cutoff) {
            tcl->pixelBuf[i] = TCrgb(0,255,0);
        } else {
            tcl->pixelBuf[i] = TCrgb(255,255,255);
        }
        
    }


    // Send the updated buffer to the strands
    if (tcl->enabled) {
        tcl->Update();
    }
}

Animation::Animation(ClutterActor *stage, ClutterActor *rotatingActor, TCLControl *tcl){ //TCLControl tcl
    printf("Building animation tools...\n");

    rect = rotatingActor;


    // Add a colored texture to the app:
    //
    // First, we need some Actor to actually display the light colors:
    ClutterContent *colors = clutter_image_new();
    ClutterActor *lightDisplay = clutter_actor_new();

    // Second we need an error object to store errors:
    GError *error = NULL;

    // Load image data from some other data source...:
    // guchar *data =
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(data);

    // Load image data from a file:
    // const char *img_path = "./wut.png";
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size (img_path, WIDTH, HEIGHT, &error);
    

    // Load image data from nothing. Build it manually.
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, WIDTH, HEIGHT);
    unsigned char* pixels = gdk_pixbuf_get_pixels(pixbuf);

    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

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
    clutter_actor_set_position(lightDisplay, mid_x, mid_y); 
    clutter_actor_set_size(lightDisplay, WIDTH*4, HEIGHT*4);
    // clutter_actor_set_position(lightDisplay, col * THUMBNAIL_SIZE, row * THUMBNAIL_SIZE);
    // clutter_actor_set_reactive(lightDisplay, TRUE);

    clutter_actor_set_content(lightDisplay, colors);
    // g_object_unref(colors);
    // g_object_unref(pixbuf);

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

}







