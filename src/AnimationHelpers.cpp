//
// Created by jamis on 7/25/15.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "p9813.h"
#include "TCLControl.h"
#include "configurations.h"
#include "AnimationHelpers.h"

// Old variables we probably still need:
int W, H;
int timeout = 0;
bool clearing = false;
int cutoff = 0;
gfloat finput_x;
gfloat finput_y;
int old_x, old_y;
int temp_x, temp_y;
int input_x = 1;
int input_y = 1;

gfloat osd_scale = 16;

int cycle = 0;

// int seedHSVColor = getHSVRandomColor();
int tempHSV;
TCpixel *HSVBuf = (TCpixel *) malloc(memSize);
bool set = false;
int tempColor;
int counter = 5;


int totalPixels = TCLControl::nStrands * TCLControl::pixelsPerStrand;
int memSize = totalPixels * sizeof(TCpixel);
TCpixel *pixelBackupBuf = (TCpixel *) malloc(memSize);

int h_angle = 0;



long map(long x, long in_min, long in_max, long out_min, long out_max) {
//    printf("x: %li, in_min: %li, in_max: %li, out_min: %li, out_max: %li\n", x, in_min, in_max, out_min, out_max);
    if (in_max == in_min) {
        in_max = 1;
    }
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


// Unpack and packing colors into and out of int
int pack(int a, int b, int c) { return (a << 16) | (b << 8) | c; }
int unpackA(int color) { return (color >> 16) & 0xff; }
int unpackB(int color) { return (color >> 8) & 0xff; }
int unpackC(int color) { return (color) & 0xff; }

int getRGB(int hue, int sat, int val) {
    // These values should come in in the range of 0-255, not 0-360!
    hue = hue * 359 / 255;
    /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
       The dim_curve is used only on brightness/value and on saturation (inverted).
       This looks the most natural.
    */

    val = dim_curve[val];
    sat = 255 - dim_curve[255 - sat];

    int colors[3];

    int r = 0;
    int g = 0;
    int b = 0;
    int base;

    if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
        colors[0] = val;
        colors[1] = val;
        colors[2] = val;
    } else {

        base = ((255 - sat) * val) >> 8;

        switch (hue / 60) {
            case 0:
                r = val;
                g = (((val - base) * hue) / 60) + base;
                b = base;
                break;

            case 1:
                r = (((val - base) * (60 - (hue % 60))) / 60) + base;
                g = val;
                b = base;
                break;

            case 2:
                r = base;
                g = val;
                b = (((val - base) * (hue % 60)) / 60) + base;
                break;

            case 3:
                r = base;
                g = (((val - base) * (60 - (hue % 60))) / 60) + base;
                b = val;
                break;

            case 4:
                r = (((val - base) * (hue % 60)) / 60) + base;
                g = base;
                b = val;
                break;

            case 5:
                r = val;
                g = base;
                b = (((val - base) * (60 - (hue % 60))) / 60) + base;
                break;
        }

        colors[0] = r;
        colors[1] = g;
        colors[2] = b;
    }

    // unsigned char toRet = TCrgb(r, g, b);
    return TCrgb(colors[0], colors[1], colors[2]);

}

int getRGB(int hsvColor) {
    return getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));
}

// Random functions:

float getrandf() { // must init with: srand(time(NULL));
    return (float)rand();
}

int getrand(int min, int max) {
    if (max == min) {
        max += 1;
    }
    return (rand() % (max - min) + min);
}

int randColor() {
    return getrand(0, 255);
}

int getRandomColor() {
    return TCrgb(randColor(), randColor(), randColor());
}

int getBetterRandomColor() {
    return getRGB(randColor(), 255, 255);
}

int getBetterRandomColor(int sat) {
    return getRGB(randColor(), sat, 255);
}

int getHSVRandomColor() {
    return pack(randColor(), 255, 255);
}

int jitter(int toJitter, int jitterAmount) {
    // printf("Derp: %i   %i\n", toJitter-jitterAmount, toJitter+jitterAmount);
    return getrand(toJitter - jitterAmount, toJitter + jitterAmount);
}

int colorJitter(int color, int jitterAmount) {
    int toRet = color;

    int r = (toRet >> 16) & 0xff;
    int g = (toRet >> 8) & 0xff;
    int b = toRet & 0xff;

    // int new_r = jitter(r, jitterAmount);
    // int new_g = jitter(r, jitterAmount);
    // int new_b = jitter(r, jitterAmount);

    r = jitter(r, jitterAmount);
    g = jitter(r, jitterAmount);
    b = jitter(r, jitterAmount);

    toRet = (r << 16) | (g << 8) | b;

    return toRet;
}

int HSVJitter(int hsvColor, int amt) {
    return pack(jitter(unpackA(hsvColor), amt), unpackB(hsvColor), unpackC(hsvColor));
}

