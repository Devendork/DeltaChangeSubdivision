#include "deltachange.h"

Delta :: Delta(int vertex_id, ofVec3f p){
	change.set(p);
	id = vertex_id;

	
}

Delta :: Delta(int vertex_id){
	id = vertex_id;
}

void Delta::setChange(ofVec3f f){
	change.set(f);
}

ofVec3f Delta::getChange(){
	return change;
}

int Delta::getVertexId(){
	return id;
}







