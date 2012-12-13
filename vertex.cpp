#include "vertex.h"

//i will always equal v in this case
Vertex :: Vertex(int i, float x, float y, float z){
	id = i;
	p.set(x, y, z);
	p_offset.set(0,0,0);

	c.set(237/255, 28/255, 26/255);
	incident_edges = 0;
	ps = OFF;
	
}

Vertex::Vertex(Vertex* v){
	id = v->getId();
	ofVec3f ptemp = v->getPoint();
	ofVec3f ctemp = v->getColor();

	p_offset.set(0,0,0);
	p.set(ptemp.x, ptemp.y, ptemp.z);
	c.set(ctemp.x, ctemp.y, ctemp.z);
	incident_edges = v->getIncident();

	ps = v->getState();
}


void Vertex::incrementIncident(){
	incident_edges++;
}

int Vertex::getId(){
	return id;
}

float Vertex::getX(){
	return p.x;
}

void Vertex::setPoint(ofVec3f v){
	
	return p.set(v.x, v.y, v.z);
}

ofVec3f Vertex::getOriginalPoint(){
	return p;
}

ofVec3f Vertex::getPoint(){
	return (p+p_offset);
}


ofVec3f Vertex::getColor(){
	return c;
}

void Vertex::setFace(Face* f){
	face = f;
}

Face* Vertex::getFace(){
	return face;
}

void Vertex::setIncidentEdge(int n){
	incident_edges = n;
}

void Vertex::addIncidentEdge(){
	incident_edges++;
}

int Vertex::getIncident(){
	return incident_edges;
}

void Vertex::setState(PICKSTATE s){
	ps = s;
}

PICKSTATE Vertex::getState(){
	return ps;
}

void Vertex::offset(ofVec3f offset){
	p_offset.set(offset);
}

void Vertex::setNormal(ofVec3f n){
	normal.set(n);
	setComponents();
}

void Vertex::setComponents(){
	// http://www.physicsforums.com/showthread.php?t=131675

	comp_x.set(1, 0, normal.x / normal.z);
	comp_y.set(0, 1, normal.y / normal.z);

	comp_x.normalize();
	comp_y.normalize();


}

ofVec3f Vertex::getComponentX(){
	return comp_x;
}

ofVec3f Vertex::getComponentY(){
	return comp_y;
}

ofVec3f Vertex::getNormal(){
	return normal;
}

void Vertex::addComponentNormal(ofVec3f v){
	normal += v;
}

void Vertex::makeNormalAverage(){
	normal /= incident_edges;
	normal.normalize();
}




