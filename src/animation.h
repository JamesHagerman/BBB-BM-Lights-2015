#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <string>

#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "p9813.h"
#include "TCLControl.h"

#ifndef TCL_ANIMATION
#define TCL_ANIMATION

class Animation {
public:
    Animation();

    Animation(ClutterActor *stage, TCLControl *tcl,
              ClutterActor *infoDisplay); //TCLControl tcl
    ~Animation();

    // Event Handlers:
    static void handleNewFrame(ClutterActor *timeline, gint frame_num, gpointer user_data);
    static gboolean handleTouchEvents(ClutterActor *actor, ClutterEvent *event, gpointer user_data);

    // Timeline object itself:
    ClutterTimeline *timeline;
    TCLControl *tcl;

    // Old Animation stuff:
    int i;
    int currentAnimation;

    void switchAnimation(int animation_num, ClutterActor *infoDisplay);
    int getCurrentAnimation();


    // New Shader stuff:
    int currentShader;
    std::string readFile(const char *filePath);
    void loadShader();
    void unloadShader();
    void switchShader(int shaderNumber);
    int getCurrentShader();
};

#endif












