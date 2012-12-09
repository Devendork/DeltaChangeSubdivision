#ifndef VTX
#define VTX

#include <set>
#include "ofVec3f.h"
#include "face.h"

typedef int PICKSTATE;
#define OFF 0
#define OVER 1
#define ON 2

class Vertex{
	
public:
	Vertex(int, float, float, float);
	Vertex(Vertex* );
	ofVec3f getColor();
	ofVec3f getPoint();
	void setPoint(ofVec3f);
	void setFace(Face* );
	Face* getFace();
	void addIncidentEdge();
	int getIncident();
	void setIncidentEdge(int );
	float getX();
	int getId();
	void setState(PICKSTATE s);
	PICKSTATE getState();
	void offset(ofVec3f );
	void setNormal(ofVec3f );
	ofVec3f getNormal();
	ofVec3f getOriginalPoint();
	ofVec3f  getComponentX();
	ofVec3f  getComponentY();
	void setComponents();

	

private:
	int id;
	int incident_edges;
	Face* face;
	ofVec3f p;
	ofVec3f p_offset;
	ofVec3f c;
	ofVec3f normal;
	ofVec3f comp_x;
	ofVec3f comp_y;
	PICKSTATE ps;
	
		
};


#endif

