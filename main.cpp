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
 Beaglebone Black:
   Do Nothing.

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
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "p9813.h"

#include "TCLControl.h"
#include "events.h"
#include "button.h"
#include "animation.h"

#include "configurations.h"

ClutterActor *rect;
ClutterState *transitions;

ClutterActor *label;
ClutterActor *label2;
ClutterActor *infoDisplay;

TCLControl tcl;

// ColorMan colorMan;

Events eventHandlers;
typedef struct  {
    ClutterActor *statusLabel;
} EventData;


// using namespace std;

// ClutterActor* createBox(ClutterActor *stage, int x, int y, int w, int h, ClutterColor color) {
//     ClutterActor *toRet = clutter_actor_new();
//     clutter_actor_set_background_color( toRet, &color);
//     clutter_actor_set_size (toRet, w, h);
//     clutter_actor_set_position (toRet, x, y);
//     clutter_actor_add_child (stage, toRet);
//     clutter_actor_show (toRet);
//     return toRet;
// }



int main(int argc, char *argv[]) {

    // init Clutter:
    if (clutter_init(&argc, &argv) != CLUTTER_INIT_SUCCESS) {
        printf("Clutter failed to initalize. Exiting...\n");
        exit(1);
    }

    // Build some colors:
    ClutterColor stage_color = { 0, 0, 0, 0xFF };
    ClutterColor text_color = {255, 255, 255, 255};
    ClutterColor actor_color = { 102, 0, 204, 255 };
    // ClutterColor red_color = { 0xFF, 0, 0, 255 };

    // Set up the stage:
    ClutterActor *stage = clutter_stage_new();
    clutter_actor_set_size(stage, width, height);
    clutter_actor_set_background_color(stage, &stage_color);

    // Set up a listener to close the app if the window is closed:
    g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);


    // Add a label to the stage:
    label = clutter_text_new_with_text ("Sans 16px", "System Live! To halt system, press enter button below screen, wait for orange LED to stop blinking before removing system power.");
    clutter_text_set_line_wrap(CLUTTER_TEXT(label), true);
    clutter_actor_set_size(label, width-35, 35);
    clutter_text_set_color(CLUTTER_TEXT(label), &text_color);
    clutter_actor_set_position(label, 35, 6); 
    clutter_actor_add_child(stage, label);

    // label2 = clutter_text_new_with_text ("Sans 14px", "To halt system, press enter button below screen and wait for the orange LED to stop blinking before removing power.");
    // clutter_text_set_line_wrap(CLUTTER_TEXT(label2), true);
    // clutter_actor_set_size(label2, width, 40);
    // clutter_text_set_color(CLUTTER_TEXT(label2), &text_color);
    // clutter_actor_set_position(label2, 0, height-buttonHeight-clutter_actor_get_height(label2)*2); 
    // clutter_actor_add_child(stage, label2);

    // Set up the keyboard listener for the arrow, enter, and esc keys:
    EventData *data;
    data = g_slice_new (EventData); // reserve memory for it...
    data->statusLabel = label; // Place the button actor itself inside the struct
    // Build a "down" color (hard coded for now...)
    // ClutterColor downColor = { 255, 0, 47, 0xFF };
    g_signal_connect(stage, "key-press-event", G_CALLBACK(eventHandlers.handleKeyPresses), data);
    

    /* Status rectangle */
    rect = clutter_actor_new();
    clutter_actor_set_background_color (rect, &actor_color);
    clutter_actor_set_size (rect, 20, 20);
    clutter_actor_set_pivot_point(rect, 0.5, 0.5);
    clutter_actor_set_position (rect, 10, 10);
    // Wire up some event listeners:
    clutter_actor_set_reactive (rect, TRUE);
    g_signal_connect (rect, "touch-event", G_CALLBACK (eventHandlers.handleTouchEvents), transitions);
    //g_signal_connect (rect, "motion-event", G_CALLBACK (_pointer_motion_cb), transitions);
    g_signal_connect (rect, "button-press-event", G_CALLBACK (eventHandlers.handleMouseEvents), rect);

    // Add the spinning rectangle to the stage:
    clutter_actor_add_child(stage, rect);

    // Create a bunch of yellow boxes on the screen:
    // for (int i = 0; i < 50; i+=1) {
    //     createBox(stage, 10+(i*1), 10, 1,1, red_color);
    // }

    infoDisplay = clutter_text_new_with_text ("Sans 14px", "Current sensor input: 6");
    clutter_text_set_line_wrap(CLUTTER_TEXT(infoDisplay), true);
    clutter_actor_set_size(infoDisplay, width, 40);
    clutter_text_set_color(CLUTTER_TEXT(infoDisplay), &text_color);
    clutter_actor_set_position(infoDisplay, 0, height-(buttonHeight*2)-clutter_actor_get_height(infoDisplay)); 
    clutter_actor_add_child(stage, infoDisplay);

    // Start animation loop:
    Animation animation = Animation(stage, rect, &tcl, infoDisplay);

    // Build UI Buttons:
    Button button1 = Button(stage, 0, buttonWidth, buttonHeight, 0, height-buttonHeight, (ClutterColor){ 0, 255, 47, 0xFF }, &animation, infoDisplay);
    Button button2 = Button(stage, 1, buttonWidth, buttonHeight, buttonWidth, height-buttonHeight, (ClutterColor){ 232, 217, 12, 0xFF }, &animation, infoDisplay);
    Button button3 = Button(stage, 2, buttonWidth, buttonHeight, buttonWidth*2, height-buttonHeight, (ClutterColor){ 255, 122, 0, 0xFF }, &animation, infoDisplay);
    Button button4 = Button(stage, 3, buttonWidth, buttonHeight, buttonWidth*3, height-buttonHeight, (ClutterColor){ 232, 12, 111, 0xFF }, &animation, infoDisplay);
    Button button5 = Button(stage, 4, buttonWidth, buttonHeight, buttonWidth*4, height-buttonHeight, (ClutterColor){ 51, 13, 255, 0xFF }, &animation, infoDisplay);
    
    Button button6 = Button(stage, 5, buttonWidth, buttonHeight, 0, height-(buttonHeight*2), (ClutterColor){ 255, 0, 91, 0xFF }, &animation, infoDisplay);
    Button button7 = Button(stage, 6, buttonWidth, buttonHeight, buttonWidth, height-(buttonHeight*2), (ClutterColor){ 55, 8, 232, 0xFF }, &animation, infoDisplay);
    Button button8 = Button(stage, 7, buttonWidth, buttonHeight, buttonWidth*2, height-(buttonHeight*2), (ClutterColor){ 0, 228, 255, 0xFF }, &animation, infoDisplay);
    Button button9 = Button(stage, 8, buttonWidth, buttonHeight, buttonWidth*3, height-(buttonHeight*2), (ClutterColor){ 30, 232, 12, 0xFF }, &animation, infoDisplay);
    Button button10 = Button(stage, 9, buttonWidth, buttonHeight, buttonWidth*4, height-(buttonHeight*2), (ClutterColor){ 255, 215, 0, 0xFF }, &animation, infoDisplay);
    

    // Actually show the stage and run the app:
    clutter_actor_show(stage);
    clutter_main();

    return EXIT_SUCCESS;
}

