#include "fft.h"
#include "alsa.h"
#include <alsa/asoundlib.h>

bool audioDying;
snd_pcm_t *handle;

void* derpthread(void* data) {
    printf("Woo. Threading.\n");
    return 0;
}

// input: ALSA
void* input_alsa(void* data)
{
    audio_data *audio = (struct audio_data *)data;
    audio->audioLive = false;
    audioDying = false;
    audio->lockAudio = false;
    char *buffer;
    snd_pcm_hw_params_t *params;
    unsigned int channelCount = 1;
    unsigned int sampleRate = 44100;

    unsigned int val;
    unsigned int periods = 2;
    unsigned int periodTime = 0;
    unsigned long periodSize = 4096; // in frames
    unsigned long bufferSize = periodSize * periods; // total number of frames * channel count * (size of data)

    int i, size, dir, err;

    // alsa: open device to capture audio
    if ((err = snd_pcm_open(&handle, audio->source, SND_PCM_STREAM_CAPTURE, 0) < 0)) {
        fprintf(stderr, "error opening stream: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    } else {
        printf("stream opened successfully\n");
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any (handle, params); //get current settings
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED); // interleaved mode right left
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE); // 16bit little endian
    snd_pcm_hw_params_set_channels(handle, params, channelCount);
    snd_pcm_hw_params_set_rate_near(handle, params, &sampleRate, 0); //trying 44100 rate


    // Periods are some number of frames long.
    snd_pcm_hw_params_set_periods(handle, params, periods, 0);

    // Each period has some given size:
    snd_pcm_hw_params_set_period_size_near(handle, params, &periodSize, &dir);

    // The buffer needs to be at least 2 times the length of a single period:
//    snd_pcm_hw_params_set_buffer_size_near(handle, params, &bufferSize);

    // Set the parameters we just configured onto the hardware itself:
    err = snd_pcm_hw_params(handle, params);
    if (err < 0) {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }


    // Get some debug values and store them in the audio data struct so we can use them elsewhere:
    snd_pcm_hw_params_get_format(params, (snd_pcm_format_t * )&val);// Audio format enum (snd_pcm_format_t):
    printf("Actual format we were able to chose on the hardware: %u\n", val);
    if (val < 6) {
        audio->format = 16;
    } else if (val > 5 && val < 10) {
        audio->format = 24;
    }else if (val > 9) {
        audio->format = 32;
    }
    snd_pcm_hw_params_get_rate( params, &audio->rate, &dir); // Get sample rate
    snd_pcm_hw_params_get_channels(params, &audio->channelCount); // Get Channel count

    // Stuff we don't need to use anywhere else
    snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *)&bufferSize); // Get buffer size
    snd_pcm_hw_params_get_period_size(params, &periodSize, &dir); // Get period size
    snd_pcm_hw_params_get_period_time(params,  &periodTime, &dir);
    printf("Buffer size: %lu, Period size: %lu, Period time: %u \n", bufferSize, periodSize, periodTime);

    printf("Audio format: %i\n", audio->format);

    // Calculate buffer size manually:
    size = periodSize * audio->channelCount * (audio->format / 8);
    printf("Buffer size: %i\n", size);
    buffer = (char *) malloc(size);

    printf("Period size: %li\n", periodSize);
    audio->actualBufferSize = periodSize/2; // The period is a bunch of 16bit values... not 8 bit values.

    int offset;
    while (1) {
        if (!audioDying) { //!audio->lockAudio &&
            // err is how many "frames" we read.
            // frames = "sample" in mono land...
            // frames = left+right pair in stereo land

    //        printf("Reading %i samples from buffer\n", periodSize);
            err = snd_pcm_readi(handle, buffer, periodSize); // frames = how many frames to be read
            if (err == -EPIPE) {
                /* EPIPE means overrun */
                fprintf(stderr, "overrun occurred\n");
                snd_pcm_prepare(handle);
            } else if (err < 0) {
                fprintf(stderr, "error from read: %s\n", snd_strerror(err));
            } else if (err != (int)periodSize) {
                fprintf(stderr, "short read, read %d %d frames\n", err, (int)periodSize);
            }

            if (err >=0) {
                audio->readCount = err;
            }

            // Grab the 16 bit audio data!!
            offset = 0;
            for (i = 0; i < err; i += 2) {
                audio->audio_out[offset] = buffer[i];//|buffer[i+1]<<8; // 16bit little endian. Never can remember....
                offset+= 1;
            }

            // Tell the rest of the app that we are actually capturing audio data:
            audio->audioLive = true;

        }
    }
}

void teardownAlsa() {
    printf("Tearing down ALSA...\n");
    audioDying = true;
    snd_pcm_close(handle);
}

