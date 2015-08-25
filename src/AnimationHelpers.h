#ifndef RUNMAKE_ANIMATIONHELPERS_H
#define RUNMAKE_ANIMATIONHELPERS_H

// Define the sizes of the image that shows the lights:
#define TCrgb(R, G, B) (((R) << 16) | ((G) << 8) | (B))
#define HEIGHT 50
#define WIDTH 12

extern gfloat osd_scale;

// Old variables we probably still need:
extern int W, H;
extern int timeout;
extern bool clearing;
extern int cutoff;
extern gfloat finput_x;
extern gfloat finput_y;
extern int old_x, old_y;
extern int temp_x, temp_y;
extern int input_x;
extern int input_y;
extern int cycle;

// int seedHSVColor = getHSVRandomColor();
extern int tempHSV;
extern TCpixel *HSVBuf;
extern bool set;
extern int tempColor;
extern int counter;

extern int totalPixels;
extern int memSize;
extern TCpixel *pixelBackupBuf;

// convert HSB to RGB:
extern int rgb_colors[3]; // holder for rgb color from hsb conversion
extern int hue;
extern int saturation;
//int brightness; // defined above, set using AN3

extern int h_angle;

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

long map(long x, long in_min, long in_max, long out_min, long out_max);


// Unpack and packing colors into and out of int
int pack(int a, int b, int c);
int unpackA(int color);
int unpackB(int color);
int unpackC(int color);

int getRGB(int hue, int sat, int val);

int getRGB(int hsvColor);

// Random functions:
// must init with: srand(time(NULL));
float getrandf();
int getrand(int min, int max);
int randColor();
int getRandomColor();
int getBetterRandomColor();
int getBetterRandomColor(int sat);
int getHSVRandomColor();

int jitter(int toJitter, int jitterAmount) ;
int colorJitter(int color, int jitterAmount);
int HSVJitter(int hsvColor, int amt);
int HSVShift(int hsvColor, int amt);

int popRainbow(int h_rate);
int popHSVRainbow(int h_rate);
int invertHSV(int hsvColor);

int popCycle(int rate);

//==============================================
// Old animations:

void animation10(TCLControl *tcl);
void animation9(TCLControl *tcl);
void animation8(TCLControl *tcl);
void animation7(TCLControl *tcl);
void animation6(TCLControl *tcl);
void animation5(TCLControl *tcl);
void animation4(TCLControl *tcl);
void animation3(TCLControl *tcl);
void animation2(TCLControl *tcl);
void animation1(TCLControl *tcl);

#endif //RUNMAKE_ANIMATIONHELPERS_H
