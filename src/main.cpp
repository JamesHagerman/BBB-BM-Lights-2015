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

#include <alsa/asoundlib.h>

// Order of these two matters. Complex needs to be first:
#include <complex.h>
#include <fftw3.h>

#include "p9813.h"

#include "TCLControl.h"
#include "events.h"
#include "button.h"
#include "animation.h"
#include "configurations.h"

#include "alsa.h"

int i; // for loops. deal with it.
#define PI 3.14159265 // ya know... for our waveform generator :facepalm:

TCLControl tcl;
Events eventHandlers;

// Stuff pulled from cava
#define GCC_UNUSED /* nothing */
pthread_t  p_thread;
int        thr_id GCC_UNUSED;
struct audio_data audio;

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
    audio.format = -1;
    audio.rate = 0;
    audio.source = new char[10];

    // hw:1,0 = BBB usb snd card
    // hw:0,1 = Ubuntu mic input...
    strncpy( audio.source, "hw:1,0", 9 ); // BBB
//    strncpy( audio.source, "hw:0,1", 9 ); // UBUNTU
    audio.im = 1;
    struct timespec req = { .tv_sec = 0, .tv_nsec = 0 };

    // Actually kick off the pthread that will grab audio:
    //thr_id = pthread_create(&p_thread, NULL, input_alsa, (void *)&audio); //starting alsamusic listener

    // Check to make sure the audio is actually working...
    // This is kind of a drag.... it just HANGS...
//    int n = 0;
//    while (audio.format == -1 || audio.rate == 0) {
//        req.tv_sec = 0;
//        req.tv_nsec = 1000000;
//        nanosleep(&req, NULL);
//        n++;
//        if (n > 2000) {
//#ifdef DEBUG
//            fprintf(stderr, "could not get rate and/or format, problems with audio thread? quiting...\n");
//#endif
//            exit(EXIT_FAILURE);
//        }
//    }
//#ifdef DEBUG
//    printf("got format: %d, rate: %d, channels: %d, buffer size: %d\n", audio.format, audio.rate, audio.channelCount, audio.actualBufferSize);
//#endif

    // Just a test thread to see if they even work on ARM:
//    int dumb = 12;
//    thr_id = pthread_create(&p_thread, NULL, derpthread, (void *)&dumb); // STUPID THREAD.

    // Setup FFTW:


//    // From cava:
//    int M = 2048; // Basically, this is a shit variable name for "sample count". (blame karl@stavestrand.no)
//    double in[2 * (M / 2 + 1)];
//    fftw_complex out[M / 2 + 1][2];
//    fftw_plan p;
//    long int lpeak, hpeak;
//    int sleep = 0;
//    float peak[201];
//    int bands = 25;
//    int f[200];
//    int y[M / 2 + 1];
//    float temp;
//    float k[200];
//    int o, sens = 50, ignore = 0;
//
//    // freq band stuff:
//    float fc[200];
//    float fr[200];
//    unsigned int lowcf = 20, highcf = 10000;
//    double freqconst = log10((float)lowcf / (float)highcf) /  ((float)1 / ((float)bands + (float)1) - 1);
//    int lcf[200], hcf[200];
//
//    // build plan for fftw3:
//    printf("Building plan...\n");
//    p = fftw_plan_dft_r2c_1d(M, in, *out, FFTW_MEASURE);
//    printf("Plan completed!\n");

    // Fuck cava with it's shit program flow and variable names
    // Things needed for fftw3:
    int actualBufferSize =  audio.actualBufferSize;
    int samples_count = 2 * (actualBufferSize / 2 + 1);
    double *samples = fftw_alloc_real(samples_count);
    fftw_complex *output = fftw_alloc_complex(samples_count);
    fftw_plan plan;

    // Things needed for alsa (because I'm too lazy to build the full arch right now:
//    int channels = 2;
//    int ch; //for loop

    // A little cleanup help:
    double max = 0;

    // Fill buffers with zeros. Because stupid shit happens otherwise:
    for (i = 0; i < samples_count; i++) {
        samples[i] = 0;
        output[i] = 0;
    }

    // Build the plan:
    plan = fftw_plan_dft_r2c_1d(samples_count, samples, output, 0);

    int count = 0;
    while(count<1) {
        // naive input read:
//        for (i = 0; i< samples_count; i++) {
//            samples[i] = audio.audio_out[i];
//        }



        for (i = 0; i < samples_count; i++) {
            printf("%i, ", audio.audio_out[i]);
            samples[i] = audio.audio_out[i];
//            printf("%f, ", sin((i*100)*PI/180));
//            samples[i] = sin((i*10)*PI/180);
        }
        printf("Audio read END\n");

        // Averaging. The audio data we get in is really noisy.
//        for (i = 0; i < samples_count; i++) {
//            samples[i] = 0;
//            for (ch = 0; ch < channels; ch++) {
//                samples[i] += audio.audio_out[i * channels + ch];
//            }
//            samples[i] /= channels;
//        }

        // compute fftw
        fftw_execute(plan);

        // The next for loop pulls the absolute value of the complex fft output and plops it back into
        // the INPUT ARRAY! (named samples here). This allows us to reuse samples for "maximizing" and
        // maybe even smoothing...
        for (i = 0; i < samples_count / 2; i++) {
            samples[i] = cabs(output[i]);
            if (samples[i] > max) {
                max = samples[i];
            }
        }

        // output:
        for (i = 0; i < samples_count / 2; i++) {
            printf("%.1f, ", samples[i]);
        }
        printf("FFT compute end\n");
        max = 0; // reset for next pass
        count++;
    }


    // figure out bands:
