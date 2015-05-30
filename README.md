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


Installing the Clutter as a service on the BBB
===============================================

After building you can configure this clutter app to open on boot. Open the clutter_window.service file and set the path to the executable binary correctly for your machine. Once it's modified install that clutter_window.service file into:

/lib/systemd/system/

Once it's there, run these commands to get it installed:

systemctl enable clutter_window.service
systemctl start clutter_window.service
systemctl status clutter_window.service

That should enable the script when ever the BBB boots.