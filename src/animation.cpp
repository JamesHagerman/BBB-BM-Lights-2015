#include "animation.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
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
    TCLControl *tcl;
    int *animationNumber;
    Animation *animationObject;
} AnimationData;

typedef struct {
    ClutterActor *lightDisplay;
    ClutterEffect *shaderEffect;
    TCLControl *tcl;
    int *animationNumber;
    gfloat *input_x;
    gfloat *input_y;
} TouchData;

// On screen display stuff:
ClutterActor *lightDisplay; // The actual object we'll draw the colors into
ClutterContent *colors; // The delegate for painting the lightDisplay
GdkPixbuf *pixbuf; // The color buffer for the colors content delegate
unsigned char *pixels; // A list of pointers to the pixels in the color buffer
int rowstride; // Size of the color buffer

// Error object for GDK/Clutter calls that need them
GError *error;




// GLSL Shader stuff:
ClutterActor *shaderOutput;
ClutterEffect *shaderEffect;
guint8 *shaderBuffer;
gfloat animationTime = 0.0; // A variable to hold the value of iGlobalTime

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

gboolean Animation::handleTouchEvents(ClutterActor *actor, ClutterEvent *event, gpointer user_data) {

    ClutterEventType eventType = clutter_event_type(event);

//    if (eventType == CLUTTER_TOUCH_END) {
//        // printf("Touch end!\n");
//
//    } else

    if (eventType == CLUTTER_TOUCH_UPDATE || eventType == CLUTTER_MOTION) {

        // Rebuild the struct from the pointer we handed in:
//        TouchData *data;
//        data = (TouchData *) user_data;
        gfloat stage_x, stage_y;
        gfloat actor_x = 0, actor_y = 0;

        clutter_event_get_coords(event, &stage_x, &stage_y);
        clutter_actor_transform_stage_point(actor, stage_x, stage_y, &actor_x, &actor_y);

        // Now we have some x,y coordinates we can throw back at those animations!
        //  but we should probably scale them now since we have all the stuff we need
        //  to do so in this block...
        //
        // This will scale both to 0.0 <-> 255.0:
        //actor_x = actor_x / clutter_actor_get_width(actor) * 255;
        //actor_y = actor_y / clutter_actor_get_height(actor) * 255;

        // This will scale both to the actual on screen pixel size:
        actor_x = actor_x * osd_scale;
        actor_y = actor_y * osd_scale;

        // This will invert the x axis. We flipped our stupid actor around...
        //actor_x = WIDTH*osd_scale-actor_x;

//        printf("Touch Move!!\nx: %f\ny: %f\n\n", actor_x, actor_y );
        input_x = static_cast<int>(actor_x);
        input_y = static_cast<int>(actor_y);
//        printf("Touch Move!!\nx: %i\ny: %i\n\n", input_x, input_y );
    }
//    else {
//        // printf("Some other touch event %i\n", eventType);
//    }

    return CLUTTER_EVENT_STOP;
}

void shaderAnimation(TCLControl *tcl) {
    int fbWidth = HEIGHT;
    int fbHeight = WIDTH;
    cogl_read_pixels(   0, // start x
                        0, // stary y
                        fbWidth,  // width
                        fbHeight, // height
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
    int fbIndex = 0;  // This is the pixel BYTE offset in the shaderBuffer
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // Find the ADDRESS of each pixel in the pixbuf via the raw char buffer we built...
            // and bind it to a pointer to a char...
            unsigned char *pixel = &pixels[y * rowstride + x * 3];

            // And directly update that memory location with a new color
            // This AUTOMATICALLY updates the color of the pixbuf!
            // It's just hitting the memory directly!
            pixel[0] = shaderBuffer[fbIndex];//red
            pixel[1] = shaderBuffer[fbIndex+1];//green
            pixel[2] = shaderBuffer[fbIndex+2];//blue

            // Then put colors to the lights themselves:
            uint32_t thisColor = pack(shaderBuffer[fbIndex],shaderBuffer[fbIndex+1],shaderBuffer[fbIndex+2]);
            fbIndex += 4;

            tcl->pixelBuf[ledIndex] = thisColor;
            ledIndex += 1;
        }
    }

}





