/*
 *  face.cpp
 *
 *  
 */

#include "face.h"


//v1, v2, v3 are oriented counter clockwise
Face :: Face(int i, int v1, int v2, int v3){
	id = i;
	color = ofVec3f((i*344587%255)/255., (i*4573%255)/255., (i*2937584%255)/255);
	a = new FaceVertex(v1);
	b = new FaceVertex(v2);
	c = new FaceVertex(v3);
	
	divided = false;
	ps = OFF;
	
}

void Face::setId(int i){
	id = i;
}

void Face::printData(){

	cout << "Face Id: " << id << " is [" << a->id << ", " << b->id << ", " << c->id << "] divided? = " << divided << endl;
	if(!divided){
		cout << "A.next =  " << a->getNext()->getId() << endl;
		cout << "B.next =  " << b->getNext()->getId() << endl;
		cout << "C.next =  " << c->getNext()->getId() << endl;
	}else {
		cout << "A.next =  " << a->getNext()->getId() << " A.face = " << a->getFace()->getId() << endl;
		cout << "B.next =  " << b->getNext()->getId() << " B.face = " << b->getFace()->getId() << endl;
		cout << "C.next =  " << c->getNext()->getId() << " C.face = " << c->getFace()->getId() << endl;
		
	}

}


ofVec3f Face::getColor(){
	return color;
}

FaceVertex* Face::getA(){
	return a;
}

FaceVertex* Face::getB(){
	return b;
}

FaceVertex* Face::getC(){
	return c;
}


void Face::setA(FaceVertex* fv){
	a = fv;
}

void Face::setB(FaceVertex* fv){
	b = fv;
}

void Face::setC(FaceVertex* fv){
	c = fv;
}

void Face::resetFaceVertexNexts(){
	a->resetHasNext();
	b->resetHasNext();
	c->resetHasNext();
}

void Face::setANext(Face* f){
	a->setNext(f);
}

void Face::setBNext(Face* f){
	b->setNext(f);
}

void Face::setCNext(Face* f){
	c->setNext(f);
}

FaceVertex* Face::getFaceVertexId(int id){
	  
	if(a->id == id) return a;
	if(b->id == id) return b;
	if(c->id == id) return c;
	assert(0);
}

//returns the existance of an edge - doesn't check order
bool Face::hasEdge(int x, int y){
	int num_found = 0;
	if(a->id == x || b->id == x || c->id == x) num_found++;
	if(a->id == y || b->id == y || c->id == y) num_found++;
	return (num_found == 2);
}

bool Face::hasVertex(int x){
	return (a->id == x || b->id == x || c->id == x);
}


int Face::getId(){
	return id;
}

bool Face::isDivided(){
	return divided;
}

void Face::setDivided(bool d){
	divided = d;
}

void Face::setNewFaces(Face* A, Face* B, Face* C){
	divided = true;
	a->setFace(A);
	b->setFace(B);
	c->setFace(C);
}

//given two vertices of a triangle, return the third
FaceVertex* Face::getOtherVertex(int x, int y){
	if(a->id != x && a->id != y ) return a;
	if(b->id != x && b->id != y ) return b;
	if(c->id != x && c->id != y ) return c;
	assert(0);

}

FaceVertex* Face::ccwNeighbor(int x){
	if(a->id == x) return b;
	if(b->id == x) return c;
	if(c->id == x) return a;
	assert(0);
	
}


FaceVertex* Face::cwNeighbor(int x){
	
	if(a->id == x) return c;
	if(b->id == x) return a;
	if(c->id == x) return b;
	assert(0);
	
}
			
//takes two vertexes and returns their common vertex id;
int Face::getVertexOnEdgeId(int from, int to){
	vector<Face*> fs;

	//get the trianlges on the end vertices
	fs.push_back(getFaceVertexId(from)->getFace());
	fs.push_back(getFaceVertexId(to)->getFace());
	
	//return the vertex that they have in common
	if(fs[0]->hasVertex(fs[1]->getA()->id)) return fs[1]->getA()->id;
	if(fs[0]->hasVertex(fs[1]->getB()->id)) return fs[1]->getB()->id;
	if(fs[0]->hasVertex(fs[1]->getC()->id)) return fs[1]->getC()->id;
	
	assert(0);
}	

void Face::setFaceNormal(ofVec3f normal){
	normal *= -1;
	n.set(normal);
}

ofVec3f Face::getFaceNormal(){
	return n;
}

PICKSTATE Face::getState(){
	return ps;
}

void Face::setState(PICKSTATE s){
	ps = s;
}


