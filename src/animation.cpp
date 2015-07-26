#include "animation.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
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

typedef struct {
    ClutterActor *infoDisplay;
    TCLControl *tcl;
    int *animationNumber;
} AnimationData;

typedef struct {
    ClutterActor *lightDisplay;
    ClutterEffect *shaderEffect;
    TCLControl *tcl;
    int *animationNumber;
    gfloat *input_x;
    gfloat *input_y;
} TouchData;

// Hold on to the Color Display:
ClutterContent *colors;
ClutterActor *lightDisplay;
gfloat osd_scale = 16;

GdkPixbuf *pixbuf;
unsigned char *pixels;
int rowstride;
// We need an error object to store errors:
GError *error;
int AnimationID = 0;

// GLSL Shader stuff:
ClutterEffect *shaderEffect;
guint8 *shaderBuffer;
gfloat animationTime = 100.0; // A variable to hold the value of iGlobalTime

// These are the pre and postambles for the Shader Toy shader import system.
// Nothing too complex can run very well on the BBB GPU but it's better than nothing!
const gchar *fragShaderPreamble = "" //"#version 110\n\n"
        "uniform float iGlobalTime;\n"
        "uniform vec2 iResolution;\n"
        "uniform vec2 iMouse;\n";

const gchar *fragShaderPostamble = ""
        "void main(void) {\n"
        "   vec4 outFragColor = vec4(1.0,0.5,0,0);\n"
        "   vec2 inFragCoord = vec2(cogl_tex_coord_in[0].x*iResolution.x, cogl_tex_coord_in[0].y*iResolution.y);\n"
        "   mainImage(outFragColor, inFragCoord);\n"
        "   cogl_color_out = outFragColor;\n"
        "}";

const gchar *fragShader = ""
        "void mainImage(out vec4 fragColor, in vec2 fragCoord) {\n"
        "   vec2 uv = fragCoord.xy / iResolution.xy;\n"
        "   fragColor = vec4(uv.x, uv.y, 0.5+0.5*sin(iGlobalTime), 1.0);\n"
        "}\n";

//const gchar *fragShader = "" //"#version 110\n\n"
//        "uniform float iGlobalTime;\n"
//        "uniform float width;\n"
//        "uniform float height;\n"
//        "void main(void) {\n"
//        "   vec2 res = vec2(50, 12);\n"
//        "   cogl_color_out = vec4(cogl_tex_coord_in[0].x+sin(iGlobalTime*0.05), cogl_tex_coord_in[0].y+cos(iGlobalTime*0.01), sin(iGlobalTime*cogl_tex_coord_in[0].x*500.0 + cogl_tex_coord_in[0].y), 1.0);\n"
//        "}";

void shaderAnimation(TCLControl *tcl) {
    int fbWidth = WIDTH;
    int fbHeight = HEIGHT;
    cogl_read_pixels(   0, // start x
                        50, // stary y
                        fbWidth,  // width (4 bytes per pixel)
                        fbHeight, // height in pixels
                        COGL_READ_PIXELS_COLOR_BUFFER,
                        COGL_PIXEL_FORMAT_RGBA_8888,
                        shaderBuffer);

//    printf("Here's the data we pulled from the FB:\n");
//    int pixelLength = 4; // 4 bytes per pixel in the FB (r,g,b,a)
//    for (int i = 0; i < (fbWidth*fbHeight)*pixelLength; i++) {
//        if (i%pixelLength == 0 && i != 0) {
//            printf(", ");
//        }
//        if (i%(fbWidth*pixelLength) == 0 && i != 0) {
//            printf("\n");
//        }
//
//        printf("%i ", shaderBuffer[i]); // Print out the value of this byte in the pixel
//    }
//    printf("\nDone!\n");

    // When we pull data from the on screen display, we will need to
    // know the full size of the on screen shader display so we know where
    // to grab the pixel colors from.
    //
    // Full screen size data is located in configurations.h
    // On screen display size is defined in main.cpp

    int ledIndex = 0; // This is the pixel offset in the actual color array. uint32_t
    int fbIndex = 0;  // This is the pixel BYTE offset in the
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            uint32_t thisColor = pack(shaderBuffer[fbIndex],shaderBuffer[fbIndex+1],shaderBuffer[fbIndex+2]);
            fbIndex += 4;

            tcl->pixelBuf[ledIndex] = thisColor;
            ledIndex += 1;
        }
    }

}


