#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <string.h>

// Order of these two matters. Complex needs to be first:
// This is a c library:
#include <complex.h>
#include <fftw3.h>
#include "fft.h"
#include "alsa.h"

#include "AnimationHelpers.h"

#define PI 3.141592653589793 // ya know... for our waveform generator :facepalm:

pthread_t p_thread;
int thr_id;
struct audio_data audio;


int samples_count;
double *samples;
fftw_complex *output;
fftw_plan plan;

double realPart, imgaPart;

int freqBands = 50; // how many frequency bands we want in the end
int binsPerBand = 1024/freqBands; // how many bins get bucketed
long max;
long min;

int peakIndex = 0;


//=======================
// ALSA setup. This REALLY needs to be moved into a class...
void initAlsa() {

    int i;


    audio.format = -1;
    audio.rate = 0;
    audio.source = new char[10];

    // hw:1,0 = BBB usb snd card
    // hw:0,1 = Ubuntu mic input...
#ifdef BEAGLEBONEBLACK
    strncpy( audio.source, "hw:1,0", 9 ); // BBB
#else
    strncpy( audio.source, "hw:0,1", 9 ); // UBUNTU
#endif

    // Actually kick off the pthread that will grab audio:
    thr_id = pthread_create(&p_thread, NULL, input_alsa, (void *)&audio); //starting alsamusic listener

    // Check to make sure the audio is actually working...
    // This is kind of a drag.... it just sleeps until the audio is live...
    struct timespec req = { .tv_sec = 0, .tv_nsec = 0 };
    i = 0;
    while (audio.format == -1 || audio.rate == 0 || audio.actualBufferSize == 0) {
        req.tv_sec = 0;
        req.tv_nsec = 1000000;
        nanosleep(&req, NULL);
        i++;
        if (i > 2000) {
#ifdef DEBUG
            fprintf(stderr, "could not get rate and/or format, problems with audio thread? quiting...\n");
#endif
            exit(EXIT_FAILURE);
        }
    }
#ifdef DEBUG
    printf("got format: %d, rate: %d, channels: %d, buffer size: %d\n", audio.format, audio.rate, audio.channelCount, audio.actualBufferSize);
#endif

    // Just a test thread to see if they even work on ARM:
//    int dumb = 12;
//    thr_id = pthread_create(&p_thread, NULL, derpthread, (void *)&dumb); // STUPID THREAD.

    // Setup fftw3:
    int actualBufferSize =  audio.actualBufferSize; // number of actual 16bit audio values we're reading in
    samples_count = actualBufferSize;//2 * (actualBufferSize / 2 + 1);
    samples = fftw_alloc_real(samples_count);
    output = fftw_alloc_complex(samples_count);

    // A little cleanup help:
    max = 0;
    min = 0;

    // Fill buffers with zeros. Because stupid shit happens otherwise:
    for (i = 0; i < samples_count; i++) {
        samples[i] = 0;
        output[i] = 0;
    }

    // Build the plan:
    plan = fftw_plan_dft_r2c_1d(samples_count, samples, output, FFTW_ESTIMATE); // better to use FFTW_MEASURE

}

//=============
// Windowing functions:
/* Reference: "Numerical Recipes in C" 2nd Ed.
 * by W.H.Press, S.A.Teukolsky, W.T.Vetterling, B.P.Flannery
 * (1992) Cambridge University Press.
 * ISBN 0-521-43108-5
 * Sec.13.4 - Data Windowing
 */
double parzen(int i, int nn) {
    return (1.0 - fabs(((double) i - 0.5 * (double) (nn - 1))
                       / (0.5 * (double) (nn + 1))));
}

double welch(int i, int nn) {
    return (1.0 - (((double) i - 0.5 * (double) (nn - 1))
                   / (0.5 * (double) (nn + 1)))
                  * (((double) i - 0.5 * (double) (nn - 1))
                     / (0.5 * (double) (nn + 1))));
}

double hanning(int i, int nn) {
    return (0.5 * (1.0 - cos(2.0 * M_PI * (double) i / (double) (nn - 1))));
}

/* Reference: "Digital Filters and Signal Processing" 2nd Ed.
 * by L. B. Jackson. (1989) Kluwer Academic Publishers.
 * ISBN 0-89838-276-9
 * Sec.7.3 - Windows in Spectrum Analysis
 */
double hamming(int i, int nn) {
    return (0.54 - 0.46 * cos(2.0 * M_PI * (double) i / (double) (nn - 1)));
}

double blackman(int i, int nn) {
    return (0.42 - 0.5 * cos(2.0 * M_PI * (double) i / (double) (nn - 1))
            + 0.08 * cos(4.0 * M_PI * (double) i / (double) (nn - 1)));
}

double steeper(int i, int nn) {
    return (0.375
            - 0.5 * cos(2.0 * M_PI * (double) i / (double) (nn - 1))
            + 0.125 * cos(4.0 * M_PI * (double) i / (double) (nn - 1)));
}