//    for (n = 0; n < bands + 1; n++) {
//        fc[n] = highcf * pow(10, freqconst * (-1) + ((((float)n + 1) / ((float)bands + 1)) * freqconst)); //decided to cut it at 10k, little interesting to hear above
//        fr[n] = fc[n] / (audio.rate /2); //remember nyquist!, pr my calculations this should be rate/2 and  nyquist freq in M/2 but testing shows it is not... or maybe the nq freq is in M/4
//        lcf[n] = fr[n] * (M /4); //lfc stores the lower cut frequency foo each band in the fft out buffer
//        if (n != 0) {
//            hcf[n - 1] = lcf[n] - 1;
//            if (lcf[n] <= lcf[n - 1])lcf[n] = lcf[n - 1] + 1; //pushing the spectrum up if the expe function gets "clumped"
//            hcf[n - 1] = lcf[n] - 1;
//        }
//
//        if (n != 0) {
//            printf("%i: %f -> %f (%d -> %d) \n", n, fc[n - 1], fc[n], lcf[n - 1], hcf[n - 1]);
//        }
//    }
//
//    // HACK an FFT together:
//    while (1) {
//
//        // process: populate input buffer and check if input is present
//        lpeak = 0;
//        hpeak = 0;
//        for (i = 0; i < (2 * (M / 2 + 1)); i++) {
//            if (i < M) {
//                in[i] = audio.audio_out[i];
//                if (audio.audio_out[i] > hpeak) hpeak = audio.audio_out[i];
//                if (audio.audio_out[i] < lpeak) lpeak = audio.audio_out[i];
//            } else in[i] = 0;
//        }
//        peak[bands] = (hpeak + abs(lpeak));
//        if (peak[bands] == 0)sleep++;
//        else sleep = 0;
//
//        if (sleep < 400) {
//            fftw_execute(p);
//
//            // process: separate frequency bands
//            for (o = 0; o < bands; o++) {
//                //flastd[o] = f[o]; //saving last value for drawing
//                peak[o] = 0;
//
//                // process: get peaks
//                for (i = lcf[o]; i <= hcf[o]; i++) {
//                    y[i] =  pow(pow(*out[i][0], 2) + pow(*out[i][1], 2), 0.5); //getting r of compex
//                    peak[o] += y[i]; //adding upp band
//                }
//                peak[o] = peak[o] / (hcf[o]-lcf[o]+1); //getting average
//                temp = peak[o] * k[o] * ((float)sens / 100); //multiplying with k and adjusting to sens settings
//                if (temp > height * 8)temp = height * 8; //just in case
//                if (temp <= ignore)temp = 0;
//                f[o] = temp;
//            }
//            for (o = 0; o < bands; o++) {
//                printf("%.1f, ", peak[o]);
//            }
//            printf("END OF LINE\n");
//        }
//
//    }

    // Start animation loop:
    Animation animation = Animation(stage, &tcl); // pointer TO the main tcl object.

    // Build UI Buttons:
    Button button1 = Button(stage, 0, buttonWidth, buttonHeight, 0, height-buttonHeight, (ClutterColor){ 0, 255, 47, 0xFF }, &animation);
    Button button2 = Button(stage, 1, buttonWidth, buttonHeight, buttonWidth, height-buttonHeight, (ClutterColor){ 232, 217, 12, 0xFF }, &animation);
    Button button3 = Button(stage, 2, buttonWidth, buttonHeight, buttonWidth*2, height-buttonHeight, (ClutterColor){ 255, 122, 0, 0xFF }, &animation);
    Button button4 = Button(stage, 3, buttonWidth, buttonHeight, buttonWidth*3, height-buttonHeight, (ClutterColor){ 232, 12, 111, 0xFF }, &animation);
    Button button5 = Button(stage, 4, buttonWidth, buttonHeight, buttonWidth*4, height-buttonHeight, (ClutterColor){ 51, 13, 255, 0xFF }, &animation);

    Button button6 = Button(stage, 5, buttonWidth, buttonHeight, 0, height-(buttonHeight*2), (ClutterColor){ 255, 0, 91, 0xFF }, &animation);
    Button button7 = Button(stage, 6, buttonWidth, buttonHeight, buttonWidth, height-(buttonHeight*2), (ClutterColor){ 55, 8, 232, 0xFF }, &animation);
    Button button8 = Button(stage, 7, buttonWidth, buttonHeight, buttonWidth*2, height-(buttonHeight*2), (ClutterColor){ 0, 228, 255, 0xFF }, &animation);
    Button button9 = Button(stage, 8, buttonWidth, buttonHeight, buttonWidth*3, height-(buttonHeight*2), (ClutterColor){ 30, 232, 12, 0xFF }, &animation);
    Button button10 = Button(stage, 9, buttonWidth, buttonHeight, buttonWidth*4, height-(buttonHeight*2), (ClutterColor){ 255, 215, 0, 0xFF }, &animation);

    // Actually show the stage and run the app:
    clutter_actor_show(stage);
    clutter_main();

    return EXIT_SUCCESS;
}

