#ifndef ALSA_CTL
#define ALSA_CTL
struct audio_data {
    int audio_out[2048];
    int format;
    unsigned int rate ;
    char *source; //alsa device or fifo path
    int im; //input mode alsa or fifo
    int readCount; // how many samples we read this pass...
    int actualBufferSize; // How large our buffer actually is based on what the soundcard can give us
};

void* input_alsa(void* data);
void* derpthread(void* data);

#endif
