	#include "scene.h"


Scene::Scene (const Rect &r, MeshManager* m, GLfloat nearPlane, GLfloat viewDistance){

	//size = 0;
	mm = m;
	draw_subdivision = 0;

	g_xz_theta = 3*PI/2.;
	g_yz_theta = 0;

	ofVec3f min = mm->getMinPoint();
	double size = mm->getBoxSize();

	cout << size << endl;

	if(!mm->hasFile()) g_fViewDistance = viewDistance;
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
    //render_coords(-1, size);
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
	map<int, Face*> faces = mm->getLimitMesh()->getFaces();
	vector<Vertex*> vList = mm->getLimitMesh()->getVList();
	

	ofstream obj;
  	obj.open ("deltasub.obj");
  	 
	obj << "# begin vertices" << endl;
  	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
  	 	ofVec3f p = (*it)->getPoint();
  	 	obj << "v " << p.x << " " << p.y << " " << p.z << endl;
  	 }
  	 
  	obj << endl << "# begin faces" << endl;
  	for(map<int, Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
  		Face* x = (*it).second;
  	 	obj << "f " << x->getA()->getId() << " " << x->getB()->getId() << " " << x->getC()->getId()<< endl;
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
	map<int, Face*> faces = mm->getCurrentMesh()->getFaces();
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
	map<int, Face*> :: iterator it;
	
	for(it = faces.begin(); it != faces.end(); it++){
		Face* x = (*it).second;
		ofVec3f c = x->getColor();
		
		ofVec3f A = vList[x->getA()->id]->getPoint();
		ofVec3f B = vList[x->getB()->id]->getPoint();
		ofVec3f C = vList[x->getC()->id]->getPoint();
		ofVec3f n = x->getFaceNormal();
        
        if(!showLimitMesh || sym_mode){
			if(sym_mode){
				glEnable(GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);				
				
				if(x->getState() == OVER) glColor4f(1., 44./255., 207./255., 1.);
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
	map<int, Face*> faces = mm->getLimitMesh()->getFaces();
	vector<Vertex*> vList = mm->getLimitMesh()->getVList();
	map<int, Face*> :: iterator it;

	for(it = faces.begin(); it != faces.end(); it++){
		Face* x = (*it).second;
		ofVec3f c = x->getColor();
		glColor3f(c.x, c.y, c.z);
		
		ofVec3f A = vList[x->getA()->id]->getPoint();
		ofVec3f B = vList[x->getB()->id]->getPoint();
		ofVec3f C = vList[x->getC()->id]->getPoint();
		ofVec3f n = x->getFaceNormal();

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
	map<int, Face*> faces = mm->getCurrentMesh()->getFaces();
	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();
		
	for(map<int, Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
		Face* f = (*it).second;
		ofVec3f A = vList[f->getA()->id]->getPoint();
		ofVec3f B = vList[f->getB()->id]->getPoint();
		ofVec3f C = vList[f->getC()->id]->getPoint();
		ofVec3f n = f->getFaceNormal();
		
		glLoadName(f->getId());
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
		Vertex* v = vList[vid];
		glColor3f(0., 0., 0.);
		glBegin(GL_LINES);
		glVertex3f(0., 0., 0.);
		vec = v->getNormal();
		vec *= lineLen;
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
						update_dialer_values(mm->getVertexDeltaValue(v->getId()));
						//cout << v->getId() << "CLICK: from OFF to ON " << endl;
					}

				break;
				case OVER:
					if(selections.count(v->getId()) > 0){ 
						v->setState(ON);
						update_dialer_values(mm->getVertexDeltaValue(v->getId()));
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

	for(std::set<int> :: iterator it = selections.begin(); it != selections.end(); it++){
		select_twins(*it, vList[*it]->getState());
	}
}

void Scene::select_twins(int i, PICKSTATE ps){
	int base;
	map<int, int> twins = mm->getCurrentMesh()->getTwins();


	vector<Vertex*> vList = mm->getCurrentMesh()->getVList();

	if(twins[i] == i) base = i;
	else base = twins[i];

	for(map<int, int> :: iterator it = twins.begin(); it != twins.end(); it++){
		if(base == (*it).second){
			if(ps != OFF) vList[(*it).first]->setState(ps);
		}
	}


}

void Scene::change_pick_faces(std::set<int> picks, PICKSTATE state){
	int selected_id = -1;
	map<int, Face*> selections;

	if(picks.size() > 0){
		Face* closest = closest_face(picks);
		selected_id = closest->getId();
	    flood_planar_faces(selected_id, selections);
	}

	map<int, Face*> faces = mm->getCurrentMesh()->getFaces();
	for(map<int, Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
		Face* f = (*it).second;

		//hover over or over away
		if(state == OVER){
			switch(f->getState()){
				case OFF:
					if(selections.find(f->getId()) != selections.end()){
						f->setState(OVER);
					} 
				break;
				case OVER:
					if(selections.find(f->getId()) == selections.end()){
						f->setState(OFF);
					} 
				break;
			}
		}
	}

	if(state == ON){
		//make the symmetry happen for all the selected vertices
		if(selections.size() > 0){
			vector<int> fids;
			for(map<int, Face*> :: iterator it = selections.begin(); it != selections.end(); it++) fids.push_back((*it).first);
			 mm->applyMirroring(fids);
		}
	}

}

//starting at this face flood outward to all planar faces 
//push each of their ids into the set
void Scene::flood_planar_faces(int fid, map<int, Face*>& ids){
	map<int, Face*> faces = mm->getCurrentMesh()->getFaces();

	map<int, Face* > to_check;
	map<int, Face* > checked;
	to_check.insert(pair<int, Face*>(fid, faces[fid]));


	while(to_check.size() > 0){
		int id = (*to_check.begin()).first;
		ids.insert(pair<int, Face*>(id, faces[id]));
		checked.insert(pair<int, Face*>(id, faces[id]));

		Face* f = faces[id];
		for(int i = 0; i < 3; i++){
			Face* n;
			if(i == 0) n = f->getA()->getNext();
			else if(i == 1) n = f->getB()->getNext();
			else n = f->getC()->getNext();

			if(checked.find(n->getId()) != checked.end()) continue;
			if(to_check.find(n->getId()) != to_check.end()) continue;
			if(!close_enough(f->getFaceNormal(), n->getFaceNormal())) continue;
			to_check.insert(pair<int, Face*> (n->getId(), n)); 

		}
		to_check.erase(id);
	}

}

bool Scene::close_enough(ofVec3f a, ofVec3f b){
	return a == b;
	// ofVec3f diff = a-b;
	// if(diff.squareLength() < .00001) return true;
	// return false;
}


Face* Scene::closest_face(std::set<int> ids){
	map<int, Face*> faces = mm->getCurrentMesh()->getFaces();
	double min_dist = 2147483647;
	Face* closest;

	for(std::set<int> :: iterator it = ids.begin(); it != ids.end(); it++){

		Face* f = faces[(*it)];
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
	ofVec3f A = vList[f->getA()->id]->getPoint();
	ofVec3f B = vList[f->getB()->id]->getPoint();		
	ofVec3f C = vList[f->getC()->id]->getPoint();

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




