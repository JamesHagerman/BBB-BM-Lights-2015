MACHINE= $(shell uname -s)

ifeq ($(MACHINE),Darwin)
	CROSSCOMPILER = 
else
	CROSSCOMPILER = arm-angstrom-linux-gnueabi-
endif

CXX = $(CROSSCOMPILER)g++
CFLAGS = -Wall -g
LDFLAGS = `pkg-config clutter-1.0 --libs`
IFLAGS = `pkg-config clutter-1.0 --cflags`

CLUTTER_INC= `pkg-config clutter-1.0 --cflags`
CLUTTER_LIB= `pkg-config clutter-1.0 --libs`

ifeq ($(MACHINE),Darwin)
	LDFLAGS += -lftd2xx
	CFLAGS += -fomit-frame-pointer -m64
endif
ifeq ($(MACHINE),Linux)
	LDFLAGS += -lpthread -lrt -lm -l/usr/lib/libftd2xx.so.1.2.7
	CFLAGS += -O3 -fomit-frame-pointer
endif


OUTPUT = clutter_window
OBJS = main.o

all: ${OUTPUT}

${OUTPUT}: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o ${OUTPUT}

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(IFLAGS) -o $@ $<


clean:
	rm -rf *.o *.a ${OUTPUT}

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