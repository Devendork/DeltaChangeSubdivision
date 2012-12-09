#include "deltachange.h"

Delta :: Delta(int did, int vid, ofVec3f p){
	change.set(p);
	id = did;
	vertex_id = vid;
	sym_mode = false;
	completed = false;
}

Delta::Delta(int did, int fid,  vector<int> f){
	face_id = fid;
	id = did;
	sym_faces.assign(f.begin(), f.end());
	sym_mode = true;
	completed = false;

}

Delta :: Delta(int did, int vid){
	id = did;
	vertex_id = vid;
	sym_mode = false;
	completed = false;
}

bool Delta::isCompleted(){
	return completed;
}

void Delta::markCompleted(){
	completed = true;
}

bool Delta::isSym(){
	return sym_mode;
}

vector<int> Delta::getSymFaces(){
	return sym_faces;
}

void Delta::setChange(ofVec3f f){
	change.set(f);
}

ofVec3f Delta::getChange(){
	return change;
}

int Delta::getVertexId(){
	return vertex_id;
}







