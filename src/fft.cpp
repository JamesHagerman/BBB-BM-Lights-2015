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

#define PI 3.141592653589793 // ya know... for our waveform generator :facepalm:

pthread_t p_thread;
int thr_id;
struct audio_data audio;


int samples_count;
double *samples;
fftw_complex *output;
fftw_plan plan;
double max;


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

    // Fill buffers with zeros. Because stupid shit happens otherwise:
    for (i = 0; i < samples_count; i++) {
        samples[i] = 0;
        output[i] = 0;
    }

    // Build the plan:
    plan = fftw_plan_dft_r2c_1d(samples_count, samples, output, FFTW_ESTIMATE); // better to use FFTW_MEASURE

}

void executeFFT() {

    int i;

    int actualBufferSize =  audio.actualBufferSize; // number of actual 16bit audio values we're reading in
    samples_count = actualBufferSize;//2 * (actualBufferSize / 2 + 1);
//    printf("FFT EXECUTE: samples_count: %i\n", samples_count);

    // Only try doing the FFT if we actually have audio data:
    if (audio.audioLive && samples_count>0) {
        audio.lockAudio = true;


        for (i = 0; i < samples_count; i++) {
            printf("%i, ", audio.audio_out[i]);
            samples[i] = audio.audio_out[i];

            // generate a sine wave:
//            printf("%f, ", sin((i*100)*PI/180));
//            samples[i] = sin((i*100)*PI/180);
        }
        printf("Audio read END\n");

        // compute fftw
        fftw_execute(plan);

        // The next for loop pulls the absolute value of the complex fft output and plops it back into
        // the INPUT ARRAY! (named samples here). This allows us to reuse samples for "maximizing" and
        // maybe even smoothing...
        for (i = 0; i < samples_count / 2; i++) {
            samples[i] = 10 * log(creal(output[i])*creal(output[i]) + cimag(output[i])*cimag(output[i]))/log(10); // power_in_db
            if (samples[i] > max) {
                max = samples[i];
            }
        }

        // output:
        for (i = 0; i < samples_count / 2; i++) {
            printf("%.1f, ", samples[i]);
        }
        printf("FFT compute end, max = %.1f\n", max);
        max = 0; // reset for next pass

        audio.lockAudio = false;
    }
//    else {
//        printf("waiting for audio...\n");
//    }
}


void teardownFFT() {
    printf("Tearing down FFT...\n");
    fftw_destroy_plan(plan);
}