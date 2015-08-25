#include "button.h"
#include <stdio.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "configurations.h"
#include "AnimationHelpers.h"

Button::Button(ClutterActor *stage, int id, int width, int height, int x, int y, ClutterColor upColor, Animation *mainAnimations, int type) {

    uniqueId = id;
    animation = mainAnimations;

    buttonActor = clutter_actor_new();
    clutter_actor_set_background_color (buttonActor, &upColor);
    clutter_actor_set_size (buttonActor, width, height);
    clutter_actor_set_position (buttonActor, x, y);
    clutter_actor_set_pivot_point(buttonActor, 0.5, 0.5);
    clutter_actor_add_child(stage, buttonActor);
    clutter_actor_set_reactive (buttonActor, TRUE);

    // Set the buttons overlay image:
    GError *err = NULL;
    const char *speedImage = "images/btn_speed_selector.png";
    const char *colorSelectorImage = "images/btn_color_selector.png";
    GdkPixbuf *pixbuf;
    ClutterContent *image = clutter_image_new ();

    if (type == 1) {
        pixbuf = gdk_pixbuf_new_from_file (speedImage, NULL);
    } else if (type == 2) {
        pixbuf = gdk_pixbuf_new_from_file (colorSelectorImage, NULL);
    }

    if (type != 0) {
        clutter_image_set_data (CLUTTER_IMAGE (image),
                                gdk_pixbuf_get_pixels (pixbuf),
                                gdk_pixbuf_get_has_alpha (pixbuf)
                                ? COGL_PIXEL_FORMAT_RGBA_8888
                                : COGL_PIXEL_FORMAT_RGB_888,
                                gdk_pixbuf_get_width (pixbuf),
                                gdk_pixbuf_get_height (pixbuf),
                                gdk_pixbuf_get_rowstride (pixbuf),
                                &err);
        g_object_unref (pixbuf);
        clutter_actor_set_content  (buttonActor, image);
    }

    if (type == 1) {
        //pixbuf = gdk_pixbuf_new_from_file (speedImage, NULL);
    } else if (type == 2) {
        pixbuf = gdk_pixbuf_new_from_file (colorSelectorImage, NULL);
    }

    if (type != 0) {
        clutter_actor_set_content  (buttonActor, image);
    }


    // Build a pointer to a struct that we can pass through the g_signal_connect function:
    ButtonData *data;
    data = g_slice_new (ButtonData); // reserve memory for it...
    data->actor = buttonActor; // Place the button actor itself inside the struct
    // Build a "down" color (hard coded for now...)
    ClutterColor downColor = { 255, 0, 47, 0xFF };
    data->upColor = upColor;
    data->downColor = downColor;
    data->uniqueId = id;
    data->animationObject = animation;
    data->type = type;

    // Wire up the callbacks:
    g_signal_connect(buttonActor, "touch-event", G_CALLBACK (handleEvents), data);
    g_signal_connect(buttonActor, "button-press-event", G_CALLBACK (handleEvents), data);
    g_signal_connect(buttonActor, "motion-event", G_CALLBACK(handleEvents), data);
    g_signal_connect(buttonActor, "button-release-event", G_CALLBACK (handleEvents), data);
}

Button::~Button() {

}

void setSpeed(int speed, Animation *animation) {
    animation->setSpeed(speed);
}

void changeAnimation(int id, Animation *animation) {
    switch (id) {
        case 0  :
//            animation->switchAnimation(1);
            animation->loadShader("./shaders/basic.frag");
            break;
        case 1  :
//            animation->switchAnimation(2);
            animation->loadShader("./shaders/led_test.frag");
            break;
        case 2  :
//            animation->switchAnimation(3);
            animation->loadShader("./shaders/timer.frag");
            break;
        case 3  :
            // animation->switchAnimation(4);
            animation->loadShader("./shaders/jason01.frag");
            break;
        case 4  :
            // animation->switchAnimation(5);
            animation->loadShader("./shaders/james01.frag");
            break;

        case 5  :
            // animation->switchAnimation(6);
            animation->loadShader("./shaders/audio.frag");
            break;
        case 6  :
            animation->switchAnimation(7);
            break;
        case 7  :
            animation->switchAnimation(8);
            break;
        case 8  :
            animation->switchAnimation(9);
            break;
        case 9  :
            animation->switchAnimation(10);
            break;

        default :
            animation->switchAnimation(6);
    }

    // animation->getCurrentAnimation();
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
    int id = data->uniqueId;
    Animation *animation = data->animationObject;
    int type = data->type;
    // clutter_actor_get_background_color(button1, &button1Color);
    // ClutterColor downColor = { 0, 0, 0, 128 };

    if (type == 0) {
        // printf("Button event: ");
        if (eventType == CLUTTER_TOUCH_BEGIN) {
            // printf("Touch Begin...\n");
            clutter_actor_set_background_color (button, &downColor);
            clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, -15.0);

            changeAnimation(id, animation);

            clutter_actor_set_background_color (button, &upColor);
            clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, 0.0);

        } else if (eventType == CLUTTER_TOUCH_END || eventType == CLUTTER_TOUCH_CANCEL) {
            // printf("Touch End...\n");
            clutter_actor_set_background_color (button, &upColor);
            clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, 0.0);

        } else if (eventType == CLUTTER_BUTTON_PRESS) {
            // printf("Mouse Down...\n");
            clutter_actor_set_background_color (button, &downColor);
            clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, -15.0);

        } else if (eventType == CLUTTER_BUTTON_RELEASE) {
            // printf("Mouse Up...\n");
            clutter_actor_set_background_color(button, &upColor);
            clutter_actor_set_rotation_angle(button, CLUTTER_Z_AXIS, 0.0);
            // On bluebutton presses...
            changeAnimation(id, animation);
        } else if (eventType == CLUTTER_LEAVE) {
            // printf("Leave event......\n");
        } else {
            // printf("Some other event %i\n", eventType);
        }
    } else if (type == 1) { // handle speed control "button"
        if (eventType == CLUTTER_TOUCH_UPDATE || eventType == CLUTTER_MOTION) {
            gfloat stage_x, stage_y;
            gfloat actor_x = 0, actor_y = 0;

            clutter_event_get_coords(event, &stage_x, &stage_y);
            clutter_actor_transform_stage_point(actor, stage_x, stage_y, &actor_x, &actor_y);

            temp_x = static_cast<int>(actor_x);
//            temp_y = static_cast<int>(actor_y);

            int buttonWidth = clutter_actor_get_width(actor);
            temp_x = map(temp_x, 0, buttonWidth, -500, 500);
//            temp_y = map(temp_y, 0, HEIGHT*osd_scale, 0-50, HEIGHT*osd_scale+(HEIGHT*osd_scale-750));
//            printf("Speed touch: %d\n", temp_x);
            setSpeed(temp_x, animation);
        }
    }



    return CLUTTER_EVENT_PROPAGATE;
}
