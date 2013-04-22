CC=g++
INCDIR=include/
CFLAGS=-c -Wall -I$(INCDIR)
LDFLAGS=-lGL -lGLU -lglfw
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