// handleNewFrame is the function that is called ever 120 milliseconds via the timeline!
// This is where ALL animation updates will happen.
void Animation::handleNewFrame(ClutterTimeline *timeline, gint frame_num, gpointer user_data) {

    // Rebuild the struct from the pointer we handed in:
    AnimationData *data;
    data = (AnimationData *) user_data;
    TCLControl *tcl = data->tcl; // tcl is STILL a pointer to the main TCLControl object
    Animation *animation = data->animationObject;

    // Error object for GDK/Clutter calls that need them
    error = NULL;

    // ToDo: Rework old animation system:
//    // We hand in the ADDRESS of the current Animation:
//    int *animation_number = data->animationNumber;
//    // Run which ever animation we're on:
//    switch (*animation_number) {
//        case 1  :
//            animation1(tcl);
//            break;
//        case 2  :
//            animation2(tcl);
//            break;
//        case 3  :
//            animation3(tcl);
//            break;
//        case 4  :
//            animation4(tcl);
//            break;
//        case 5  :
//            animation5(tcl);
//            break;
//        case 6  :
//            animation6(tcl);
//            break;
//        case 7  :
//            animation7(tcl);
//            break;
//        case 8  :
//            animation8(tcl);
//            break;
//        case 9  :
//            animation9(tcl);
//            break;
//        case 10  :
//            animation10(tcl);
//            break;
//
//        default :
//            shaderAnimation(tcl);
//    }

    shaderAnimation(tcl);

    // Send the updated color buffer to the strands
    if (tcl->enabled) {
        tcl->Update();
    }

    // Old animation loop stuff:
//    // Update the on screen color display using the colors FROM THE PRE-LIGHT ARRAY ITSELF!
//    // Draw onscreen color map display:
//    int index = 0;
//    for (int x = 0; x < WIDTH; x++) {
//        for (int y = 0; y < HEIGHT; y++) {
//
//            // This next line grabs the address of single pixel out of the pixels char buffer
//            // and points a char at it so that it's value can be set:
//            unsigned char *pixel = &pixels[y * rowstride + x * 3];
//
//            TCpixel thisPixel = tcl->pixelBuf[index];
//            // #define TCrgb(R,G,B) (((R) << 16) | ((G) << 8) | (B))
//            pixel[0] = ((thisPixel) >> 16) & 0xff;//red
//            pixel[1] = ((thisPixel) >> 8) & 0xff;//green
//            pixel[2] = (thisPixel) & 0xff;//blue
//            index += 1;
//        }
//    }


    // Dump the colors from the color buffer into the ClutterContent delegate!
    if (pixbuf != NULL) {
        clutter_image_set_data(CLUTTER_IMAGE(colors),
                               gdk_pixbuf_get_pixels (pixbuf),
                               COGL_PIXEL_FORMAT_RGB_888,
                               gdk_pixbuf_get_width (pixbuf),
                               gdk_pixbuf_get_height (pixbuf),
                               gdk_pixbuf_get_rowstride (pixbuf),
                               &error);
    }
//    clutter_actor_set_content(lightDisplay, colors); // Bind that delegate to the lightDisplay


    // Update the shader uniforms:

    // Use the timeline delta to determine how much time to add to the clock:
    int delta = clutter_timeline_get_delta(timeline);
//    printf("Delta: %f\n", delta/1000.0);
    animationTime += delta/1000.0;

    if (animation->currentShader>0) {
        clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iGlobalTime", G_TYPE_FLOAT, 1,
                                          animationTime);
        clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iMouse", G_TYPE_FLOAT, 2, input_y*1.0, input_x*1.0);
    }

}


