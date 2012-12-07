	#include "scene.h"


Scene::Scene (const Rect &r, MeshManager* m, GLfloat nearPlane, GLfloat viewDistance){


	//lets hope this calls back to the main view
	//addCallback(Event::MouseDrag, Behavior::Unused);

	//size = 0;
	mm = m;
	draw_subdivision = 0;

	g_xz_theta = 3*PI/2.;
	g_yz_theta = 0;

	ofVec3f min = mm->getMinPoint();
	double size = mm->getBoxSize();

	cout << size << endl;

	if(!mm->doFlipNormal()) g_fViewDistance = viewDistance;
	else g_fViewDistance = size*2 + min.z;
	g_fNearPlane = nearPlane;
	g_fFarPlane = size*size;

	showCurrentMesh = TRUE;
	showLimitMesh = TRUE;
	sym_mode = FALSE;

	pickTargetSize = size / 100;

	pos(r.left(), r.top());

}

ofVec3f Scene::eye_coords(){
	ofVec3f eye;
	eye.set(g_fViewDistance*cos(g_xz_theta), g_fViewDistance*sin(g_yz_theta), g_fViewDistance*sin(g_xz_theta));
	return eye;
}

void Scene::setDialers(NumberDialer* x, NumberDialer* y, NumberDialer* z, NumberDialer* n){
	nd_x = x;
	nd_y = y; 
	nd_z = z;
	nd_n = n;
}

void Scene::onDraw3D(GLV& g){
	ofVec3f eye = eye_coords();

	GLint view[4];

 
 	glGetIntegerv(GL_VIEWPORT, view);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60, (float)view[2]/(float)view[3], g_fNearPlane, g_fFarPlane);
 	glMatrixMode(GL_MODELVIEW);

	double size = mm->getBoxSize();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // Set up viewing transformation, looking down -Z axis
  
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glClearColor (1.0, 1.0, 1.0, 0.0);
   glLineWidth(1.);

    if(showLimitMesh){
    glPushMatrix();
	gluLookAt(eye.x, eye.y, eye.z, 0, 0, 0, 0, 1, 0);
    render_coords(-1, size);
	enable_lights();
	glTranslatef(-size/2., -size/2., size/2.);
	render_limit_mesh();
	glDisable(GL_LIGHTING);
	glPopMatrix();
	}


	glPushMatrix();
	gluLookAt(eye.x, eye.y, eye.z, 0, 0, 0, 0, 1, 0);
	glTranslatef(-size/2., -size/2., size/2.);
	if(showCurrentMesh) render_current_mesh();
	render_selections();
	glPopMatrix();
	
}


bool Scene::onEvent(Event::t e, GLV& glv){
	switch(e){
	case Event::MouseDrag:
		if(glv.mouse().left()){
			g_xz_theta += glv.mouse().dx() * RADS_PER_UNIT_VELOCITY;
			g_yz_theta += glv.mouse().dy() * RADS_PER_UNIT_VELOCITY;
		}else if(glv.mouse().right()){
			g_fViewDistance += glv.mouse().dy() * DIST_PER_UNIT_ROTATION;
		}
	return false;
	case Event::MouseDown:	
		//glv.mouse().setContext(this); 
		pick(glv.mouse().x(),glv.mouse().y(), ON); 
		return false;
	case Event::MouseMove: 
		//glv.mouse().setContext(this); 
		pick(glv.mouse().x(),glv.mouse().y(), OVER); 
		return false;
	case Event::KeyUp:
		keyb(glv.keyboard().key());
		return false;
	}

	return true;	// bubble unrecognized events to parent
}


//exports the OBJ for the limit mesh
void Scene::exportObj(){
	vector<Face*> faces = mm->getLimitMesh()->getFaces();
	vector<Vertex*> vList = mm->getLimitMesh()->getVList();
	

	ofstream obj;
  	obj.open ("deltasub.obj");
  	 
	obj << "# begin vertices" << endl;
  	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
  	 	ofVec3f p = (*it)->getPoint();
  	 	obj << "v " << p.x << " " << p.y << " " << p.z << endl;
  	 }
  	 
  	obj << endl << "# begin faces" << endl;
  	for(vector<Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
  	 	obj << "f " << (*it)->getA()->getId() << " " << (*it)->getB()->getId() << " " << (*it)->getC()->getId()<< endl;
  	 }
  	 
  	 obj.close();
	 cout << "File Exported!" << endl;
}

