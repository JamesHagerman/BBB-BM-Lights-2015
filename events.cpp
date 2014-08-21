#include "events.h"
#include <stdio.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>

Events::Events() {
	printf("Building events class\n");
}

Events::~Events() {

}

gboolean Events::handleButton1 (ClutterActor *actor,
    ClutterEvent *event,
    gpointer      user_data) {

    // First, we need to figure out if this was a touch or a click event:
    ClutterEventType eventType = clutter_event_type(event);

    // Then we need to copy the actor itself:
    ClutterActor *button1 = CLUTTER_ACTOR (user_data);

    // We'll need a place to store the color of this button:
    // ClutterColor button1Color;
    // clutter_actor_get_background_color(button1, &button1Color);
    // ClutterColor downColor = { 0, 0, 0, 128 };

    printf("Button 1 event: ");
    if (eventType == CLUTTER_TOUCH_BEGIN) {
        printf("Touch Begin...\n");
    } else if (eventType == CLUTTER_TOUCH_END) {
        printf("Touch End...\n");
    } else if (eventType == CLUTTER_BUTTON_PRESS) {
        printf("Mouse Down...\n");

        // clutter_actor_set_background_color (button1, &downColor);

        clutter_actor_set_rotation_angle(button1, CLUTTER_Z_AXIS, -15.0);
    } else if (eventType == CLUTTER_BUTTON_RELEASE) {
        printf("Mouse Up...\n");
        // clutter_actor_set_background_color (button1, &button1Color);

        clutter_actor_set_rotation_angle(button1, CLUTTER_Z_AXIS, 0.0);
    } else if (eventType == CLUTTER_LEAVE) {
        printf("Leave event......\n");
    } else {
        printf("Some other event %i\n", eventType);
    }

    // And now we need to figure out how to display that we've captured the event...

    
    return CLUTTER_EVENT_PROPAGATE;
}

gboolean Events::handleKeyPresses (ClutterActor *actor,
    ClutterEvent *event,
    gpointer      user_data) {

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