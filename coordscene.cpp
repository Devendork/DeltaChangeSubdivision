#include "coordscene.h"


CoordScene::CoordScene (const Rect &r){

	g_xz_theta = 3*PI/2.;
	g_yz_theta = 0;
	pos(r.left(), r.top());

}

void CoordScene::onDraw3D(GLV& g){
	GLfloat g_fViewDistance = 100;
	
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glClearColor (1.0, 1.0, 1.0, 0.0);
   glLineWidth(1.);

   glPushMatrix();
   gluLookAt(g_fViewDistance*cos(g_xz_theta), g_fViewDistance*sin(g_yz_theta), g_fViewDistance*sin(g_xz_theta), 0, 0, 0, 0, 1, 0);
   render_coords(80);
   glPopMatrix();
	
}




void CoordScene::render_coords(float lineLen){


	glPushMatrix();
	//draw the frenet frame around the selected point
	glLineWidth(2.);

	ofVec3f vec;

	glColor3f(1.,0,0);
	glBegin(GL_LINES);
	glVertex3f(0., 0., 0.);
	glVertex3f(lineLen, 0., 0.);
	glEnd();

	glColor3f(0, 1., 0);
	glBegin(GL_LINES);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., lineLen, 0.);	
	glEnd();

	glColor3f(0., 0, 1.);
	glBegin(GL_LINES);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., lineLen);
	glEnd();

	glLineWidth(1.);
	glPopMatrix();

}