void Scene::toggleSymmetryMode(){
	sym_mode = !sym_mode;
}

void Scene::toggleLimitMeshVisibility(){
	showLimitMesh = !showLimitMesh;
}

void Scene::toggleCurrentMeshVisibility(){
	showCurrentMesh = !showCurrentMesh;

}

void Scene::setViewDistance(float f){
	g_fViewDistance = f;

}


void Scene::enable_lights(){
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 25.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 1.0 };
   glShadeModel (GL_SMOOTH);

   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_position);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);

}

void Scene::render_current_mesh(){
	vector<Face*> faces = mm->getCurrentMesh()->getFaces();
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
	vector<Face*> :: iterator it;
	
	for(it = faces.begin(); it != faces.end(); it++){
		
		ofVec3f c = (*it)->getColor();
		
		ofVec3f A = vList[((*it)->getA()->id)-1]->getPoint();
		ofVec3f B = vList[((*it)->getB()->id)-1]->getPoint();
		ofVec3f C = vList[((*it)->getC()->id)-1]->getPoint();
		ofVec3f n = (*it)->getFaceNormal();
        
        if(!showLimitMesh || sym_mode){
			if(sym_mode){
				glEnable(GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);				
				
				if((*it)->getState() == OVER) glColor4f(1., 44./255., 207./255., 1.);
				else glColor4f(1., 44./255., 207./255., .5);
		    }else{ 
		    	enable_lights();    
				glColor4f(1., 0., 0., .5);
	        }
	        glBegin(GL_TRIANGLES);
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(A.x, A.y, A.z);
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(B.x, B.y, B.z);
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(C.x, C.y, C.z);
			glEnd();
			
			if(sym_mode) glDisable(GL_BLEND);
			else glDisable(GL_LIGHTING);
		}

		if(sym_mode){
			glColor4f(1., 1., 1., 1);
		}else{
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1., 44./255., 207./255., .5);
		}
		glBegin(GL_LINE_LOOP);
		glVertex3f(A.x, A.y, A.z);
		glVertex3f(B.x, B.y, B.z);
		glVertex3f(C.x, C.y, C.z);
		glEnd();
		if(!sym_mode) glDisable(GL_BLEND);

	}

}

void Scene::render_limit_mesh(){
	vector<Face*> faces = mm->getLimitMesh()->getFaces();
	vector<Vertex*> vList = mm->getLimitMesh()->getVList();
	vector<Face*> :: iterator it;

	for(it = faces.begin(); it != faces.end(); it++){
		
		ofVec3f c = (*it)->getColor();
		glColor3f(c.x, c.y, c.z);
		
		ofVec3f A = vList[((*it)->getA()->id)-1]->getPoint();
		ofVec3f B = vList[((*it)->getB()->id)-1]->getPoint();
		ofVec3f C = vList[((*it)->getC()->id)-1]->getPoint();
		ofVec3f n = (*it)->getFaceNormal();

		if(mm->doFlipNormal()) n *= -1;

		glBegin(GL_TRIANGLES);
		glNormal3f(n.x, n.y, n.z);
		glVertex3f(A.x, A.y, A.z);
		glNormal3f(n.x, n.y, n.z);
		glVertex3f(B.x, B.y, B.z);
		glNormal3f(n.x, n.y, n.z);
		glVertex3f(C.x, C.y, C.z);
		glEnd();

	}
}

void Scene::render_selections(){
	if(!sym_mode) render_vertex_selections();
}

void Scene::render_vertex_selections(){
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();

	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		Vertex* v = *it;
		ofVec3f A = v->getPoint();
		ofVec3f n = v->getNormal();
		glLoadName(v->getId());
		glPushMatrix();
		glTranslatef(A.x, A.y, A.z);
		
		 if(v->getState() != OFF){
			render_coords(v->getId(), pickTargetSize * 6);
			glColor4f(.3, .5, 1., 1.);
		 } 
		 else{
			glEnable (GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			glColor4f(.0, .0, 0., 1.);
		 } 
		
		glutSolidCube(pickTargetSize);
		glDisable(GL_BLEND);
		glPopMatrix();
	}
}

void Scene::render_picks(){
	if(sym_mode) render_face_picks();
	else render_vertex_picks();
}

void Scene::render_vertex_picks(){

	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
		
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		ofVec3f A = (*it)->getPoint();
		ofVec3f n = (*it)->getNormal();
		glLoadName((*it)->getId());
		glPushMatrix();
		glTranslatef(A.x, A.y, A.z);
		glutSolidCube(pickTargetSize);
		glPopMatrix();
	}
}

