//
// Created by jamis on 7/25/15.
//

#ifndef RUNMAKE_ANIMATIONHELPERS_H
#define RUNMAKE_ANIMATIONHELPERS_H

// Define the sizes of the image that shows the lights:
#define HEIGHT 50
#define WIDTH 12
#define TCrgb(R, G, B) (((R) << 16) | ((G) << 8) | (B))

gfloat osd_scale = 16;

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

int totalPixels = TCLControl::nStrands * TCLControl::pixelsPerStrand;
int memSize = totalPixels * sizeof(TCpixel);
TCpixel *pixelBackupBuf = (TCpixel *) malloc(memSize);

// int seedHSVColor = getHSVRandomColor();
int tempHSV;
TCpixel *HSVBuf = (TCpixel *) malloc(memSize);
bool set = false;
int tempColor;
int counter = 5;


// Unpack and packing colors into and out of int
int pack(int a, int b, int c) { return (a << 16) | (b << 8) | c; }
int unpackA(int color) { return (color >> 16) & 0xff; }
int unpackB(int color) { return (color >> 8) & 0xff; }
int unpackC(int color) { return (color) & 0xff; }

// Map one set of values to another:
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// convert HSB to RGB:
int rgb_colors[3]; // holder for rgb color from hsb conversion
int hue;
int saturation;
//int brightness; // defined above, set using AN3
const unsigned char dim_curve[] = {
        0, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6,
        6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8,
        8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11,
        11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15,
        15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20,
        20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26,
        27, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35,
        36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 46, 47,
        48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
        63, 64, 65, 66, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 81, 82,
        83, 85, 86, 88, 90, 91, 93, 94, 96, 98, 99, 101, 103, 105, 107, 109,
        110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
        146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
        193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

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

int h_angle = 0;

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

int cycle = 0;

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

#endif //RUNMAKE_ANIMATIONHELPERS_H
