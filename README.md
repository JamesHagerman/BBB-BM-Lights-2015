clutter_window
==============

Simple test of using Clutter to draw to a screen.

Three assets: green spinning rectangle, "Hello, World." text, and a tiny yellowish square.


Building on OS X
================

To build under OS X, you need to install clutter, cogl, and XQuartz.

Actually, we shouldn't need XQuarts because clutter and cogl just suck trying to connect to the x11 display. So we are just
going to use the --without-x11 flag for the install.

```
brew install clutter clutter-gst clutter-gtk pango cairo cogl --without-x11
```

You can use Homebrew to install clutter and cogl, but you'll need to use updated Formulas to make sure you pull down the latest versions.

I banged my head against this thing seg faulting a lot... so I uninstalled cogl, clutter, pango, cairo, and XQuartz, and reinstall them all.

I think it was an issue with me building pango before installing XQuartz... but I'm not sure.


Building on Ubuntu
==================

We need some packages installed under Ubuntu too. This does NOT build an ARM binary! No cross compiling here!

```
sudo apt-get install libclutter-1.0-dev libclutter-gst-2.0-dev libcogl-dev libpango1.0-dev libcairo2-dev
```



Building on Windows
===================

--clafgs:
```
-pthread -I/usr/include/clutter-1.0 -I/usr/include/pango-1.0 -I/usr/include/cogl -I/usr/include/cairo -I/usr/include/atk-1.0 -I/usr/include/json-glib-1.0 -I/usr/include/gtk-3.0 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libdrm -I/usr/include/libpng12 -I/usr/include/pixman-1 -I/usr/include/gio-unix-2.0/
```

--libs:
```
-pthread -lclutter-1.0 -latk-1.0 -lcogl-pango -ljson-glib-1.0 -lgdk-3 -lXi -lcogl -lgmodule-2.0 -lwayland-egl -lgbm -ldrm -lwayland-server -lEGL -lX11 -lXext -lXdamage -lXcomposite -lXrandr -lwayland-client -lgio-2.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lpango-1.0 -lcairo -lgobject-2.0 -lglib-2.0 -lXfixes
```

Installing the Clutter as a service on the BBB
===============================================

After building you can configure this clutter app to open on boot. Open the clutter_window.service file and set the path to the executable binary correctly for your machine. Once it's modified install that clutter_window.service file into:

/lib/systemd/system/

Once it's there, run these commands to get it installed:

systemctl enable clutter_window.service
systemctl start clutter_window.service
systemctl status clutter_window.service

That should enable the script when ever the BBB boots.