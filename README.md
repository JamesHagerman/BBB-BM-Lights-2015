clutter_window
==============

Simple test of using Clutter to draw to a screen.

Three assets: green spinning rectangle, "Hello, World." text, and a tiny yellowish square.


Building on OS X
================

To build under OS X, you need to install clutter, cogl, and XQuartz.

You can use Homebrew to install clutter and cogl, but you'll need to use updated Formulas to make sure you pull down the latest versions.

I banged my head against this thing seg faulting a lot... so I uninstalled cogl, clutter, pango, cairo, and XQuartz, and reinstall them all.

I think it was an issue with me building pango before installing XQuartz... but I'm not sure.