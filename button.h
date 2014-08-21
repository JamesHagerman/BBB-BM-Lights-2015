#include <stdio.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>

class Button {
	public:
		Button();
		Button(ClutterActor *stage, int width, int height, int x, int y, ClutterColor button1Color);
		~Button();
		static gboolean handleEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
		// static gboolean handleKeyPresses (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
		// static gboolean handleTouchEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);
		// static gboolean handleMouseEvents (ClutterActor *actor, ClutterEvent *event, gpointer user_data);

		ClutterActor *buttonActor;
		ClutterActor *stage;

		ClutterColor *buttonColor;
		ClutterColor *buttonDownColor;

		int buttonWidth, buttonHeight;
		int width, height;

		typedef struct  {
	        ClutterActor *actor;
	        ClutterColor upColor;
	        ClutterColor downColor;
	    } ButtonData;
	private:

		
};