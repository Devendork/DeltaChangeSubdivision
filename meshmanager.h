#ifndef MESHMANAGER
#define MESHMANAGER

#include <vector>
#include <map>
#include "dmesh.h"
#include "deltachange.h"
#include <sstream>
#include <fstream>
#include <string>



class MeshManager{
	
public:
	MeshManager();
	MeshManager(ofVec3f, int, bool, char*);
	void updateMeshes(unsigned int );
	void updateMeshScaling(ofVec3f );
	void setCurrentMesh(unsigned int i);
	Mesh* getStateMesh(int );
	Mesh* getCurrentMesh( );
	Mesh* getAndSetCurrentMesh();
	Mesh* getOriginalMesh();
	Mesh* getLimitMesh();
	Mesh* loadObj();
	Mesh* loadMesh();
	void adjustVertex(int , int , ofVec3f );
	vector<Delta*> makeDeltaVector(int);
	Delta* getOrMakeDelta(int , int );
	void printChanges();
 	int getCurrentMeshLevel();
 	double getBoxSize();
 	bool doFlipNormal();
 	ofVec3f getMinPoint();
	ofVec3f getDeltaValue(int i);
	void setScaleX(float f);
	void setScaleY(float f);
	void setScaleZ(float f);


	
private:
	vector<Mesh*> meshes;
	map<int, map<int, int> > changes; //map: stage_id -> ( map : vertex_id -> delta_id)
	vector<Delta*> dList; 
	ofVec3f scale;
	int current;
	int depth;
	int limit_depth;
	char* filename;
	bool hasfile;
};


#endif

