/*
 *  face.h
 *  subdivision
 *
 *  Created by Laura Devendorf on 10/5/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef FACE
#define FACE

using namespace std;

#include "facevertex.h"
#include "ofVec3f.h"
#include <set>
#include <vector>
#include <assert.h>

class FaceVertex;

typedef int PICKSTATE;
#define OFF 0
#define OVER 1
#define ON 2

class Face{
	
public:
	Face(int, int, int, int);
	ofVec3f getColor();
	bool hasEdge(int , int );
	bool hasVertex(int );
	FaceVertex* getA();
	FaceVertex* getB();
	FaceVertex* getC();
	void setA(FaceVertex*);
	void setB(FaceVertex*);
	void setC(FaceVertex*);
	void setANext(Face*);
	void setBNext(Face*);
	void setCNext(Face*);	
	FaceVertex* getFaceVertexId(int );
	int getId();
	bool isDivided();
	void markDivided();
	void setDivided(bool);
	vector<Face*> getNewFacesOnEdge(int, int);
	void setNewFaces(Face*, Face*, Face*);
	int getVertexOnEdgeId(int , int );
	FaceVertex* getOtherVertex(int, int);
	FaceVertex* cwNeighbor(int);
	FaceVertex* ccwNeighbor(int);
	void setFaceNormal(ofVec3f);
	ofVec3f getFaceNormal();
	
	PICKSTATE getState();
	void setState(PICKSTATE);

	void printData();

private:
	int id;
	FaceVertex* a; //oriented counter clockwise
	FaceVertex* b;
	FaceVertex* c;
	ofVec3f color;
	ofVec3f n;
	bool divided;	
	PICKSTATE ps;


};


#endif