void executeFFT(unsigned char *audioPixels, int audioRowstride) {

    int i;
    max = 0; // reset for next pass
    min = 0;

    int actualBufferSize =  audio.actualBufferSize; // number of actual 16bit audio values we're reading in
    samples_count = actualBufferSize;//2 * (actualBufferSize / 2 + 1);
//    printf("FFT EXECUTE: samples_count: %i\n", samples_count);

    // Only try doing the FFT if we actually have audio data:
    if (audio.audioLive && samples_count>0) {
        audio.lockAudio = true;

        // Read in values from the sound card:
        for (i = 0; i < samples_count; i++) {
//#ifdef BEAGLEBONEBLACK
//            printf("%d, ", audio.audio_out[i]);
            samples[i] = audio.audio_out[i] * hanning(i, samples_count); // Apply a window function
//#else

            // generate a sine wave:
//            samples[i] = (sin((i*10)*PI/180)*7000) * hanning(i, samples_count);;
//#endif

            if (round(samples[i]) > max) {
                max = round(samples[i]);
            }
            if (round(samples[i]) < min) {
                min = round(samples[i]);
            }

        }
//        printf("Audio read END, max = %li, min = %li\n", max, min);
        min = 0.0;
        max = 0.0;

        // compute fftw
        fftw_execute(plan);

        // The next for loop pulls the absolute value of the complex fft output and plops it back into
        // the INPUT ARRAY! (named samples here). This allows us to reuse samples for "maximizing" and
        // maybe even smoothing...
        for (i = 0; i < samples_count / 2; i++) {
            realPart = creal(output[i])*(2./samples_count);
            imgaPart = cimag(output[i])*(2./samples_count);
            samples[i] = 10. * log(realPart*realPart + imgaPart*imgaPart)/log(10); // power_in_db

            // After this, the values should be between 0.0 and 1.0:
            if (samples[i] < 0) {
                samples[i] = 0;
            } else {
                //samples[i] /= 24.0; // Divide the results by 96dB
//                if (samples[i] > 1) {
//                    samples[i] = 1;
//                }
            }

            // The REAL values of the FFT are not important right now:
            if (round(samples[i]) > max) {
                max = round(samples[i]);
                peakIndex = i;
            }
            if (round(samples[i]) < min) {
                min = round(samples[i]);
            }
        }

        // output:
//        for (i = 0; i < samples_count / 2; i++) {
//        for (i = 0; i < freqBands; i++) {
//            printf("%.1f, ", samples[i]);
//        }
//        printf("FFT compute end, max = %li, min = %li, max peak at index: %i\n", max, min, peakIndex);

        // This will average the bands together
        int currentBand = 1; // start at 1 to drop the DC offset...
        peakIndex = 0;
        max = 0;
        min = 0;
        while (currentBand < freqBands) {
            for (int bin = 0; bin < binsPerBand; bin++) {
                samples[currentBand] += samples[(currentBand*binsPerBand)+bin];
            }
            samples[currentBand] = samples[currentBand]/binsPerBand;
            if (round(samples[currentBand]) > max) {
                max = round(samples[currentBand]);
                peakIndex = currentBand;
            }
            if (round(samples[currentBand]) < min) {
                min = round(samples[currentBand]);
            }

            currentBand++;
        }

//        printf("FFT averaging end, max = %li, min = %li, max peak at index: %i\n\n", max, min, peakIndex);

//        std::string key ("tunnel1.frag");
//        std::size_t found = shaderName.rfind(key);
//        if (found!=std::string::npos) {
            int sampleCount = 0;
            long converted = 0;
            for (int x = 0; x < WIDTH; x++) {
                for (int y = 0; y < HEIGHT; y++) {

                    // Find the ADDRESS of each pixel in the pixbuf via the raw char buffer we built...
                    // and bind it to a pointer to a char...
                    unsigned char *pixel = &audioPixels[y * audioRowstride + x * 3];

                    // And directly update that memory location with a new color
                    // This AUTOMATICALLY updates the color of the pixbuf!
                    // It's just hitting the memory directly!

                    converted = round(samples[y]);
                    converted = map(converted, 0, max, 0, 255);

                    pixel[0] = converted;// & 0xff;   // low bits...
                    pixel[1] = getrand(0,255);//converted & 0xff00; //((int)(samples[y]-min)) & 0xff00; // high bits.
                    pixel[2] = 0;//getrand(0, 255);     // some random value could be used for noise...
                    sampleCount++;
                    if (sampleCount > 600) {
                        x= WIDTH+1;
                        y= HEIGHT+1;
                    }

//                    // Show x bounds:
//                    if (x==0 && y==0) {
//                        pixel[0] = 255;
//                        pixel[1] = 255;
//                        pixel[2] = 255;
//                    } else if (x==11 && y==0) {
//                        pixel[0] = 255;
//                        pixel[1] = 255;
//                        pixel[2] = 255;
//                    }
//
//                    // Show y bounds:
//                    if (x==0 && y==49) {
//                        pixel[0] = 255;
//                        pixel[1] = 255;
//                        pixel[2] = 255;
//                    } else if (x==11 && y==49) {
//                        pixel[0] = 255;
//                        pixel[1] = 255;
//                        pixel[2] = 255;
//                    }
                }
            }
//        }





        peakIndex = 0; // reset peak detector

        audio.lockAudio = false;
    }
}

void populateTexture() {

}


void teardownFFT() {
    printf("Tearing down FFT...\n");
    fftw_destroy_plan(plan);
}