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

/*
 Make sure you've loaded the correct FTDI driver:
 retina machine:
   cd /Users/jhagerman/dev/processing/other peopels stuff/p9813/processing
 original 13"
   cd /Volumes/Keket/Users/jamis/dev/Circuits_MPUs/FTDI\ Hacks/TCL\ Lights/p9813/processing
 for arduino: make load
 for bitbanging: make unload
 
 My wiring rig pinout:
   g = blue   = ground
   c = yellow = clock
   + = red/nc = positive 5 volts
   d = green  = data
  nc = not connected
  nc = not connected
  
  orange box:
  1 ground  brown
  2 clock   orange
  3 5volts  red
  4 data    black
*/

// The TotalControl processing library doesn't define the FTDI pins so we do:
// short TC_FTDI_TX  = 0x01;  /* Avail on all FTDI adapters,  strand 0 default */
// short TC_FTDI_RX  = 0x02;  /* Avail on all FTDI adapters,  strand 1 default */
// short TC_FTDI_RTS = 0x04;  /* Avail on FTDI-branded cable, strand 2 default */
// short TC_FTDI_CTS = 0x08;  /* Avail on all FTDI adapters,  clock default    */
// short TC_FTDI_DTR = 0x10;  /* Avail on third-party cables, strand 2 default */
// short TC_FTDI_DSR = 0x20;  /* Avail on full breakout board */
// short TC_FTDI_DCD = 0x40;  /* Avail on full breakout board */
// short TC_FTDI_RI  = 0x80;  /* Avail on full breakout board */

#define COGL_ENABLE_EXPERIMENTAL_2_0_API

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "p9813.h"

#include "cat.h"
#include "TCLControl.h"
#include "events.h"

ClutterActor *rect;
ClutterState *transitions;
gdouble rotation = 0;

const int width = 800;
const int height = 480;
const int mid_x = width/2;
const int mid_y = height/2;

int i;

int cutoff = 0;
double x = 0.0;
double s1,s2,s3;
unsigned char r,g,b;
time_t        t,prev = 0;

TCLControl tcl;
Events eventHandlers;



// Button actors:
const int buttonHeight = height/5;
const int buttonWidth = width/4;
ClutterActor *button1, *button2, *button3, *button4;


// using namespace std;

void on_timeline_new_frame(ClutterTimeline *timeline, gint frame_num, gpointer data) {
    rotation += 0.3;

    clutter_actor_set_rotation_angle(rect, CLUTTER_Z_AXIS, rotation * 5);


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


    cutoff += 1;

    if (cutoff > 50) {
        cutoff = 0;
    }

    for(i=0; i<tcl.totalPixels; i++) {
        if ( i > cutoff) {
            tcl.pixelBuf[i] = TCrgb(0,255,0);
        } else {
            tcl.pixelBuf[i] = TCrgb(255,255,255);
        }
        
    }


    // Send the updated buffer to the strands
    if (tcl.enabled) {
        tcl.Update();
    }

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

    // init Clutter:
    int ret;
    ret = clutter_init(&argc, &argv);

    // // Check clutter input devices:
    // ClutterDeviceManager * deviceManager = clutter_device_manager_get_default();
    // ClutterInputDevice *device;
    // device = clutter_device_manager_get_core_device (deviceManager, CLUTTER_TOUCHPAD_DEVICE);

    // if (device == NULL) {
    //     printf("Oops. No touchpad found!\n");
    //     device = clutter_device_manager_get_core_device (deviceManager, CLUTTER_TOUCHSCREEN_DEVICE);
    // }
    // if (device == NULL) {
    //     printf("Oops. No touchscreen found!\n");
    //     device = clutter_device_manager_get_core_device (deviceManager, CLUTTER_POINTER_DEVICE);
    // }
    // g_printf("Device name: %s\n", clutter_input_device_get_device_name(device));
    // g_printf(" Enabled: %s\n", clutter_input_device_get_enabled(device) ? "true":"false");

    // GSList * deviceList = clutter_device_manager_list_devices(deviceManager);
    // int deviceCount = g_slist_length(deviceList);
    // printf("Input device count: %i\n", deviceCount);


    // // printf("deviceList 0: ", deviceList[])
    // for (i = 0; i < deviceCount; i+=1) {
    //     _ClutterInputDevice = g_slist_nth_data(deviceList, 0);
    // //     gboolean enabled = clutter_input_device_get_enabled(deviceList);
    // //     printf("Input device: %i\n", enabled);

    // }


    // How far off was I???
    // translate_native_event_to_clutter (native_event, &c_event);
    // ClutterDeviceManager *manager;
    // ClutterInputDevice *device;
    // manager = clutter_device_manager_get_default ();
    // device = clutter_device_manager_get_core_device (manager, CLUTTER_POINTER_DEVICE);



    // exit(1);


    // Build some colors:
    ClutterColor stage_color = { 0xFF, 0xFF, 0xFF, 0xFF };
    ClutterColor actor_color = { 0x00, 0xCC, 0x00, 0xFF };
    ClutterColor red_color = { 0xFF, 0xCC, 0x00, 0xFF };

    // Set up the stage:
    ClutterActor *stage = clutter_stage_new();
    clutter_actor_set_size(stage, width, height);
    clutter_actor_set_background_color(stage, &stage_color);

    // Set up a listener to close the app if the window is closed:
    g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);
    // g_signal_connect (stage, "motion-event", G_CALLBACK (_pointer_motion_cb), transitions);
    g_signal_connect(stage, "key-press-event", G_CALLBACK(eventHandlers.handleKeyPresses), NULL);

    
    /* Add a rectangle to the stage: */
    rect = clutter_actor_new();
    clutter_actor_set_background_color (rect, &actor_color);
    clutter_actor_set_size (rect, 100, 50);
    clutter_actor_set_position (rect, 20, 20);

    // Wire up some event listeners:
    clutter_actor_set_reactive (rect, TRUE);
    g_signal_connect (rect, "touch-event", G_CALLBACK (eventHandlers.handleTouchEvents), transitions);
    // g_signal_connect (rect, "motion-event", G_CALLBACK (_pointer_motion_cb), transitions);
    g_signal_connect (rect, "button-press-event", G_CALLBACK (eventHandlers.handleMouseEvents), transitions);
    
    clutter_actor_add_child(stage, rect);

    // Create a bunch of yellow boxes on the screen:
    for (int i = 0; i < 50; i+=1) {
        createBox(stage, 10+(i*10), 10+(i*10), 10,10, red_color);
    }


    // Build Buttons:
    // const int buttonHeight = height/5;
    // const int buttonWidth = width/4;
    // ClutterActor *button1, *button2, *button3, *button4;

    button1 = clutter_actor_new();
    clutter_actor_set_background_color (button1, &actor_color);
    clutter_actor_set_size (button1, buttonWidth, buttonHeight);
    clutter_actor_set_position (button1, height-(clutter_actor_get_height(button1)), 0);


    
    // Add a label to the stage:
    ClutterActor *label = clutter_text_new_with_text ("Sans 32px", "Sensatron 2014");
    clutter_actor_set_position(label, mid_x-(clutter_actor_get_width(label)/2), 0); 
    clutter_actor_add_child(stage, label);

    ClutterTimeline *timeline = clutter_timeline_new(120);
    g_signal_connect(timeline, "new-frame", G_CALLBACK(on_timeline_new_frame), NULL);
    clutter_timeline_set_repeat_count(timeline, -1);
    clutter_timeline_start(timeline);

    clutter_actor_show(stage);

    clutter_main();

    return EXIT_SUCCESS;
}

