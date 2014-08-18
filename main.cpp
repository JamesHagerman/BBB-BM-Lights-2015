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
short TC_FTDI_TX  = 0x01;  /* Avail on all FTDI adapters,  strand 0 default */
short TC_FTDI_RX  = 0x02;  /* Avail on all FTDI adapters,  strand 1 default */
short TC_FTDI_RTS = 0x04;  /* Avail on FTDI-branded cable, strand 2 default */
short TC_FTDI_CTS = 0x08;  /* Avail on all FTDI adapters,  clock default    */
short TC_FTDI_DTR = 0x10;  /* Avail on third-party cables, strand 2 default */
short TC_FTDI_DSR = 0x20;  /* Avail on full breakout board */
short TC_FTDI_DCD = 0x40;  /* Avail on full breakout board */
short TC_FTDI_RI  = 0x80;  /* Avail on full breakout board */
  

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

double x = 0.0;
double s1,s2,s3;
unsigned char r,g,b;
int i, totalPixels;
int nStrands = 1;
int pixelsPerStrand = 50;
time_t        t,prev = 0;
TCstats stats;
TCpixel *pixelBuf;

// using namespace std;

void on_timeline_new_frame(ClutterTimeline *timeline, gint frame_num, gpointer data) {
    rotation += 0.3;

    clutter_actor_set_rotation_angle(rect, CLUTTER_Z_AXIS, rotation * 5);


    // Update the lights:
    x += (double)pixelsPerStrand / 20000.0;
    s1 = sin(x                 ) *  11.0;
    s2 = sin(x *  0.857 - 0.214) * -13.0;
    s3 = sin(x * -0.923 + 1.428) *  17.0;
    for(i=0;i<totalPixels;i++)
    {
        r   = (int)((sin(s1) + 1.0) * 127.5);
        g   = (int)((sin(s2) + 1.0) * 127.5);
        b   = (int)((sin(s3) + 1.0) * 127.5);
        pixelBuf[i] = TCrgb(r,g,b);
        s1 += 0.273;
        s2 -= 0.231;
        s3 += 0.428;
    }

    if((i = TCrefresh(pixelBuf,NULL,&stats)) != TC_OK)
       TCprintError(static_cast<TCstatusCode>(i));

    /* Update statistics once per second. */
    if((t = time(NULL)) != prev)
    {
        system("clear");
        TCprintStats(&stats);
        prev = t;
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

    /* Allocate pixel array.  One TCpixel per pixel per strand. */
    totalPixels = nStrands * pixelsPerStrand;
    i           = totalPixels * sizeof(TCpixel);
    if(NULL == (pixelBuf = (TCpixel *)malloc(i)))
    {
        printf("Could not allocate space for %d pixels (%d bytes).\n", totalPixels,i);
        return 1;
    }

    /* Initialize library, open FTDI device.  Baud rate errors
       are non-fatal; program displays a warning but continues. */
    if((i = TCopen(nStrands,pixelsPerStrand)) != TC_OK)
    {
        TCprintError(static_cast<TCstatusCode>(i));
        if(i < TC_ERR_DIVISOR) return 1;
    }

    /* Initialize statistics structure before use. */
    TCinitStats(&stats);

    /* The demo animation sets every pixel in every frame.  Your code
       doesn't necessarily have to --  it could just change altered
       pixels and call TCrefresh().  The example is some swirly color
       patterns using a combination of sine waves.  There's no meaning
       to any of this, just applying various constants at each stage
       in order to avoid repetition between the component colors. */
    // for(x=0.0;;x += (double)pixelsPerStrand / 20000.0)
    // {
    //     s1 = sin(x                 ) *  11.0;
    //     s2 = sin(x *  0.857 - 0.214) * -13.0;
    //     s3 = sin(x * -0.923 + 1.428) *  17.0;
    //     for(i=0;i<totalPixels;i++)
    //     {
    //         r   = (int)((sin(s1) + 1.0) * 127.5);
    //         g   = (int)((sin(s2) + 1.0) * 127.5);
    //         b   = (int)((sin(s3) + 1.0) * 127.5);
    //         pixelBuf[i] = TCrgb(r,g,b);
    //         s1 += 0.273;
    //         s2 -= 0.231;
    //         s3 += 0.428;
    //     }

    //     if((i = TCrefresh(pixelBuf,NULL,&stats)) != TC_OK)
    //        TCprintError(static_cast<TCstatusCode>(i));

    //     /* Update statistics once per second. */
    //     if((t = time(NULL)) != prev)
    //     {
    //         system("clear");
    //         TCprintStats(&stats);
    //         prev = t;
    //     }
    // }

    



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

    TCclose();
    free(pixelBuf);

    return EXIT_SUCCESS;
}