void Scene::render_face_picks(){
	vector<Face*> faces = mm->getCurrentMesh()->getFaces();
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
		
	for(vector<Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
		ofVec3f A = vList[((*it)->getA()->id)-1]->getPoint();
		ofVec3f B = vList[((*it)->getB()->id)-1]->getPoint();
		ofVec3f C = vList[((*it)->getC()->id)-1]->getPoint();
		ofVec3f n = (*it)->getFaceNormal();
		
		glLoadName((*it)->getId());
		glPushMatrix();

		glBegin(GL_TRIANGLES);
		glNormal3f(n.x, n.y, n.z);
		glVertex3f(A.x, A.y, A.z);
		glNormal3f(n.x, n.y, n.z);
		glVertex3f(B.x, B.y, B.z);
		glNormal3f(n.x, n.y, n.z);
		glVertex3f(C.x, C.y, C.z);
		glEnd();
		glPopMatrix();
	}
}


void Scene::render_coords(int vid, float lineLen){
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();


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

	if(vid > -1 ){
		Vertex* v = vList[vid-1];
		glColor3f(0., 0., 0.);
		glBegin(GL_LINES);
		glVertex3f(0., 0., 0.);
		vec = v->getNormal();
		vec *= lineLen;
		if(mm->doFlipNormal()) vec *= -1;
		glVertex3f(vec.x, vec.y, vec.z);
		glEnd();
	}


	glLineWidth(1.);
	glPopMatrix();

}

void Scene::keyb(int key){
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
	switch(key) {
		case 43:
				draw_subdivision++;
				mm->setCurrentMesh(draw_subdivision);	
			break;
			
		case 45:
				draw_subdivision--;
				if(draw_subdivision < 0) draw_subdivision = 0;
				mm->setCurrentMesh(draw_subdivision);		
			break;

		case 120:
			for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++)
				(*it)->setState(OFF);
			break;	
	}

}

void Scene::pick(int x, int y, PICKSTATE state){
	int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
	y = screenHeight - y;
	
	double size = mm->getBoxSize();
	ofVec3f eye = eye_coords();

	GLuint buff[64] = {0};
 	GLint hits, view[4];

 
 	glSelectBuffer(64, buff);
 	glGetIntegerv(GL_VIEWPORT, view);
 	glRenderMode(GL_SELECT);
 	glInitNames();
 	glPushName(0);
 
 	 glMatrixMode(GL_PROJECTION);
 	 glPushMatrix();
 	 glLoadIdentity();
 

     gluPickMatrix(x, y, 5.0, 5.0, view);
     gluPerspective(60, (float)view[2]/(float)view[3], g_fNearPlane, g_fFarPlane);
 
 	 glMatrixMode(GL_MODELVIEW);
 	 //ADD DRAWING

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   // Set up viewing transformation, looking down -Z axis
	  
	   glLoadIdentity();
	   glMatrixMode(GL_MODELVIEW);
	   glClearColor (1.0, 1.0, 1.0, 0.0);

	 
		glPushMatrix();
		gluLookAt(eye.x, eye.y, eye.z, 0, 0, 0, 0, 1, 0);
		glTranslatef(-size/2., -size/2., size/2.);
		render_picks();
		glPopMatrix();

 	 //DELETE DRAWING
 	 glMatrixMode(GL_PROJECTION);
 	 glPopMatrix();
 

 	hits = glRenderMode(GL_RENDER);
 	change_pick_state(hits, buff, state);
 	glMatrixMode(GL_MODELVIEW);
}

void Scene::update_dialer_values(ofVec3f delta){
 		nd_x->setValue(delta.x);
 		nd_y->setValue(delta.y);
 		nd_z->setValue(delta.z);
 		nd_n->setValue(0);
}


void Scene::change_pick_state(GLint hits, GLuint *names, PICKSTATE state){

	std::set<int> selections;

	//create a set of the selected vertices
	for (int i = 0; i < hits; i++){
 		int s = (GLubyte)names[i * 4 + 3];
 		selections.insert(s);
 	}

 	if(sym_mode) change_pick_faces(selections, state);
 	else change_pick_vertices(selections, state);

}

