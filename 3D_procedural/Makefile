####################################################################
#
#  Makefile for OpenGL example from Chapter 1 in Red Book.
#
####################################################################

CC = g++
CFLAGS = -D_DEBUG -I./glm
CPPFLAGS = -g

.cpp.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

LIBDIR = /usr/local/lib
LIBS = -framework OpenGL -framework GLUT -L$(LIBDIR) -lglew -lSOIL -framework CoreFoundation
SRCS = viewer.cpp loadObj.cpp  LoadShaders.cpp
OBJS = viewer.o loadObj.o LoadShaders.o

viewer: $(OBJS) LoadShaders.h loadObj.hpp
	g++ -g -o viewer $(OBJS) $(LIBS)

clean:
	rm -f viewer *.o
