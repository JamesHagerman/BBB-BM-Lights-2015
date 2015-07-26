#include <stdio.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "animation.h"

class Button {
    public:
        Button();
        Button(ClutterActor *stage, int id, int width, int height, int x, int y, ClutterColor button1Color, Animation *mainAnimations, ClutterActor *infoDisplay);
        ~Button();
        // void changeAnimation(int id);
        static gboolean handleEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
        ClutterActor *buttonActor;
        ClutterActor *stage;

        ClutterColor *buttonColor;
        ClutterColor *buttonDownColor;

        Animation *animation;

        int buttonWidth, buttonHeight;
        int width, height;
        int uniqueId;

        typedef struct  {
            ClutterActor *actor;
            ClutterActor *infoDisplay;
            ClutterColor upColor;
            ClutterColor downColor;
            int uniqueId;
            Animation *animationObject;
        } ButtonData;
    private:

};