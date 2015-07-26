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

#ifndef TCL_ANIMATION
#define TCL_ANIMATION

class Animation {
public:
    Animation();

    Animation(ClutterActor *stage, TCLControl *tcl,
              ClutterActor *infoDisplay); //TCLControl tcl
    ~Animation();

    void switchAnimation(int animation_num, ClutterActor *infoDisplay);
    int getCurrentAnimation();

    // Timeline object itself:
    ClutterTimeline *timeline;
    TCLControl *tcl;

    int i;
    int currentAnimation;

    ClutterActor *rect;
};

#endif












