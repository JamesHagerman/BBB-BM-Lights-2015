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

class Animation {
    public:
        Animation(ClutterActor *rotatingActor, TCLControl *tcl); //TCLControl tcl
        ~Animation();

        // Timeline object itself:
        ClutterTimeline *timeline;
        TCLControl *tcl;

        int i;

        ClutterActor *rect;

};