int HSVShift(int hsvColor, int amt) {
    return pack(unpackA(hsvColor) + amt, unpackB(hsvColor), unpackC(hsvColor));
}

int popRainbow(int h_rate) {
    h_angle += h_rate;

    int toRet = getRGB(h_angle, 255, 255);

    if (h_angle >= 255) {
        h_angle = 0;
    }
    return toRet;
}

int popHSVRainbow(int h_rate) {
    h_angle += h_rate;

    int toRet = pack(h_angle, 255, 255);

    if (h_angle >= 255) {
        h_angle = 0;
    }
    return toRet;
}

int invertHSV(int hsvColor) {
    int temp = unpackA(hsvColor);
    temp = temp + 126;
    if (temp > 255) {
        temp -= 255;
    }
    return pack(temp, unpackB(hsvColor), unpackC(hsvColor));
}

int popCycle(int rate) {
    cycle += rate;
    return cycle;
}




//==============================================
// Old animations:

void animation10(TCLControl *tcl) {
    cutoff += 1;
    if (cutoff > (input_y * 50 / 255)) {
        cutoff = 0;
        W = getrand(0, WIDTH);
        H = getrand(0, HEIGHT);
    }

    timeout += 1;
    if (timeout > (input_x * 100 / 255)) {
        timeout = 0;
        clearing = true;
    }

    // int thresh = 4;

    int temp = popHSVRainbow(1);
    int inverted_temp = invertHSV(temp);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            if (abs(W - x) < 2 && abs(H - y) < 2) {
                tcl->pixelBuf[index] = getRGB(temp);
            } else {
                if (clearing) {
                    tcl->pixelBuf[index] = getRGB(inverted_temp);
                }
            }
            index += 1;
        }
    }
    if (clearing) {
        clearing = false;
    }
}

void animation9(TCLControl *tcl) {
    // printf("input_x: %i \ninput_y: %i\n\n", input_x, input_y);
    // int jitterAmount = input_y*50/255;
    // printf( "Jitter: %i\n", jitterAmount);

    int rainbowPass = popHSVRainbow(input_y * 50 / 255);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            pixelBackupBuf[index] = tcl->pixelBuf[index];
            index += 1;
        }
    }

    index = 0;
    for (int x = 0; x < WIDTH; x++) {
        // int wandOffset = input_x*11/255;
        for (int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            int hsvColor = rainbowPass; //HSVJitter(rainbowPass, jitterAmount);
            int jitColor = getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));

            if (y >= 1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index - 1];
            } else {
                if (input_x == old_x && input_y == old_y) {
                    tcl->pixelBuf[index] = tcl->pixelBuf[index + 49];
                } else {
                    if (x == input_x * 12 / 255) {
                        tcl->pixelBuf[index] = jitColor;
                    }
                }


            }

            index += 1;
        }
    }
    old_x = input_x;
    old_y = input_y;
}

void animation8(TCLControl *tcl) {
    // printf("input_x: %i \ninput_y: %i\n\n", input_x, input_y);
    // int jitterAmount = input_y*50/255;
    // printf( "Jitter: %i\n", jitterAmount);

    int rainbowPass = popHSVRainbow(input_x * 10 / 255);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            int hsvColor = rainbowPass; //HSVJitter(rainbowPass, jitterAmount);
            int jitColor = getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));

            if (y >= 1 && y < input_y * 50 / 255) {
                tcl->pixelBuf[index] = pixelBackupBuf[index - 1];
            } else if (y >= input_y * 50 / 255) {
                if (x == 11) {
                    tcl->pixelBuf[index] = pixelBackupBuf[index - 49];
                } else {
                    tcl->pixelBuf[index] = pixelBackupBuf[index + 1];
                }
            } else {
                tcl->pixelBuf[index] = jitColor;
            }

            index += 1;
        }
    }
}

void animation7(TCLControl *tcl) {

    // Handle inputs:
    int rate = input_y * 50 / 255;
    int skew = popCycle(input_x * 50 / 255);
    rate = rate - rate / 2;

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        // h_angle = 0;
        for (int y = 0; y < HEIGHT; y++) {


            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            tcl->pixelBuf[index] = getRGB(HSVShift(popHSVRainbow(rate), skew));

            index += 1;
        }
    }
}

// Old light update:
// Calculate the new values for the pixelBuf:
// Update the lights:
// x += (double)tcl.pixelsPerStrand / 20000.0;
// s1 = sin(x                 ) *  11.0;
// s2 = sin(x *  0.857 - 0.214) * -13.0;
// s3 = sin(x * -0.923 + 1.428) *  17.0;
// for(i=0;i<tcl.totalPixels;i++)
// {
//     r   = (int)((sin(s1) + 1.0) * 127.5);
//     g   = (int)((sin(s2) + 1.0) * 127.5);
//     b   = (int)((sin(s3) + 1.0) * 127.5);
//     tcl.pixelBuf[i] = TCrgb(r,g,b);
//     s1 += 0.273;
//     s2 -= 0.231;
//     s3 += 0.428;
// }

