CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lGL -lGLFW
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=SurfaceSimplification

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $< -o $@ $(CFLAGS)

clean:
	rm $(EXECUTABLE) $(OBJECTS)

.PHONY: all clean
