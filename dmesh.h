using namespace std;


#ifndef DMESH
#define DMESH


#include "vertex.h"
#include "face.h"
#include "facevertex.h"
#include "ofVec3f.h"
#include "deltachange.h"
#include <vector>
#include <set>
#include <stack>
#include <map>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <assert.h>



#ifndef TWO_PI
	#define TWO_PI   6.28318530717958647693
	#endif

typedef ofVec3f BoundingBox3f;

class Delta;

class Mesh{
	
public:
	Mesh(int, vector<Vertex*>, vector<Face*>);
	Mesh(int s, vector<Vertex*> v, vector<Face*> f,vector<Delta*> changes);
	Mesh(int, Mesh* , vector<Delta*> );
	Mesh(const Mesh& m);
	map<int, Face*> getFaces();
	vector<Vertex*> getVList();
	void subdivide();
	void addNewFaces(Face*, int, int, int, int, int, int);
	void deleteOldFaces();
	void subdivideNeighbor(Face*);
	int  getVertexOnEdgeId(int , int );
	void connectEdgeFaces(Face* , Face* , int , int , int );
	Vertex* getOrMakeVertex(Face*, FaceVertex* , FaceVertex* );
	void updateIncidentEdgeData();
	ofVec3f loopEdgeValue(Face* f, FaceVertex* A, FaceVertex* B, Vertex* v);
	ofVec3f loopVertexValue(Face* f, FaceVertex* A);
	ofVec3f computeFaceNormal(Face* f);
	ofVec3f computeVertexNormal(Vertex* );
	void updateNormals();
	void addModifications(vector<Delta*> changes);
	void resetVariables();
	ofVec3f getBoxMin();
	ofVec3f getBoxMax();
	double getBoxSize();
	void updateMins(ofVec3f& m, ofVec3f c);
	void updateMaxs(ofVec3f& m, ofVec3f c);
	void setFacePointers();
	void applyScaling(ofVec3f);
	void mirrorMesh(vector<int>);
	void constructTopology();


private:
	bool closeEnough(ofVec3f a, ofVec3f b);

	int stage;
	vector<Vertex*> vList; //all vertexes from 
	map<int, Face*> faces;
	map<int, Face*> new_faces;
	stack<Face*> to_check;
	float weight;
	ofVec3f box_min;
	ofVec3f box_max;
	map<int, int> twin_vertices; //<a map from the orignial vertex to it's mirror vertex >>


};

#endif 
