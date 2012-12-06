// The OpenGL libraries, make sure to include the GLUT and OpenGL frameworks
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <glv.h>
#include <glv_binding.h>
#include <glv_util.h>

#include "scene.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;
using namespace glv;


#define VIEWING_DISTANCE_MIN  3.0
#define LIMIT_DEPTH 5

typedef void BYTE;

vector<int> selected_vertices();


set<int> active_vertices;
unsigned int draw_subdivision;


//static GLfloat g_fViewDistance = 1 * 3.0;
static GLfloat g_fViewDistance = 250;
static GLfloat g_fRotations = -32.000000;
static GLfloat g_fRotationsy = 342.500000;
static GLfloat g_nearPlane = 1;
static GLfloat g_farPlane = 10000;
static int g_Width = 600;                     // Initial window width
static int g_Height = 600;   
static int g_yClick = 0;                      // Initial window height
static int g_xClick = 0;                      // Initial window height
static BOOL g_bButton1Down = FALSE;
static BOOL g_bButton2Down = FALSE;
static BOOL has_file;

static const char * filename;

MeshManager *mm;


void ntExportObj(const Notification& n){
	Scene& sc = *n.receiver<Scene>();
	Button& b = *n.sender<Button>();
	sc.exportObj();
}

void ntSetCurrentVisability(const Notification& n){
	Scene& sc = *n.receiver<Scene>();
	Button& b = *n.sender<Button>();
	sc.toggleCurrentMeshVisibility();
}

void ntSetLimitVisability(const Notification& n){

	Scene& sc = *n.receiver<Scene>();
	Button& b = *n.sender<Button>();
	sc.toggleLimitMeshVisibility();
}



void ntScaleX(const Notification& n){

	//MeshManager& meshm = *n.receiver<MeshManager>();
	NumberDialer& s = *n.sender<NumberDialer>();

	mm->setScaleX(s.getValue());

}

void ntScaleY(const Notification& n){

	//MeshManager& m = *n.receiver<MeshManager>();
	NumberDialer& s = *n.sender<NumberDialer>();
	
	mm->setScaleY(s.getValue());

}

void ntScaleZ(const Notification& n){

	//MeshManager& m = *n.receiver<MeshManager>();
	NumberDialer& s = *n.sender<NumberDialer>();
	
	mm->setScaleZ(s.getValue());

}


void ntSetViewDistance(const Notification& n){

	Scene& sc = *n.receiver<Scene>();
	NumberDialer& s = *n.sender<NumberDialer>();
	
	sc.setViewDistance(s.getValue());

}

void ntSetOffsetNormal(const Notification& n){

	Scene& sc = *n.receiver<Scene>();
	NumberDialer& s = *n.sender<NumberDialer>();
	sc.adjust_vertex_by_normal(s.getValue());
}

void ntSetOffsetX(const Notification& n){
	ofVec3f offset;

	Scene& sc = *n.receiver<Scene>();
	NumberDialer& s = *n.sender<NumberDialer>();
	
	offset.set(s.getValue(), 0, 0);
	sc.adjust_vertex(offset);
}

void ntSetOffsetY(const Notification& n){
	ofVec3f offset;

	Scene& sc = *n.receiver<Scene>();
	NumberDialer& s = *n.sender<NumberDialer>();
	
	offset.set(0, s.getValue(), 0);
	sc.adjust_vertex(offset);
}

void ntSetOffsetZ(const Notification& n){
	ofVec3f offset;

	Scene& sc = *n.receiver<Scene>();
	NumberDialer& s = *n.sender<NumberDialer>();
	
	offset.set(0, 0, s.getValue());
	sc.adjust_vertex(offset);
}


void ntSetSubdivision(const Notification& n){

	Scene& sc = *n.receiver<Scene>();
	NumberDialer& s = *n.sender<NumberDialer>();
	sc.set_subdivision(s.getValue());

}

