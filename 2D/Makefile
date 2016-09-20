####################################################################
#
#  Makefile for OpenGL example from Chapter 1 in Red Book.
#
####################################################################

CC = g++
CFLAGS = -D_DEBUG -I./glm -I/usr/local/include/
CPPFLAGS = -g

INCLUDE = /usr/local/include
.cpp.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<
LIBDIR = /usr/local/lib
LIBS = -framework OpenGL -framework GLUT -L$(LIBDIR) -lGLEW -lSOIL -framework CoreFoundation
SRCS = viewer.cpp LoadShaders.cpp
OBJS = viewer.o LoadShaders.o

viewer: $(OBJS) LoadShaders.h
	g++ -g -o viewer $(OBJS) $(LIBS)

clean:
	rm -f viewer *.o
