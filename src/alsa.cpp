#include "alsa.h"
#include <alsa/asoundlib.h>


void* derpthread(void* data) {
    printf("Woo. Threading.\n");
    return 0;
}

// input: ALSA
void* input_alsa(void* data)
{
    audio_data *audio = (struct audio_data *)data;
    char *buffer;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    unsigned int channelCount = 2;
    unsigned int sampleRate = 44100;

    unsigned int val;
    unsigned long bufferSize = 0;
    unsigned long periodSize = 0;

    int i, size, dir, err;
//    int n, o, lo;
//    int tempr, templ;
//    int radj, ladj;

    // alsa: open device to capture audio
    if ((err = snd_pcm_open(&handle, audio-> source, SND_PCM_STREAM_CAPTURE, 0) < 0)) {
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


    // These values are pretty small, might be useful in situations where latency is a dirty word.
//    snd_pcm_uframes_t buffer_size = 2048;
//    snd_pcm_uframes_t period_size = 64;
//
//    snd_pcm_hw_params_set_buffer_size_near (pcm_handle, hw_params, &buffer_size);
//    snd_pcm_hw_params_set_period_size_near (pcm_handle, hw_params, &period_size, NULL);
    //snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir); //number of frames pr read

    // Set the parameters we just configured onto the hardware itself:
    err = snd_pcm_hw_params(handle, params);
    if (err < 0) {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }


    // Get some debug values and store them in the audio data struct so we can use them elsewhere:
    snd_pcm_hw_params_get_format(params, (snd_pcm_format_t * )&val);// Audio format enum (snd_pcm_format_t):
    if (val < 6) {
        audio->format = 16;
    } else if (val > 5 && val < 10) {
        audio->format = 24;
    }else if (val > 9) {
        audio->format = 32;
    }
    snd_pcm_hw_params_get_rate( params, &audio->rate, &dir); // Get sample rate
    snd_pcm_hw_params_get_channels(params, &audio->channelCount); // Get Channel count
    snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *)&bufferSize); // Get buffer size
    snd_pcm_hw_params_get_period_size(params, &periodSize, &dir); // Get period size
    printf("Buffer size: %d, Period size: %d\n", bufferSize);

    // Now, I have no idea what he's doing here but I think this is wrong:
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    snd_pcm_hw_params_get_period_time(params,  &val, &dir);

//    size = frames * (audio->format / 8) * channelCount; // frames * 16 bits / 8 * 2 channels
    size = periodSize;
    buffer = (char *) malloc(size);
    audio->actualBufferSize = size;
//    radj = audio->format / 4; //adjustments for interleaved
//    ladj = audio->format / 8;
//    o = 0;



    int leftOffset = 16 / 4;
    while (1) {

        err = snd_pcm_readi(handle, buffer, frames);
        if (err == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (err < 0) {
            fprintf(stderr, "error from read: %s\n", snd_strerror(err));
        } else if (err != (int)frames) {
            fprintf(stderr, "short read, read %d %d frames\n", err, (int)frames);
        }

        if (err >=0) {
            audio->readCount = err;
        }

        // Forget cava's bullshit.
        // err is how many "frames" we read.
        // frames = "sample" in mono land...
        // frames = left+right pair in stereo land

        for (i = 0; i < err; i += leftOffset) {

            audio->audio_out[i] = buffer[i]<<8|buffer[i+1]; // 16bit little endian. Never can remember....
//            audio->audio_out[i] = buffer[i+1]<<8|buffer[i]; // 16bit little endian. Never can remember....
        }

        //sorting out one channel and only biggest octet
//        n = 0; //frame counter
//        for (i = 0; i < size ; i = i + (ladj) * 2) {
//
//            //first channel
//            //using the 10 upper bits this would give me a vert res of 1024, enough...
//            tempr = ((buffer[i + (radj) - 1 ] << 2));
//            lo = ((buffer[i + (radj) - 2] >> 6));
//            if (lo < 0)lo = abs(lo) + 1;
//            if (tempr >= 0)tempr = tempr + lo;
//            if (tempr < 0)tempr = tempr - lo;
//
//            //other channel
//            templ = (buffer[i + (ladj) - 1] << 2);
//            lo = (buffer[i + (ladj) - 2] >> 6);
//            if (lo < 0)lo = abs(lo) + 1;
//            if (templ >= 0)templ = templ + lo;
//            else templ = templ - lo;
//
//            //adding channels and storing it in the buffer
//            audio->audio_out[o] = (tempr + templ) / 2;
////            printf("%i, ", buffer[i]);
//            o++;
//            if (o == 2048 - 1)o = 0;
//
//            n++;
//        }

    }
}

