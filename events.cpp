#include "events.h"
#include <stdio.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "configurations.h"

Events::Events() {
    printf("Building events class\n");
}

Events::~Events() {

}

gboolean Events::handleKeyPresses (ClutterActor *actor,
    ClutterEvent *event,
    gpointer      user_data) {


    // Rebuild the struct from the pointer we handed in:
    EventData *data;
    data = (EventData *)user_data;

    ClutterActor *label = CLUTTER_ACTOR (data->statusLabel);
    guint keyval = clutter_event_get_key_symbol (event);

    ClutterModifierType state = clutter_event_get_state (event);
    gboolean shift_pressed = (state & CLUTTER_SHIFT_MASK ? TRUE : FALSE);
    gboolean ctrl_pressed = (state & CLUTTER_CONTROL_MASK ? TRUE : FALSE);

    if (CLUTTER_KEY_Up == keyval) {
        if (shift_pressed & ctrl_pressed)
            printf ("Up and shift and control pressed\n");
        else if (shift_pressed)
            printf ("Up and shift pressed\n");
        else
            printf ("Up pressed\n");

        /* The event was handled, and the emission should stop */
        return CLUTTER_EVENT_STOP;
    } else if (CLUTTER_KEY_Left == keyval) {
        if (shift_pressed & ctrl_pressed)
            printf ("Left and shift and control pressed\n");
        else if (shift_pressed)
            printf ("Left and shift pressed\n");
        else
            printf ("Left pressed\n");

        /* The event was handled, and the emission should stop */
        return CLUTTER_EVENT_STOP;
    } else if (CLUTTER_KEY_Down == keyval) {
        if (shift_pressed & ctrl_pressed)
            printf ("Down and shift and control pressed\n");
        else if (shift_pressed)
            printf ("Down and shift pressed\n");
        else
            printf ("Down pressed\n");

        /* The event was handled, and the emission should stop */
        return CLUTTER_EVENT_STOP;
    } else if (CLUTTER_KEY_Right == keyval) {
        if (shift_pressed & ctrl_pressed)
            printf ("Right and shift and control pressed\n");
        else if (shift_pressed)
            printf ("Right and shift pressed\n");
        else
            printf ("Right pressed\n");

        /* The event was handled, and the emission should stop */
        return CLUTTER_EVENT_STOP;
    } else if (65293 == keyval) {
        printf("Looks like the enter key...\n");
        // ClutterActor *label;
        clutter_text_set_text(CLUTTER_TEXT(label), "Shutting down! PLEASE WAIT until orange LED stops blinking before killing power!");
        sleep(3);
        // clutter_main_quit();
        system("sync; shutdown -h now");
    } else if (65307 == keyval) {
        printf("esc pressed. Exiting...\n");
        exit(1);
    } else {
        printf("Something else pressed: %i\n", keyval);
    }

    /* The event was not handled, and the emission should continue */
    return CLUTTER_EVENT_PROPAGATE;

}


gboolean Events::handleTouchEvents (ClutterActor *actor,
    ClutterEvent *event,
    gpointer user_data) {

    ClutterEventType eventType = clutter_event_type(event);

    if (eventType == CLUTTER_TOUCH_END) {
        printf("Touch end!\n");
    } else {
        printf("Some other touch event %i\n", eventType);
    }

    return CLUTTER_EVENT_STOP;
}

gboolean Events::handleMouseEvents (ClutterActor *actor,
    ClutterEvent *event,
    gpointer      user_data) {


    ClutterActor *rect_actor = CLUTTER_ACTOR (user_data);

    /* get the coordinates where the pointer crossed into the actor */
    gfloat stage_x, stage_y;
    clutter_event_get_coords (event, &stage_x, &stage_y);

    /*
    * as the coordinates are relative to the stage, rather than
    * the actor which emitted the signal, it can be useful to
    * transform them to actor-relative coordinates
    */
    gfloat actor_x, actor_y;
    clutter_actor_transform_stage_point (actor,
                                       stage_x, stage_y,
                                       &actor_x, &actor_y);

    printf("pointer at stage x %.0f, y %.0f; actor x %.0f, y %.0f\n",
           stage_x, stage_y,
           actor_x, actor_y);

    clutter_actor_set_position (rect_actor, stage_x, stage_y);

    return CLUTTER_EVENT_STOP;
}