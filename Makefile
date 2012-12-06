CC=g++ 
GLVDIR=~/glv-0.96
CFLAGS=-c -I $(GLVDIR)/include/ -Wall -m64

LDFLAGS=-framework GLUT -framework OpenGL -framework Cocoa -lm -lstdc++ -L/usr/lib

all: delta


delta: main.o scene.o dmesh.o meshmanager.o deltachange.o face.o vertex.o facevertex.o $(GLVDIR)/build/Release/libGLV.a
	$(CC) $(LDFLAGS)  main.o scene.o dmesh.o meshmanager.o deltachange.o face.o vertex.o facevertex.o  $(GLVDIR)/build/Release/libGLV.a -o delta

scene.o: scene.cpp scene.h facevertex.h vertex.h face.h ofVec3f.h
	$(CC) $(CFLAGS) scene.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

dmesh.o: dmesh.cpp dmesh.h face.h vertex.h facevertex.h ofVec3f.h
	$(CC) $(CFLAGS) dmesh.cpp

vertex.o: vertex.cpp vertex.h ofVec3f.h
	$(CC) $(CFLAGS) vertex.cpp

face.o: face.cpp face.h facevertex.h
	$(CC) $(CFLAGS) face.cpp

facevertex.o: facevertex.cpp facevertex.h face.h
	$(CC) $(CFLAGS) facevertex.cpp

meshmanager.o: meshmanager.cpp dmesh.h deltachange.h
	$(CC) $(CFLAGS) meshmanager.cpp

deltachange.o: deltachange.cpp
	$(CC) $(CFLAGS) deltachange.cpp

clean:
	rm -rf *o delta