gboolean handleTouchEvents(ClutterActor *actor, ClutterEvent *event, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    TouchData *data;
    data = (TouchData *) user_data;

    // ClutterActor *lightDisplay = CLUTTER_ACTOR (data->lightDisplay);
    // TCLControl *tcl = data->tcl;
    // int *animation_number = data->animationNumber;
    // gfloat *input_x = data->input_x;
    // gfloat *input_y = data->input_y;

    ClutterEventType eventType = clutter_event_type(event);
    gfloat stage_x, stage_y;
    gfloat actor_x = 0, actor_y = 0;

    if (eventType == CLUTTER_TOUCH_END) {
        // printf("Touch end!\n");

    } else if (eventType == CLUTTER_TOUCH_UPDATE || eventType == CLUTTER_MOTION) {
        clutter_event_get_coords(event, &stage_x, &stage_y);
        clutter_actor_transform_stage_point(actor, stage_x, stage_y, &actor_x, &actor_y);

        // Now we have some x,y coordinates we can throw back at those animations!
        //  but we should probably scale them now since we have all the stuff we need
        //  to do so in this block...
        //
        // This will scale both to 0.0 <-> 255.0:
        actor_x = actor_x / clutter_actor_get_width(actor) * 255;
        actor_y = actor_y / clutter_actor_get_height(actor) * 255;

        // printf("Touch Move!!\nx: %f\ny: %f\n\n", actor_x, actor_y );
        input_x = static_cast<int>(actor_x);
        input_y = static_cast<int>(actor_y);

    } else {
        // printf("Some other touch event %i\n", eventType);
    }

    return CLUTTER_EVENT_STOP;
}


// handleNewFrame is the function that is called ever 120 milliseconds via the timeline!
// This is where ALL animation updates will happen.
void handleNewFrame(ClutterActor *timeline, gint frame_num, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    AnimationData *data;
    data = (AnimationData *) user_data;
    TCLControl *tcl = data->tcl; // tcl is STILL a pointer to the main TCLControl object

    // We hand in the ADDRESS of the current Animation:
    int *animation_number = data->animationNumber;

    // Run which ever animation we're on:
    switch (*animation_number) {
        case 1  :
            animation1(tcl);
            break;
        case 2  :
            animation2(tcl);
            break;
        case 3  :
            animation3(tcl);
            break;
        case 4  :
            animation4(tcl);
            break;
        case 5  :
            animation5(tcl);
            break;
        case 6  :
            animation6(tcl);
            break;
        case 7  :
            animation7(tcl);
            break;
        case 8  :
            animation8(tcl);
            break;
        case 9  :
            animation9(tcl);
            break;
        case 10  :
            animation10(tcl);
            break;

        default :
            shaderAnimation(tcl);
    }

//    shaderAnimation(tcl);

    // Send the updated color buffer to the strands
    if (tcl->enabled) {
        tcl->Update();
    }

    // Update the on screen color display using the colors FROM THE PRE-LIGHT ARRAY ITSELF!
    // Draw onscreen color map display:
    int index = 0;
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // This next line grabs the address of single pixel out of the pixels char buffer
            // and points a char at it so that it's value can be set:
            unsigned char *pixel = &pixels[y * rowstride + x * 3];

            TCpixel thisPixel = tcl->pixelBuf[index];
            // #define TCrgb(R,G,B) (((R) << 16) | ((G) << 8) | (B))
            pixel[0] = ((thisPixel) >> 16) & 0xff;//red
            pixel[1] = ((thisPixel) >> 8) & 0xff;//green
            pixel[2] = (thisPixel) & 0xff;//blue
            index += 1;
        }
    }
    if (pixbuf != NULL) {
        // THIS actually draws the image on the screen.
       clutter_image_set_data(CLUTTER_IMAGE(colors),
                           gdk_pixbuf_get_pixels (pixbuf),
                           COGL_PIXEL_FORMAT_RGB_888,
                           gdk_pixbuf_get_width (pixbuf),
                           gdk_pixbuf_get_height (pixbuf),
                           gdk_pixbuf_get_rowstride (pixbuf),
                           &error);
    }
    clutter_actor_set_content(lightDisplay, colors);


    // Update the shader uniforms:
    animationTime += 1.0;
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iGlobalTime", G_TYPE_FLOAT, 1,
                                      animationTime);
}


