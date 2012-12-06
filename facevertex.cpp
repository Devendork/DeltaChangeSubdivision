/*
 *  facevertex.cpp
 *  subdivision
 *
 *  Created by Laura Devendorf on 10/5/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "facevertex.h"


FaceVertex :: FaceVertex(int i){
	id = i;
	hasnext = false;
}

Face* FaceVertex::getNext(){
	return next;
}

void FaceVertex::setNext(Face* f){
	next = f;
	hasnext = true;
}

int FaceVertex::getId(){
	return id;
}

void FaceVertex::reset(){
	hasnext = false;
}

bool FaceVertex::hasNext(){
	return hasnext;
}

Face* FaceVertex::getFace(){
	return face;
}

void FaceVertex::setFace(Face* f){
	face = f;
}



