#ifndef FACEVTX
#define FACEVTX

#include "face.h"

class Face;

class FaceVertex{
	
public:
	FaceVertex(int);
	int getId();
	Face* getNext();
	void setNext(Face*);
	bool hasNext();
	Face* getFace();
	void setFace(Face*);
	void reset();
	
	
	int id;
private:
	
	bool hasnext;
	Face* next;
	Face* face;
};


#endif

