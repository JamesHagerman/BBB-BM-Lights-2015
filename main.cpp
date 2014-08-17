// Sensatron work:
// We need SPI pins on the BBB. SPI0 pins are in use via the LCD. SPI1 pins are not:
// 28   spi1_cs0
// 29   spi1_d0
// 30   spi1_d1 (IN USE BY LCD!!)
// 31   spi1_sclk
//
// This means we should be able to use the SPI1 lines to at least TEST SPI data output.
//
// Another option is to just dump all of the color data out via DMA to a Teensy...
// And yet another option is to dump color data out to the FTDI bitbang driver like we did last year.
//
// Honestly, the FTDI mode is probably the best solution. So we should try that first.


#define COGL_ENABLE_EXPERIMENTAL_2_0_API

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <clutter/clutter.h>
#include "p9813.h"

ClutterActor *rect;
gdouble rotation = 0;

const int width = 800;
const int height = 480;
const int mid_x = width/2;
const int mid_y = height/2;

using namespace std;

void on_timeline_new_frame(ClutterTimeline *timeline, gint frame_num, gpointer data) {
    rotation += 0.3;

    clutter_actor_set_rotation_angle(rect, CLUTTER_Z_AXIS, rotation * 5);
}

ClutterActor* createBox(ClutterActor *stage, int x, int y, int w, int h, ClutterColor color) {
    ClutterActor *toRet = clutter_actor_new();
    clutter_actor_set_background_color( toRet, &color);
    clutter_actor_set_size (toRet, w, h);
    clutter_actor_set_position (toRet, x, y);
    clutter_actor_add_child (stage, toRet);
    clutter_actor_show (toRet);
    return toRet;
}

int main(int argc, char *argv[]) {
    int ret;
    ret = clutter_init(&argc, &argv);

    ClutterColor stage_color = { 0xFF, 0xFF, 0xFF, 0xFF };
    ClutterColor actor_color = { 0x00, 0xCC, 0x00, 0xFF };
    ClutterColor red_color = { 0xFF, 0xCC, 0x00, 0xFF };

    ClutterActor *stage = clutter_stage_new();
    clutter_actor_set_size(stage, width, height);
    clutter_actor_set_background_color(stage, &stage_color);

    
    /* Add a rectangle to the stage: */
    rect = clutter_actor_new();
    clutter_actor_set_background_color (rect, &actor_color);
    clutter_actor_set_size (rect, 100, 50);
    clutter_actor_set_position (rect, 20, 20);
    clutter_actor_add_child (stage, rect);
    clutter_actor_show (rect);

    for (int i = 0; i < 50; i+=1) {
        createBox(stage, 10+(i*10), 10+(i*10), 10,10, red_color);
    }
    

    // Add a label to the stage:
    ClutterActor *label = clutter_text_new_with_text ("Sans 32px", "Hello, world");
    clutter_actor_set_position(label, mid_x, mid_y); 
    clutter_actor_add_child(stage, label);


    ClutterTimeline *timeline = clutter_timeline_new(60);
    g_signal_connect(timeline, "new-frame", G_CALLBACK(on_timeline_new_frame), NULL);
    clutter_timeline_set_repeat_count(timeline, -1);
    clutter_timeline_start(timeline);

    clutter_actor_show(stage);

    clutter_main();

    return EXIT_SUCCESS;
}

