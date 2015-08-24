#ifndef ALSA_CTL
#define ALSA_CTL
struct audio_data {
    bool audioLive;
    int audio_out[8192]; // int = 16 bit number
    int format;
    unsigned int rate ;
    char *source; //alsa device or fifo path
    int im; //input mode alsa or fifo
    int readCount; // how many samples we read this pass...
    int actualBufferSize; // How large our buffer actually is based on what the soundcard can give us
    unsigned int channelCount;
};

void* input_alsa(void* data);
void* derpthread(void* data);

#endif