void Scene::change_pick_vertices(std::set<int> selections, PICKSTATE state){
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
	int count_ons = 0;
	//for each vertex - update the value
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		Vertex* v = *it;

		//hover over or over away
		if(state == OVER){
			switch(v->getState()){
				case OFF:
					if(selections.count(v->getId()) > 0){
						v->setState(OVER);
						//cout << v->getId() << "HOVER: from OFF to OVER " << endl;
					} 
				break;
				case OVER:
					if(selections.count(v->getId()) == 0){
						v->setState(OFF);
						//cout << v->getId() << "HOVER: from OVER to OFF " << endl;
					} 
				break;
				case ON: count_ons++; break;
				default:
				break;
			}
		}else{
			switch(v->getState()){
				case OFF:
					if(selections.count(v->getId()) > 0){
						v->setState(ON);
						count_ons++;
						update_dialer_values(mm->getDeltaValue(v->getId()));
						//cout << v->getId() << "CLICK: from OFF to ON " << endl;
					}

				break;
				case OVER:
					if(selections.count(v->getId()) > 0){ 
						v->setState(ON);
						update_dialer_values(mm->getDeltaValue(v->getId()));
						count_ons++;
						//cout << v->getId() << "CLICK: from OVER to ON " << endl;
					}else{
						v->setState(OFF);
						//cout << v->getId() << "CLICK: from OVER to OFF " << endl;
					} 
				break;
				case ON:
					if(selections.count(v->getId()) > 0){
						v->setState(OFF);
						//cout << v->getId() << "CLICK: from ON to OFF " << endl;
					}else{
						count_ons++;
					}
				default:
				break;
			}
		}

	}

	if(count_ons == 0) update_dialer_values(ofVec3f(0,0,0));
}

void Scene::change_pick_faces(std::set<int> selections, PICKSTATE state){
	vector<Face*> faces = mm->getCurrentMesh()->getFaces();
	int selected_id = -1;

	if(selections.size() > 0){
		Face* closest = closest_face(selections);
		selected_id = closest->getId();
	}

	for(vector<Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
		Face* f = *it;

		//hover over or over away
		if(state == OVER){
			switch(f->getState()){
				case OFF:
					if(selected_id == f->getId()){
						f->setState(OVER);
					} 
				break;
				case OVER:
					if(selected_id != f->getId()){
						f->setState(OFF);
					} 
				break;
			}
		}
	}

	if(state == ON){
		//make the symmetry happen for all the selected vertices
		cout << "REFLECTING ON " << selected_id << endl;

	}

}


Face* Scene::closest_face(std::set<int> ids){
	vector<Face*> faces = mm->getCurrentMesh()->getFaces();
	double min_dist = 2147483647;
	Face* closest;

	for(std::set<int> :: iterator it = ids.begin(); it != ids.end(); it++){

		Face* f = faces[(*it)-1];
		double d = total_distance(f);

		if(d < min_dist){
			min_dist = d;
			closest = f;
		}
	}

	return closest;

}

double Scene::total_distance(Face* f){
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
	ofVec3f eye = eye_coords();
	ofVec3f A = vList[(f->getA()->id)-1]->getPoint();
	ofVec3f B = vList[(f->getB()->id)-1]->getPoint();		
	ofVec3f C = vList[(f->getC()->id)-1]->getPoint();

	double d1 = A.squareDistance(eye);
	double d2 = B.squareDistance(eye);
	double d3 = C.squareDistance(eye);

	return (d1+d2+d3);

}

vector<int> Scene::selected_vertices(){
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
	vector<int> selected;
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		if((*it)->getState() == ON) selected.push_back((*it)->getId());
	}

	return selected;

}


void Scene::adjust_vertex(ofVec3f amount){
	vector<int> selected = selected_vertices();

	if(selected.size() > 0 ){
		for(vector<int> ::iterator it = selected.begin(); it != selected.end(); it++){
			int id = *it;
			mm->adjustVertex(mm->getCurrentMeshLevel(), id, amount);
		} 
	}
}

void Scene::adjust_vertex_by_normal(float f){
	vector<int> selected = selected_vertices();
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();

	if(selected.size() > 0 ){
		for(vector<int> ::iterator it = selected.begin(); it != selected.end(); it++){
			int id = *it;
			ofVec3f amount = vList[id-1]->getNormal();
			amount *= f;
			mm->adjustVertex(mm->getCurrentMeshLevel(), id, amount);
		} 
	}
}



void Scene::set_subdivision(int s){
	mm->setCurrentMesh(s);
}




