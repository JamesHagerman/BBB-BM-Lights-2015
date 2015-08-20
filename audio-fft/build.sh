#!/bin/bash

CC=arm-angstrom-linux-gnueabi-gcc
CC=gcc
$CC -o main ./main.c `pkg-config --cflags --libs alsa ncurses fftw3 sndfile` -lm


