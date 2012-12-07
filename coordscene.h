#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <glv.h>
#include <glv_binding.h>
#include <glv_util.h>

#include "dmesh.h"
#include "ofVec3f.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include <set>

using namespace glv;
using namespace std;

#define RADS_PER_UNIT_VELOCITY 0.01
#define DIST_PER_UNIT_ROTATION 1

class CoordScene : public View3D{

public:
	CoordScene (const Rect &r=Rect(0));
	void render_coords(float);
	virtual void onDraw3D(GLV& g);


private:
	GLfloat g_xz_theta;
	GLfloat g_yz_theta;



};