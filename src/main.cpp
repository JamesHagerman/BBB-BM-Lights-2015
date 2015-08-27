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
  1 ground  brown   black
  2 clock   orange  white
  3 5volts  red     red
  4 data    black   green


Looking TOWARDS the holes on the female connector:

Official connector colors:
     |
blue yellow
green red

My wire colors:
     |
brown orange
black red


Official wire colors:
     |
black white
green red




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

#include "alsa.h"
#include "fft.h"

TCLControl tcl;
Events eventHandlers;

int main(int argc, char *argv[]) {

    if (clutter_check_version(1,2,0)) {
        printf("Right version of clutter is installed!\n\n");
    }

    if (clutter_init(&argc, &argv) != CLUTTER_INIT_SUCCESS) {
        printf("Clutter failed to initalize. Exiting...\n");
        exit(1);
    }

    // Build some colors:
    ClutterColor stage_color = { 0, 0, 0, 0xFF };

    // Set up the stage:
    ClutterActor *stage = clutter_stage_new();
    clutter_actor_set_size(stage, width, height);
    clutter_actor_set_background_color(stage, &stage_color);

    // Set up a listener to close the app if the window is closed:
    g_signal_connect (stage, "destroy", G_CALLBACK (clutter_main_quit), NULL);

    // Set up the keyboard listener for the arrow, enter, and esc keys:
    g_signal_connect(stage, "key-press-event", G_CALLBACK(eventHandlers.handleKeyPresses), NULL);



    // Try getting alsa loaded!!
    initAlsa();

    // Start animation loop:
    Animation animation = Animation(stage, &tcl); // pointer TO the main tcl object.

    // Load some images for the buttons:
//    const char *img_path = "./wut.png";
//    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size (img_path, WIDTH, HEIGHT, &error);




    // Build UI Buttons:
    // Bottom row
    Button speedCtrl = Button(stage, 0, buttonWidth*2, buttonHeight, 0, height-buttonHeight, (ClutterColor){ 159,238,0, 0xFF }, &animation, 1); // Speed control
    Button reloadCtrl = Button(stage, 1, buttonWidth, buttonHeight, buttonWidth*2, height-buttonHeight, (ClutterColor){ 255, 0, 91, 0xFF }, &animation, 5);
    Button colorCtrl = Button(stage, 1, buttonWidth, buttonHeight, buttonWidth*3, height-buttonHeight, (ClutterColor){ 255,255,0, 0xFF }, &animation, 2); // Color Control

    // Top row:
    Button decrCtrl = Button(stage, 2, buttonWidth/2, buttonHeight, 0, height-buttonHeight*2, (ClutterColor){ 113,9,170, 0xFF }, &animation, 3);
    Button incrCtrl = Button(stage, 3, buttonWidth/2, buttonHeight, buttonWidth*4+buttonWidth/2, height-buttonHeight*2, (ClutterColor){ 205,0,116, 0xFF }, &animation, 4);

    Button button7 = Button(stage, 4, buttonWidth, buttonHeight, buttonWidth-buttonWidth/2, height-buttonHeight*2, (ClutterColor){ 255,0,0, 0xFF }, &animation, 0);
    Button button8 = Button(stage, 5, buttonWidth, buttonHeight, buttonWidth*2-buttonWidth/2, height-buttonHeight*2, (ClutterColor){ 255,116,0, 0xFF }, &animation, 0);
    Button button9 = Button(stage, 6, buttonWidth, buttonHeight, buttonWidth*3-buttonWidth/2, height-buttonHeight*2, (ClutterColor){ 0,153,153, 0xFF }, &animation, 0);
    Button button5 = Button(stage, 7, buttonWidth, buttonHeight, buttonWidth*4-buttonWidth/2, height-buttonHeight*2, (ClutterColor){ 0,204,0, 0xFF }, &animation, 0);



//    Button button3 = Button(stage, 2, buttonWidth, buttonHeight, buttonWidth*2, height-buttonHeight, (ClutterColor){ 255, 122, 0, 0xFF }, &animation, 0);
//    Button button4 = Button(stage, 3, buttonWidth, buttonHeight, buttonWidth*3, height-buttonHeight, (ClutterColor){ 232, 12, 111, 0xFF }, &animation, 0);




    // Actually show the stage and run the app:
    clutter_actor_show(stage);
    clutter_main();

    return EXIT_SUCCESS;
}

