#include "button.h"
#include <stdio.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>

Button::Button(ClutterActor *stage, int width, int height, int x, int y, ClutterColor upColor) {
	printf("Building events class\n");

    buttonActor = clutter_actor_new();
    clutter_actor_set_background_color (buttonActor, &upColor);
    clutter_actor_set_size (buttonActor, width, height);
    clutter_actor_set_position (buttonActor, x, y);
    clutter_actor_set_pivot_point(buttonActor, 0.5, 0.5);
    clutter_actor_add_child(stage, buttonActor);
    clutter_actor_set_reactive (buttonActor, TRUE);

    // Build a pointer to a struct that we can pass through the g_signal_connect function:
    ButtonData *data;
    data = g_slice_new (ButtonData); // reserve memory for it...
    data->actor = buttonActor; // Place the button actor itself inside the struct

    // Build a "down" color (hard coded for now...)
    ClutterColor downColor = { 255, 0, 47, 0xFF };
    data->upColor = upColor;
    data->downColor = downColor;

    // struct buttonData *p;
    // p = g_new0 (buttonData, 1);
    // p->actor = buttonActor;


    // struct buttonData data;
    // struct my_struct *data = malloc(sizeof(*data));
    // data->actor = buttonActor;

    g_signal_connect(buttonActor, "touch-event", G_CALLBACK (handleEvents), data);
    g_signal_connect(buttonActor, "button-press-event", G_CALLBACK (handleEvents), data);
    g_signal_connect(buttonActor, "button-release-event", G_CALLBACK (Button::handleEvents), data);
}

Button::~Button() {

}

gboolean Button::handleEvents (ClutterActor *actor,
    ClutterEvent *event,
    gpointer      user_data) {

    // First, we need to figure out if this was a touch or a click event:
    ClutterEventType eventType = clutter_event_type(event);

    // Rebuild the struct from the pointer we handed in:
    ButtonData *data;
    data = (ButtonData *)user_data;

    // And yank the values we need out of the rebuilt struct:
    ClutterActor *button = CLUTTER_ACTOR (data->actor);
    ClutterColor upColor = data->upColor;
    ClutterColor downColor = data->downColor;
    // clutter_actor_get_background_color(button1, &button1Color);
    // ClutterColor downColor = { 0, 0, 0, 128 };

    printf("Button event: ");
    if (eventType == CLUTTER_TOUCH_BEGIN) {
        printf("Touch Begin...\n");
        clutter_actor_set_background_color (button, &downColor);
        clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, -15.0);

    } else if (eventType == CLUTTER_TOUCH_END) {
        printf("Touch End...\n");
        clutter_actor_set_background_color (button, &upColor);
        clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, 0.0);

    } else if (eventType == CLUTTER_BUTTON_PRESS) {
        printf("Mouse Down...\n");
        clutter_actor_set_background_color (button, &downColor);
        clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, -15.0);

    } else if (eventType == CLUTTER_BUTTON_RELEASE) {
        printf("Mouse Up...\n");
        clutter_actor_set_background_color (button, &upColor);
        clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, 0.0);

    } else if (eventType == CLUTTER_LEAVE) {
        printf("Leave event......\n");
    } else {
        printf("Some other event %i\n", eventType);
    }

    // And now we need to figure out how to display that we've captured the event...

    
    return CLUTTER_EVENT_PROPAGATE;
}



// gboolean Events::handleTouchEvents (ClutterActor *actor,
//     ClutterEvent *event,
//     gpointer user_data) {

//     ClutterEventType eventType = clutter_event_type(event);

//     if (eventType == CLUTTER_TOUCH_END) {
//         printf("Touch end!\n");
//     } else {
//         printf("Some other touch event %i\n", eventType);
//     }

//     return CLUTTER_EVENT_STOP;
// }

// gboolean Events::handleMouseEvents (ClutterActor *actor,
//     ClutterEvent *event,
//     gpointer      user_data) {


//     ClutterActor *rect_actor = CLUTTER_ACTOR (user_data);

//     /* get the coordinates where the pointer crossed into the actor */
//     gfloat stage_x, stage_y;
//     clutter_event_get_coords (event, &stage_x, &stage_y);

//     /*
//     * as the coordinates are relative to the stage, rather than
//     * the actor which emitted the signal, it can be useful to
//     * transform them to actor-relative coordinates
//     */
//     gfloat actor_x, actor_y;
//     clutter_actor_transform_stage_point (actor,
//                                        stage_x, stage_y,
//                                        &actor_x, &actor_y);

//     printf("pointer at stage x %.0f, y %.0f; actor x %.0f, y %.0f\n",
//            stage_x, stage_y,
//            actor_x, actor_y);

//     clutter_actor_set_position (rect_actor, stage_x, stage_y);

//     return CLUTTER_EVENT_STOP;
// }