Animation::Animation(ClutterActor *stage, TCLControl *tcl, ClutterActor *infoDisplay) {
    printf("Building animation tools...\n");

    // First, we need some Actor to actually display the light colors:
    colors = clutter_image_new();
    lightDisplay = clutter_actor_new();
    error = NULL;

    // Load image data from some other data source...:
    // guchar *data =
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(data);

    // Load image data from a file:
    // const char *img_path = "./wut.png";
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size (img_path, WIDTH, HEIGHT, &error);

    // Load image data from nothing. Build it manually.
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, WIDTH, HEIGHT);
    // Then grab all of the pixels from that blank pixel buffer:
    pixels = gdk_pixbuf_get_pixels(pixbuf);
    // And figure out how many bytes wide that pixel buffer actually is.
    rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    // Allocate the memory for the shader output buffer:
    // Figure out how big our buffer needs to be. *3 because three bytes per pixel (r, g, b)
//    int shaderBufferSize = ceil(clutter_actor_get_width(stage)) * ceil(clutter_actor_get_height(stage)) * 3;
    int shaderBufferSize = ceil(WIDTH) * ceil(HEIGHT) * 4;
    // Actually malloc the buffer!!
    shaderBuffer = (guint8 *) malloc(shaderBufferSize);

    // Double check that we built the buffer correctly.
    if (shaderBuffer == NULL) {
        printf("OOPS! malloc error!\n");
    } else {
        printf("We malloc'd %i bytes for your shaderBuffer. It points at: %p\n", shaderBufferSize, shaderBuffer);
        printf(" The Rowstride on the shaderBuffer is %i\n", rowstride);
    }

    // Draw the color image:
    // Loop through the (blank) image we just made...
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // Find the address of each pixel in turn...
            // This next line grabs the address of single pixel out of the pixels char buffer
            // and points a char at it so that it's value can be set:
            unsigned char *pixel = &pixels[y * rowstride + x * 3];

            // And set that specific pixel's color to red.
            pixel[0] = 255;//red
            pixel[1] = 0x0;//green
            pixel[2] = 0x0;//blue
        }
    }

    // Assuming the buffer is defined, throw the image at the on screen display:
    if (pixbuf != NULL) {
        clutter_image_set_data(CLUTTER_IMAGE(colors),
                            gdk_pixbuf_get_pixels (pixbuf),
                            COGL_PIXEL_FORMAT_RGB_888,
                            gdk_pixbuf_get_width (pixbuf),
                            gdk_pixbuf_get_height (pixbuf),
                            gdk_pixbuf_get_rowstride (pixbuf),
                            &error);
    }

    // Set the on screen light display to the image that we hard coded to red earlier in this function..
    clutter_actor_set_content(lightDisplay, colors);



    //=============
    // NOW we can just IGNORE that "color" image/object that we built... because we're just going to blow
    // it all away with colors from a shader anyways!
    //
    // Setup the GLSL Fragment shaders that we'll use to generate colors
    // Build a GLSL Fragment shader to affect the color output (to the screen at least for now)
    shaderEffect = clutter_shader_effect_new(CLUTTER_FRAGMENT_SHADER);
    clutter_shader_effect_set_shader_source(CLUTTER_SHADER_EFFECT(shaderEffect), fragShader);

    // Bind uniforms to the shader so we can hand variables into them
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iGlobalTime", G_TYPE_FLOAT, 1, 0.0);
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iResolution", G_TYPE_FLOAT, 2, WIDTH*osd_scale, HEIGHT*osd_scale);
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iMouse", G_TYPE_FLOAT, 2, input_x, input_y);

    // Set the effect live on the on screen display actor...
