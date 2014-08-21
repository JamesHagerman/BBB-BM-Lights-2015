#include <stdio.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>

class Events {
	public:
		Events();
		~Events();
		static gboolean handleTapEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
		static gboolean handleKeyPresses (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
		static gboolean handleTouchEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
		static gboolean handleMouseEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
	private:

		
};