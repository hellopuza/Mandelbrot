####

CC = g++
CFLAGS = -c -march=native -msse4.2 -O3
LDFLAGS =
LIBS = -lsfml-system -lsfml-graphics -lsfml-window
SOURCES = main.cpp mandelbrot.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = Mandelbrot

all: $(SOURCES) $(EXECUTABLE) clean

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o