void animation6(TCLControl *tcl) {
    // printf("input_x: %i \ninput_y: %i\n\n", input_x, input_y);
    int jitterAmount = input_y * 50 / 255;
    // printf( "Jitter: %i\n", jitterAmount);

    int rainbowPass = popHSVRainbow(input_x * 10 / 255);

    int index = 0;
    for (int x = 0; x < 1; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for (int x = 0; x < 1; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            int hsvColor = HSVJitter(rainbowPass, jitterAmount);
            int jitColor = getRGB(unpackA(hsvColor), unpackB(hsvColor), unpackC(hsvColor));

            if (y >= 1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index - 1];
            } else {
                tcl->pixelBuf[index] = jitColor;
            }

            index += 1;
        }
    }
}

void animation5(TCLControl *tcl) {
    // printf("input_x: %i \ninput_y: %i\n\n", input_x, input_y);
    if (input_x != old_x || input_y != old_y) {
        tempHSV = pack(input_y, 255, 255);
    } else {
        tempHSV = HSVJitter(tempHSV, input_x*5/255);
    }


    int index = 0;
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            tcl->pixelBuf[index] = getRGB(tempHSV);

            index += 1;
        }
    }

    old_x = input_x;
    old_y = input_y;
}

void animation4(TCLControl *tcl) {

    int temp = popRainbow(input_y * 100 / 255);

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            if (y == input_y * WIDTH / 255) {
                tcl->pixelBuf[index] = temp;
            } else {
                tcl->pixelBuf[index] = tcl->pixelBuf[index ^ (input_x * HEIGHT / 255)] + temp;
            }

            // tcl->pixelBuf[index] = temp;

            index += 1;
        }
    }
}


void animation3(TCLControl *tcl) {
    cutoff += 1;
    if (cutoff > (input_y * 50 / 255)) {
        cutoff = 0;

        for (int i = 0; i < tcl->totalPixels; i++) {
            // int temp = TCrgb(200,200,10);
            // int temp = (200<<16)|(200<<8)|10;
            // int temp = getRGB(0,255,255);
            int temp = popRainbow(input_x * 10 / 255);
            tcl->pixelBuf[i] = temp;
        }
    }
}

void animation2(TCLControl *tcl) {

    if (!set) {
        for (int i = 0; i < tcl->totalPixels; i++) {
            HSVBuf[i] = pack(255, 255, 255);
        }
        set = true;
    }

    // int nextSeed = getrand(0,tcl->totalPixels);
    // int tempColor;

    // for(int i=0; i < tcl->totalPixels; i++) {
    //     tempColor = HSVJitter(seedHSVColor, 1);
    //     tcl->pixelBuf[i] = getRGB(unpackA(tempColor), unpackB(tempColor), unpackC(tempColor));
    // }

    // seedHSVColor = tcl->pixelBuf[nextSeed];

    // int temp = getBetterRandomColor();

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            HSVBuf[index] = HSVJitter(HSVBuf[index], input_y * 50 / 255);

            index += 1;
        }
    }

    // index = 0;
    // for(int x = 0; x < WIDTH; x++) {
    //     for(int y = 0; y < HEIGHT; y++) {

    //         // pixelBackupBuf[index] = tcl->pixelBuf[index];

    //         if (y>=1) {
    //             tcl->pixelBuf[index] = pixelBackupBuf[index-1];
    //         } else {
    //             tcl->pixelBuf[index] = temp;
    //         }

    //         index += 1;
    //     }
    // }

    for (int i = 0; i < tcl->totalPixels; i++) {
        tcl->pixelBuf[i] = getRGB(unpackA(HSVBuf[i]), unpackB(HSVBuf[i]), unpackC(HSVBuf[i]));
    }

}

void animation1(TCLControl *tcl) {

    counter += 1;
    if (counter > (input_y * 50 / 255)) {
        int sat = input_x;
        tempColor = getBetterRandomColor(sat);
        counter = 0;
    }

    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // // This next line grabs the address of single pixel out of the pixels char buffer
            // // and points a char at it so that it's value can be set:
            // unsigned char* pixel =  &pixels[y * rowstride + x * 3];

            pixelBackupBuf[index] = tcl->pixelBuf[index];

            index += 1;
        }
    }

    index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // pixelBackupBuf[index] = tcl->pixelBuf[index];

            if (y >= 1) {
                tcl->pixelBuf[index] = pixelBackupBuf[index - 1];
            } else {
                tcl->pixelBuf[index] = tempColor;
            }

            index += 1;
        }
    }
}