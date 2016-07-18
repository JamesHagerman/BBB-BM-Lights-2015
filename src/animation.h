#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <vector>
#include <string>
#include <algorithm>

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

    Animation(ClutterActor *stage, TCLControl *tcl); //TCLControl tcl
    ~Animation();

    // Event Handlers:
    static void handleNewFrame(ClutterTimeline *timeline, gint frame_num, gpointer user_data);
    static gboolean handleTouchEvents(ClutterActor *actor, ClutterEvent *event, gpointer user_data);

    // Timeline object itself:
    ClutterTimeline *timeline;
    TCLControl *tcl;

    // Old Animation stuff:
    unsigned int i;
    int currentAnimation;
    void switchAnimation(int animation_num);
    int getCurrentAnimation();


    // New Shader stuff:
    int currentSpeed;
    void setSpeed(int newSpeed);

    std::vector <std::string> shaderList;
//    int shaderCount;
    unsigned int currentShader;
    unsigned int currentOffset;
    bool shaderLoaded;

    void buildShaderList();
    void incrShaderIndex();
    void decrShaderIndex();

    void updateCurrentShader();
    void updateCurrentShader(int offset);
    void loadShader(const char *fragment_path);
    std::string readFile(const char *filePath);
    void unloadShader();

    void switchShader(int shaderNumber);

};

#endif












