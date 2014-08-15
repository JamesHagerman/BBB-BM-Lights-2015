CLUTTER_INC= `pkg-config clutter-1.0 --cflags`
CLUTTER_LIB= `pkg-config clutter-1.0 --libs`

CXX = g++
CFLAGS = -Wall -g
LDFLAGS = `pkg-config clutter-1.0 --libs`
IFLAGS = `pkg-config clutter-1.0 --cflags`

OUTPUT = clutter_window
OBJS = main.o

all: ${OUTPUT}

${OUTPUT}: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o ${OUTPUT}

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(IFLAGS) -o $@ $<


clean:
	rm -rf *o ${OUTPUT}
