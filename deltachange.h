#ifndef DELTA
#define DELTA

#include <vector>
#include "dmesh.h"
#include "ofVec3f.h"



class Delta{
	
public:
	Delta(int, ofVec3f);
	Delta(int);
	void setChange(ofVec3f);
	ofVec3f getChange();
	int getVertexId();

	
private:
	ofVec3f change;	
	int id;
};


#endif

