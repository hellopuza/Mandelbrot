####

CC = g++
CFLAGS = -c -Wall -O2
LDFLAGS =
LIBS = -lsfml-system -lsfml-graphics -lsfml-window
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = Mandelbrot

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	make clean

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o


