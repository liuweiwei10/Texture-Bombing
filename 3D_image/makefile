####################################################################
#
#  Makefile for OpenGL example from Chapter 1 in Red Book.
#
####################################################################

CC = g++
CFLAGS = -D_DEBUG -I./glm -I/usr/local/include/
CPPFLAGS = -g

.cpp.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

LIBDIR = /usr/local/lib
LIBS = -framework OpenGL -framework GLUT -L$(LIBDIR) -lglew -lSOIL -framework CoreFoundation
SRCS = viewer.cpp loadObj3.cpp  LoadShaders.cpp
OBJS = viewer.o loadObj3.o LoadShaders.o

viewer: $(OBJS) LoadShaders.h loadObj3.h
	g++ -g -o viewer $(OBJS) $(LIBS)

clean:
	rm -f viewer *.o