Animation::Animation(ClutterActor *stage, TCLControl *tcl) {
    printf("Building animation tools...\n");

    //====================
    // On screen display:

    // Build the Actor that will display what's going to the lights:
    lightDisplay = clutter_actor_new();

    // Build the color delegate for the lightDisplay's content/texture:
    colors = clutter_image_new();

    // Error object for GDK/Clutter calls that need them
    error = NULL;

    // Load image data from some other data source...:
    // guchar *data =
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(data);

    // Load image data from a file:
    // const char *img_path = "./wut.png";
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size (img_path, WIDTH, HEIGHT, &error);

    // Build the color buffer we'll store the actual colors in:
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, WIDTH, HEIGHT);
    pixels = gdk_pixbuf_get_pixels(pixbuf); // Grab it's pixels...
    rowstride = gdk_pixbuf_get_rowstride(pixbuf); // figure out the width of the buffer...

    // And put some RED into the buffer to start off with
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {

            // Find the ADDRESS of each pixel in the pixbuf via the raw char buffer we built...
            // and bind it to a pointer to a char...
            unsigned char *pixel = &pixels[y * rowstride + x * 3];

            // And directly update that memory location with a new color
            // This AUTOMATICALLY updates the color of the pixbuf!
            // It's just hitting the memory directly!
            pixel[0] = 255;//red
            pixel[1] = 0x0;//green
            pixel[2] = 0x0;//blue
        }
    }

    // Dump the colors from the color buffer into the ClutterContent delegate!
    if (pixbuf != NULL) {
        clutter_image_set_data(CLUTTER_IMAGE(colors),
                            gdk_pixbuf_get_pixels (pixbuf),
                            COGL_PIXEL_FORMAT_RGB_888,
                            gdk_pixbuf_get_width (pixbuf),
                            gdk_pixbuf_get_height (pixbuf),
                            gdk_pixbuf_get_rowstride (pixbuf),
                            &error);
    }

    // Tell the lightDisplay that it's content should be whatever we've dumped in the content delegate:
    clutter_actor_set_content(lightDisplay, colors);

    // Define the size of the On Screen Display:
    clutter_actor_set_x_expand(lightDisplay, TRUE);
    clutter_actor_set_y_expand(lightDisplay, TRUE);
    clutter_actor_set_position(lightDisplay, 0, WIDTH*(osd_scale+7)+WIDTH);
    clutter_actor_set_size(lightDisplay, WIDTH, HEIGHT);
    clutter_actor_set_scale(lightDisplay, osd_scale, osd_scale+7);
    clutter_actor_set_rotation_angle(lightDisplay, CLUTTER_Z_AXIS, -90);
//    clutter_actor_set_rotation_angle(lightDisplay, CLUTTER_Y_AXIS, 180);

    // Actually add that actor to the stage!
    clutter_actor_add_child(stage, lightDisplay);

    // Allow for UI events on this crazy thing!
    clutter_actor_set_reactive(lightDisplay, TRUE);

    // Wire up the event listener on this lightDisplay actor.
    TouchData *touch_data;
    touch_data = g_slice_new(TouchData); // reserve memory for it...
    touch_data->lightDisplay = lightDisplay; // Place the button actor itself inside the struct
    touch_data->tcl = tcl; // TCLControl *tcl is just a POINTER here (unlike in main.cpp)
    touch_data->animationNumber = &currentAnimation;

    // Actually wire up the events and set up the data structs that the events need to operate:
    g_signal_connect(lightDisplay, "touch-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "button-press-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "motion-event", G_CALLBACK(handleTouchEvents), touch_data);
    g_signal_connect(lightDisplay, "button-release-event", G_CALLBACK(handleTouchEvents), touch_data);


    // End On screen display
    //=============================
    // Shader setup and wirings:

    // Build the Actor that the shader will dump to directly:
    shaderOutput = clutter_actor_new();
    clutter_actor_set_position(shaderOutput, 0, 0);
    clutter_actor_set_size(shaderOutput, WIDTH, HEIGHT);
    clutter_actor_set_rotation_angle(shaderOutput, CLUTTER_Z_AXIS, -90);
    clutter_actor_set_rotation_angle(shaderOutput, CLUTTER_Y_AXIS, 180);
    clutter_actor_add_child(stage, shaderOutput);

    // Allocate the memory for the shader output buffer:
    // Figure out how big our buffer needs to be. *4 because four bytes per pixel (r, g, b, a) in shader land
    int shaderBufferSize = WIDTH * HEIGHT * 4;
    shaderBuffer = (guint8 *) malloc(shaderBufferSize); // malloc the buffer!!

    // Double check that we built the buffer correctly.
    if (shaderBuffer == NULL) {
        printf("OOPS! malloc error!\n");
    } else {
        printf("We malloc'd %i bytes for your shaderBuffer. It points at: %p\n", shaderBufferSize, shaderBuffer);
        printf(" The Rowstride on the shaderBuffer is %i\n", rowstride);
    }

    // Make sure we don't have a current shader so we don't break the update loop:
    currentShader = -1;




    // Once we have all that set up, we still need to START THE ACTUAL ANIMATION!!
    // To do that, we'll need to use the event chain/callback system we have been using so far.
    // Get ready to hand this display chunk in to the animation event:
    AnimationData *data;
    data = g_slice_new(AnimationData); // reserve memory for it...
    data->tcl = tcl; // tcl is an pointer to the main TCLControl object.
    data->animationNumber = &currentAnimation;
    data->animationObject = this;

    // The clutter timeline object takes a "duration" in milliseconds...
    timeline = clutter_timeline_new(1);
    g_signal_connect(timeline, "new-frame", G_CALLBACK(handleNewFrame), data);

    // which will just continue repeating:
    clutter_timeline_set_repeat_count(timeline, -1);

    // This actually starts the timeline animation!
    clutter_timeline_start(timeline);

    // Load the first shader so we do not get a black screen:
    loadShader("./shaders/basic.frag");

}

