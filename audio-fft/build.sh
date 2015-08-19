#!/bin/bash


arm-angstrom-linux-gnueabi-gcc -o main ./main.c `pkg-config --cflags --libs alsa ncurses fftw3 sndfile` -lm


