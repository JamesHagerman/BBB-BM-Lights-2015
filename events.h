#include <stdio.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "animation.h"

typedef struct  {
    ClutterActor *statusLabel;
} EventData;
typedef struct  {
    Animation *animation;
} EventDataAfterPaint;

class Events {
    public:
        Events();
        ~Events();
        static gboolean handleKeyPresses (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
        static gboolean handleTouchEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
        static gboolean handleMouseEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
        // ToDo: Uncomment shader stuff:
         static gboolean handleAfterPaint (ClutterActor *actor, ClutterEvent *event, gpointer user_data);

    private:

        
};