Animation::~Animation() {
    g_object_unref(colors);
    g_object_unref(pixbuf);
    g_object_unref(shaderBuffer);
}

Animation::Animation() {
}

// Old Animation stuff:
void Animation::switchAnimation(int animationNumber) {
    printf("Changing to animation: %i\n", animationNumber);
    currentAnimation = animationNumber;
}

int Animation::getCurrentAnimation() {
    printf("Called! Current animation is: %i\n", currentAnimation);
    return currentAnimation;
}


// New Shader Stuff:
std::string Animation::readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Could not read file %s. File does not exist.\n", filePath);
    }

    content.append(fragShaderPreamble);

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    content.append(fragShaderPostamble);

    fileStream.close();
    return content;
}

void Animation::loadShader(const char *fragment_path) {

    std::string fragShaderStr = readFile(fragment_path);
    const gchar *fragShaderSrc = fragShaderStr.c_str();

    // Pop off the old shader:
    unloadShader();

    printf("Loading shader: '%s' \n", fragment_path);

    // Setup the GLSL Fragment shaders that we'll use to generate colors
    // Build a GLSL Fragment shader to affect the color output (to the screen at least for now)
    shaderEffect = clutter_shader_effect_new(CLUTTER_FRAGMENT_SHADER);
    clutter_shader_effect_set_shader_source(CLUTTER_SHADER_EFFECT(shaderEffect), fragShaderSrc);

    // Bind uniforms to the shader so we can hand variables into them
    animationTime = 0.0;
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iGlobalTime", G_TYPE_FLOAT, 1, animationTime);
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iResolution", G_TYPE_FLOAT, 2, HEIGHT*osd_scale, WIDTH*osd_scale);
    clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shaderEffect), "iMouse", G_TYPE_FLOAT, 2, input_x*osd_scale, input_y*osd_scale);

    // Set the effect live on the on screen display actor...
    clutter_actor_add_effect(shaderOutput, shaderEffect);
    currentShader = 1;
}

void Animation::unloadShader() {
    printf("Unloading previous shader...\n");
    // Pop off the old shader:
    if (currentShader > 0) {
        currentShader = -1;
        clutter_actor_remove_effect(shaderOutput, shaderEffect);
    }
}

void Animation::switchShader(int shaderNumber) {
    currentShader = shaderNumber;
    animationTime = 0.0;
}

int Animation::getCurrentShader() {
    printf("Current shader is: %i\n", currentShader);
    return currentShader;
}



