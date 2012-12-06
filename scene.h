#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <glv.h>
#include <glv_binding.h>
#include <glv_util.h>

#include "face.h"
#include "facevertex.h"
#include "vertex.h"
#include "dmesh.h"
#include "meshmanager.h"
#include "ofVec3f.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include <set>

using namespace glv;
using namespace std;

typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef int PICKSTATE;
#define OFF 0
#define OVER 1
#define ON 2

class Scene : public View3D{

public:
	Scene (const Rect &r=Rect(0), MeshManager* m = NULL, GLfloat nearPlane = 0, GLfloat viewDistance =0 );
	void render_current_mesh();
	void render_limit_mesh();
	void render_picks();
	void render_coords(Vertex* v);
	void render_selections();


	void enable_lights();
	void pick(int x, int y, PICKSTATE state);
	void change_pick_state(GLint hits, GLuint *names, PICKSTATE state);
	vector<int> selected_vertices();
	void keyb(int key);

	void toggleLimitMeshVisibility();
	void toggleCurrentMeshVisibility();
	void toggleAlignNormal();
	void exportObj();

	void set_size(float f);
	void set_subdivision(int s);
	void adjust_vertex(ofVec3f amount);
	void adjust_vertex_by_normal(float amount);
	void setViewDistance(float f);


	virtual void onDraw3D(GLV& g);
	virtual bool onEvent(Event::t e, GLV& glv);


private:
	//float size;
	MeshManager* mm;
	unsigned int draw_subdivision;
	GLfloat g_fViewDistance;
	GLfloat g_fRotations;
	GLfloat g_fRotationsy;	
	GLfloat g_fFarPlane;
	GLfloat g_fNearPlane;

	float pickTargetSize;

	BOOL showCurrentMesh;
	BOOL showLimitMesh;



};