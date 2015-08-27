#ifndef FFT_H
#define FFT

void initAlsa();
void executeFFT(unsigned char *audioPixels, int audioRowstride);
void teardownFFT();

#endif //FFT