//    clutter_actor_add_effect(lightDisplay, shaderEffect);



    // Resize the on screen color display/Shader output display Actor:
    clutter_actor_set_x_expand(lightDisplay, TRUE);
    clutter_actor_set_y_expand(lightDisplay, TRUE);
    clutter_actor_set_position(lightDisplay, 0, 0);
//    clutter_actor_set_size(lightDisplay, HEIGHT * osd_scale, WIDTH * osd_scale);
    clutter_actor_set_size(lightDisplay, WIDTH, HEIGHT);
    clutter_actor_set_scale(lightDisplay, osd_scale, osd_scale+8);
    clutter_actor_set_rotation_angle(lightDisplay, CLUTTER_Z_AXIS, -90);
    clutter_actor_set_rotation_angle(lightDisplay, CLUTTER_Y_AXIS, 180);

    // Actually add that actor to the stage!
    clutter_actor_add_child(stage, lightDisplay);

    // Allow for UI events on this crazy thing!
    clutter_actor_set_reactive(lightDisplay, TRUE);




    // Wire up the event listener on this lightDisplay actor. This is the data object layout:
    // typedef struct  {
    //     ClutterActor *lightDisplay;
    //     TCLControl *tcl;
    //     int *animationNumber;
    // } TouchData;
    //
    // Build a pointer to a struct in that format that we can pass through the g_signal_connect function
    // and into the event handler:
    TouchData *touch_data;
    touch_data = g_slice_new(TouchData); // reserve memory for it...
    touch_data->lightDisplay = lightDisplay; // Place the button actor itself inside the struct
    touch_data->tcl = tcl; // TCLControl *tcl is just a POINTER here (unlike in main.cpp)
    touch_data->animationNumber = &currentAnimation;

    // To get touch values back OUT of the callback, we will use variables in the global scope.
    // This is what was left over from my attempts at handing data back out... but, per usual,
    // I mess up my callback structures and forgot that calling things async = LOSING CONTEXT!!
    // input_x = 0;
    // input_y = 0;
    // touch_data->input_x = input_x;
    // touch_data->input_y = input_y;

    // Actually wire up the events and set up the data structs that the events need to operate:
    g_signal_connect(lightDisplay, "touch-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "button-press-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "motion-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "button-release-event", G_CALLBACK(handleTouchEvents), touch_data);


    // Once we have all that set up, we still need to START THE ACTUAL ANIMATION!!
    // To do that, we'll need to use the event chain/callback system we have been using so far. This
    // will take in some actor instances that the event handler can then update/modify in real time.
    //
    // Get ready to hand this display chunk in to the animation event:
    AnimationData *data;
    data = g_slice_new(AnimationData); // reserve memory for it...
//    data->rotatingActor = rect;
    data->infoDisplay = infoDisplay;
    data->tcl = tcl; // tcl is an pointer to the main TCLControl object.
    data->animationNumber = &currentAnimation;

    // The clutter timeline object takes a "duration" in milliseconds...
    timeline = clutter_timeline_new(120);
    g_signal_connect(timeline, "new-frame", G_CALLBACK(handleNewFrame), data);

    // which will just continue repeating:
    clutter_timeline_set_repeat_count(timeline, -1);

    // This actually starts the timeline animation!
    clutter_timeline_start(timeline);

}

Animation::~Animation() {
    g_object_unref(colors);
    g_object_unref(pixbuf);
    g_object_unref(shaderBuffer);
}

Animation::Animation() {
}

void Animation::switchAnimation(int animationNumber, ClutterActor *infoDisplay) {
    printf("Changing to animation: %i\n", animationNumber);
    currentAnimation = animationNumber;
}

int Animation::getCurrentAnimation() {
    printf("Called! Current animation is: %i\n", currentAnimation);
    return currentAnimation;
}

