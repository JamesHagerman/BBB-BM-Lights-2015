clutter_window
==============

Simple test of using Clutter to draw to a screen.

Three assets: green spinning rectangle, "Hello, World." text, and a tiny yellowish square.


Things to do
============
- LOAD SHADERS FROM FILES!
    - Clean up animation switching
    - Figure out how to swap shader programs!
    - Build a UI state machine
- AUDIO INPUT!
    - Alsa mixer level maybe set via command line? or maybe a config file?
    - Onscreen volume control so we can control source input attenuation
- Interface updates!


- Remove the stupid text and all calls trying to put crap on the screen
- Remove the status square and text at the top of the screen
- Build a Debug view of some kind.
- MAIN.CPP LOOKS HALF DECENT!
- SERIOUSLY clean up animation.cpp!
    - Pull all of the helper methods out into a helper class
    - Pull all of the animations themselves out into animation classes.
    - Build an Animation Manager class that will do most of what Animation is doing currently.
    - Figure out how to swap out shaders/Get shaders stored in individual Animations (new animation objects)
- Clean up the constructors and object instantiation so we do not have variable hidding happening all over the place
- Rename the objects so they make sense.
- Update the events handlers so they are all defined in the same area of main.cpp
- Make the "features" into command line switches where appropriate.
- abstract UI layout so that the UI can be swapped out more easily (VIEWS!)
- Abstract the UI elements themselves so they are easier to use
- Perhaps write a shim to handle the difference between touches, movements, and mouse events.


Future Features
===============

- WIFI INTERFACING!! We should seriously get this to act as an AP somehow. Or just bring along a tiny WiFi AP and give
  the BBB a static IP. All other wifi controllers could attach to that AP and offload a bunch of bullshit like the 
  possible antenna issues and such. They'll be better with the power issues anyways.
  
- dump1090 integration! Not a big deal but might be kinda fun somehow...

- Multiple touch zones in the main display object??
    - Not just X-Y, but multiple X-Y fields to play with.
    - Can this be done easily with a drop in GLSL function? If so, it would allow us to build these shaders in Shader Toy more easily...


Bugs
====

- Memory issue on BBB
    After running for a few seconds:
    (clutter_window:2082): Cogl-WARNING **: /home/elias/workspace/setup-scripts/build/tmp-angstrom_v2014_06-eglibc/work/armv7at2hf-vfp-neon-angstrom-linux-gnueabi/cogl-1.0/1.18.2-r0/cogl-1.18.2/cogl/driver/gl/cogl-framebuffer-gl.c:1135: GL error (1285): Out of memory
    
    Maybe GDB can give some insight here. I thought I fixed this issue once before...
    It does NOT have anything to do with the events. I disabled all of those and it still happened.
    So it's SOMETHING to do with the Animation class itself.
    
    SOLVED: It was just running out of memory. There is just enough memory to draw to the screen once.
    We were drawing two large status areas to the screen and that was overflowing as far as I can tell...



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

Just doesn't work. Cygwin is a pain in the butt because of a bunch of reasons...

--clafgs:
```
-pthread -I/usr/include/clutter-1.0 -I/usr/include/pango-1.0 -I/usr/include/cogl -I/usr/include/cairo -I/usr/include/atk-1.0 -I/usr/include/json-glib-1.0 -I/usr/include/gtk-3.0 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libdrm -I/usr/include/libpng12 -I/usr/include/pixman-1 -I/usr/include/gio-unix-2.0/
```

--libs:
```
-pthread -lclutter-1.0 -latk-1.0 -lcogl-pango -ljson-glib-1.0 -lgdk-3 -lXi -lcogl -lgmodule-2.0 -lwayland-egl -lgbm -ldrm -lwayland-server -lEGL -lX11 -lXext -lXdamage -lXcomposite -lXrandr -lwayland-client -lgio-2.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lpango-1.0 -lcairo -lgobject-2.0 -lglib-2.0 -lXfixes
```

Getting a fresh BBB up and running
==================================

Download the latest image:

```
http://feeds.thing-printer.com/images/BBB-eMMC-flasher-v2014-06-2014.11.28.img.xz
```

Which is talked more about on this page:

```
http://wiki.thing-printer.com/index.php?title=Thing_image
```

Once you've got that on the BBB, you'll need to update the opkg binary repo and install these items:

```
opkg install libclutter-1.0-dev --force-overwrite 
opkg install libglib-2.0-dev systemd-dev
```

You may also need some of these:

```
cogl-1.0-dev libcogl-dev libcogl-pango-dev libgles-omap3-dev libjson-glib-1.0-dev pango-dev
```

Then you need to get your hands on the ARM Hard Float (hf)  version of the FTDI drivers: `libftd2xx1.1.12.tar.gz`

```
cd orange_box/FTDI_Hack/release
cp *.h /usr/include
cd build/arm926-hf
cp -R lib* /usr/lib
chmod 0755 /usr/lib/libftd2xx.so.1.2.7
ln -sf /usr/lib/libftd2xx.so.1.2.7 /usr/lib/libftd2xx.so
```


Annnnnd then you need to p9813 library installed too. That includes updating the Makefile so you can build it on the BBB itself.

```
In the Makefile:
CC = arm-angstrom-linux-gnueabi-gcc
```

Once you've got those changes done, you should be able to run:

```
make install
```

And at some point you should update and upgrade the opkg packages... Oh, and you need to STOP the services that are already installed on the image:

```
systemctl disable toggle.service
systemctl disable redeem.service
systemctl disable mjpg-streamer.service
systemctl disable octoprint.service
```



Oh. Alsa. Audio stuff
=====================

Now that we're trying to get audio stuff to work, we'll need to get some other packages installed on the BBB:

```
opkg install alsa-utils alsa-lib
```

and on Ubuntu
```
sudo apt-get install alsa-utils libasound-dev libsndfile-dev fftw3-dev
```


This worked to record on ubuntu:
```
arecord -D plughw:0,1 -r 16000 -f S16_LE -c 1 -d 3 recorded.wav
```

And this worked to play on ubuntu:
```
aplay -D plughw:0,0 recorded.wav
```

Turn on the boot log
===================================

By Default, the image we're using from Thing Printer doesn't spit out a visual boot log

So, to get the boot log to load up on the screen on boot:

```
mkdir /mnt/boot
mount /dev/mmcblk0p1 /mnt/boot
vi /mnt/boot/uEnv.txt
```

Once you're in there, go to the end of the line and add:

```
console=ttyO0,115200n8 console=tty0,115200n8 video=HDMI-A-1:800x480M@60
```

This will turn on a boot console on the first hardware UART, the first virtual terminal (alt-F1), and force the HDMI output to a size that the 7" LCD touchscreen can support.


WiFi on the BBB
===============

opkg install linux-firmware-rtl8192cu


Installing the Clutter as a service on the BBB
===============================================

After building you can configure this clutter app to open on boot. Open the clutter_window.service file and set the path to the executable binary correctly for your machine. Once it's modified install that clutter_window.service file into:

/lib/systemd/system/

Once it's there, run these commands to get it installed:

systemctl enable clutter_window.service
systemctl start clutter_window.service
systemctl status clutter_window.service

That should enable the script when ever the BBB boots.