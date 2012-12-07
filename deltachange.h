#ifndef DELTA
#define DELTA

#include <vector>
#include "dmesh.h"
#include "ofVec3f.h"



class Delta{
	
public:
	Delta(int, int, ofVec3f);
	Delta(int, int, vector<int> );
	Delta(int, int);
	void setChange(ofVec3f);
	ofVec3f getChange();
	int getVertexId();
	vector<int> getSymFaces();
	bool isSym();

	
private:
	int id;
	int vertex_id;
	int face_id;
	ofVec3f change;	
	bool sym_mode;
	vector<int> sym_faces;
};


#endif

