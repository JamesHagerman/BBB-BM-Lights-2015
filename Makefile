MACHINE= $(shell uname -s)
ARCH = $(shell uname -m)

ifeq ($(MACHINE)_$(ARCH),Darwin_x86_64)
	CROSSCOMPILER = 
endif
ifeq ($(MACHINE)_$(ARCH),Linux_armv7l)
#	CROSSCOMPILER = arm-angstrom-linux-gnueabi-
	CROSSCOMPILER = 
endif
ifeq ($(MACHINE)_$(ARCH),Linux_x86_64)	
	CROSSCOMPILER = 
endif

ifeq ($(MACHINE)_$(ARCH),CYGWIN_NT-6.3_x86_64)
	CROSSCOMPILER =
endif


CXX = $(CROSSCOMPILER)g++
CFLAGS = -Wall -g -DDEBUG -DCOGL_ENABLE_EXPERIMENTAL_API -DCLUTTER_ENABLE_EXPERIMENTAL_API
LDFLAGS = `pkg-config clutter-1.0 cogl-2.0-experimental --libs` -lm -lp9813 -lftd2xx  -lpthread -lfftw3 -lasound
IFLAGS = `pkg-config clutter-1.0 cogl-2.0-experimental --cflags`

ifeq ($(MACHINE)_$(ARCH),Darwin_x86_64)
	LDFLAGS = `PKG_CONFIG_PATH=/opt/ImageMagick/lib/pkgconfig:/opt/X11/lib/pkgconfig /usr/local/bin/pkg-config gdk-pixbuf-2.0 clutter-1.0 --libs`
	IFLAGS = `PKG_CONFIG_PATH=/opt/ImageMagick/lib/pkgconfig:/opt/X11/lib/pkgconfig /usr/local/bin/pkg-config gdk-pixbuf-2.0 clutter-1.0 --cflags`
	LDFLAGS += -lftd2xx -lp9813 -lpthread -lfftw3
	CFLAGS += -fomit-frame-pointer
endif


$(warning Building for $(MACHINE)_$(ARCH))
ifeq ($(MACHINE)_$(ARCH),Linux_armv7l)
	LDFLAGS += -lrt -lasound
	#-l/usr/lib/libftd2xx.so.1.2.7 -l/usr/lib/libp9813.a
	CFLAGS += -DBEAGLEBONEBLACK -O3 -fomit-frame-pointer -Wno-unused-result -Wno-unused-but-set-variable
endif

ifeq ($(MACHINE)_$(ARCH),Linux_x86_64)
	LDFLAGS += -lasound
	CFLAGS += -O3 -fomit-frame-pointer -Wno-unused-result -Wno-unused-but-set-variable
endif


ifeq ($(MACHINE)_$(ARCH),CYGWIN_NT-6.3_x86_64)

# Not sure which of these works:
	LDFLAGS = -pthread -lclutter-1.0 -lcogl 
	IFLAGS = -pthread -I/usr/include/clutter-1.0 -I/usr/include/pango-1.0 -I/usr/include/cogl -I/usr/include/cairo -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
	LDFLAGS += -lftd2xx -lp9813

#	LDFLAGS = `pkg-config clutter-1.0 --libs`
#	IFLAGS = `pkg-config clutter-1.0 --cflags`
#	LDFLAGS = -pthread -lclutter-1.0 -lcogl
#	IFLAGS = -pthread -I/usr/include/clutter-1.0 -I/usr/include/pango-1.0 -I/usr/include/cogl -I/usr/include/cairo -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/atk-1.0 -I/usr/include/json-glib-1.0 -I/usr/include/gdk-pixbuf-2.0
#	LDFLAGS += -lasound

	CFLAGS += -fomit-frame-pointer
endif


OUTPUT = clutter_window
OBJS = src/main.o src/TCLControl.o src/events.o src/button.o src/animation.o src/AnimationHelpers.o
#src/alsa.o src/fft.o
#src/sensatron-effect.o

all: ${OUTPUT}

${OUTPUT}: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o ${OUTPUT}

#%.o: %.c
#	$(CXX) -c $(CFLAGS) $(IFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(IFLAGS) -o $@ $<


clean:
	rm -rf src/*.o src/*.a ${OUTPUT}

# The below make scripts will load or unload the original FTDI drivers 
# So that the PaintYourDragon/D2XX drivers can be used for LED output.
# 
# On Mac and Linux, the Virtual COM Port driver must be unloaded in
# order to use bitbang mode.  Use "make unload" to do this, but ALWAYS
# remember to "make load" when done, in order to restore normal serial
# port functionality!

ifeq ($(shell uname -s),Darwin)
# Mac driver stuff
  DRIVER    = com.FTDI.driver.FTDIUSBSerialDriver
  KEXTFLAGS = -b
  unload:
	sudo kextunload $(KEXTFLAGS) $(DRIVER)
  load:
	sudo kextload $(KEXTFLAGS) $(DRIVER)
else
ifeq ($(shell uname -s),Linux)
# Linux driver stuff
  unload:
	sudo modprobe -r ftdi_sio
	sudo modprobe -r usbserial
  load:
	sudo modprobe ftdi_sio
	sudo modprobe usbserial
endif
endif