int main (int argc, char ** argv){
	char * filename;
	for(int i = 0; i<argc; i++) printf("argv[%d] = %s\n", i, argv[i]);
	if(argc > 1){
		filename = argv[2];
		has_file = TRUE;
	} 

	ofVec3f scale(1,1,1);

	if(has_file){
		mm = new MeshManager(scale, LIMIT_DEPTH, has_file, filename);
		g_fViewDistance = 6;
	} 
	else{
		g_fViewDistance = 250;
		mm = new MeshManager(scale, LIMIT_DEPTH, has_file, filename);

	}


	Scene scene(Rect(270, 10, 400, 400), mm, g_nearPlane, g_fViewDistance);


	View viewAdjustments(Rect(10,10, 180,400));
	Label labelVertexAdjustments("Vertex Adjusters", false);	
	Label labelX("X", false);	
	Label labelY("Y", false);	
	Label labelZ("Z", false);	
	Label labelN("Normal", false);	
	View v_x(Rect(0, 0, 200, 20));
	View v_y(Rect(0, 0, 200, 20));
	View v_z(Rect(0, 0, 200, 20));
	View v_n(Rect(0, 0, 200, 20));
	NumberDialer nd_x(Rect(20), 3, 2);
	NumberDialer nd_y(Rect(20), 3, 2);
	NumberDialer nd_z(Rect(20), 3, 2);
	NumberDialer nd_n(Rect(20), 3, 2);

	View viewScales(Rect(10,viewAdjustments.bottom()+20, 400,80));
	Label labelScale("Adjust Scale", false);		
	View v_scalexyz(Rect(0, 0, 300, 20));
	NumberDialer nd_scalex(Rect(20), 3, 2, 0.01, 999);
	NumberDialer nd_scaley(Rect(20), 3, 2, 0.01, 999);
	NumberDialer nd_scalez(Rect(20), 3, 2, 0.01, 999);

	nd_scalex.setValue(1.);
	nd_scaley.setValue(1.);
	nd_scalez.setValue(1.);


	Color c_c(1., 44./255., 207./255.);
	Color c_m(221./255, 1., 44./255.);
	Color c_y(44./255., 249./255, 1.);
	Color c_k(0., 0., 0.);
	
	
	View v_cur(Rect(0, 0, 200, 20));
	Button b_cur;
	Label label_cur("Hide Control Mesh", false);

	View v_limit(Rect(0, 0, 200, 20));
	Button b_limit;
	Label label_limit("Hide Limit Mesh", false);

	View v_export(Rect(0, 0, 200, 20));
	Button b_export;
	Label label_export("Export .OBJ", false);


	View viewSubdivisions(Rect(20));
	Label labelSubdivisions("Subdivision Number", false);
	NumberDialer nd_subdivision(Rect(20), 1, 0, 5, 0);

	Label labelViewDistance("View Distance", false);
	NumberDialer nd_viewdistance(Rect(20), 4, 2, 1000, 0);

	// Set properties of Views	
	View* views[] = {&viewAdjustments, &viewSubdivisions, &viewScales, &scene};
	for(int i=0; i<3; ++i){
		views[i]->addCallback(Event::MouseDrag, Behavior::mouseMove);
		views[i]->enable(KeepWithinParent);
	}

	View* noBorders[] = {&v_x, &v_y, &v_z, &v_n, &v_cur, &v_limit, &v_export, &v_scalexyz};
	for(int i=0; i<8; ++i){
		noBorders[i]->disable(Property::DrawBorder);
		noBorders[i]->disable(Property::DrawBack);
	}

	View* inParents[] = {&nd_x, &nd_y, &nd_z, &nd_n, 
						&labelX, &labelY, &labelZ, &labelN, 
						&b_cur, &label_cur, &b_limit, &label_limit,
						&label_export, &labelScale
						};
	for(int i=0; i<14; ++i){
		inParents[i]->enable(KeepWithinParent);
	}

	Placer placer(viewAdjustments, Direction::S, Place::TL, 10,10, 10);
	Placer placerscales(viewScales, Direction::S, Place::TL, 10,10, 10);
	Placer placerx(v_x, Direction::E, Place::CL, 0, 0, 10);
	Placer placery(v_y, Direction::E, Place::CL, 0, 0, 10);
	Placer placerz(v_z, Direction::E, Place::CL, 0, 0, 10);
	Placer placern(v_n, Direction::E, Place::CL, 0, 0, 10);
	Placer placercur(v_cur, Direction::E, Place::CL, 0,0, 10);
	Placer placerlimit(v_limit, Direction::E, Place::CL, 0,0, 10);
	Placer placerexport(v_export, Direction::E, Place::CL, 0,0, 10);
	Placer placerscalexyz(v_scalexyz, Direction::E, Place::CL, 0,0, 10);

	// Create the Views
	GLV top;	
	top << scene;
	scene << viewAdjustments << viewScales;
	placer << labelVertexAdjustments << v_x << v_y << v_z << v_n 
			<< labelSubdivisions 
			<< nd_subdivision 
			<< labelViewDistance << nd_viewdistance 
			<< v_cur << v_limit << v_export;
	placerx << nd_x << labelX;
	placery << nd_y << labelY;
	placerz << nd_z << labelZ;
	placern << nd_n << labelN;
	placercur << b_cur << label_cur;
	placerlimit << b_limit << label_limit;
	placerexport << b_export << label_export;

	placerscales << labelScale << v_scalexyz;
	placerscalexyz << nd_scalex << nd_scaley << nd_scalez;

	
	scene.stretch(1,1);
	scene.maximize();

	//attach notifications		
	nd_x.attach(ntSetOffsetX, Update::Value, &scene);
	nd_y.attach(ntSetOffsetY, Update::Value, &scene);
	nd_z.attach(ntSetOffsetZ, Update::Value, &scene);
	nd_n.attach(ntSetOffsetNormal, Update::Value, &scene);
	b_cur.attach(ntSetCurrentVisability, Update::Value, &scene);
	b_limit.attach(ntSetLimitVisability, Update::Value, &scene);
	nd_viewdistance.attach(ntSetViewDistance, Update::Value, &scene);
	nd_subdivision.attach(ntSetSubdivision, Update::Value, &scene);
	b_export.attach(ntExportObj, Update::Value, &scene);

	nd_scalex.attach(ntScaleX, Update::Value, &mm);
	nd_scaley.attach(ntScaleY, Update::Value, &mm);
	nd_scalez.attach(ntScaleZ, Update::Value, &mm);

	Window win(1080, 720, "Delta Change Subdivision");
	win.setGLV(top);
	Application::run();
}




