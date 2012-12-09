
#include "dmesh.h"

//this constructs an initial mesh from a data file
Mesh::Mesh(int s, vector<Vertex*> v, vector<Face*> f){
	stage = s;
	box_min.set(10000, 10000, 10000);
	box_max.set(-10000, -10000, -10000);
	stage = s;
	
	for(vector<Face*> :: iterator it = f.begin(); it != f.end(); it++) faces.insert(pair<int, Face*> ((*it)->getId(), *it));
	for(vector<Vertex*> :: iterator it = v.begin(); it != v.end(); it++) vList.push_back(*it);

	box_min.set(10000, 10000, 10000);
	box_max.set(-10000, -10000, -10000);
	
	//update Bounding box
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		updateMins(box_min, (*it)->getPoint());
		updateMaxs(box_max, (*it)->getPoint());
	}
	constructTopology();
}

//this constructs an initial mesh from a data file and applies modifications
Mesh::Mesh(int s, vector<Vertex*> v, vector<Face*> f,vector<Delta*> changes){
	stage = s;
	box_min.set(10000, 10000, 10000);
	box_max.set(-10000, -10000, -10000);
	stage = s;
	
	for(vector<Face*> :: iterator it = f.begin(); it != f.end(); it++) faces.insert(pair<int, Face*> ((*it)->getId(), *it));
	for(vector<Vertex*> :: iterator it = v.begin(); it != v.end(); it++) vList.push_back(*it);

	box_min.set(10000, 10000, 10000);
	box_max.set(-10000, -10000, -10000);
	
	//update Bounding box
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		updateMins(box_min, (*it)->getPoint());
		updateMaxs(box_max, (*it)->getPoint());
	}
	constructTopology();
	addModifications(changes);
	setFacePointers();
	updateNormals();
}



Mesh::Mesh(int s, Mesh* old, vector<Delta*> changes){

	stage = s;
	
	map<int, Face*> f = old->getFaces();
	vector<Vertex*> v = old->getVList();
	
	for(map<int, Face*> :: iterator it = f.begin(); it != f.end(); it++) faces.insert(*it);
	for(vector<Vertex*> :: iterator it = v.begin(); it != v.end(); it++){
		vList.push_back(new Vertex(*it));
	} 
	
	subdivide();
	addModifications(changes);
	setFacePointers();
	updateNormals();
}

void Mesh::constructTopology(){


	cout << "# begin vertices" << endl;
  	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
  	 	ofVec3f p = (*it)->getPoint();
  	 	cout << "v " << (*it)->getId() << ": " << p.x << " " << p.y << " " << p.z << endl;
  	 }
  	 
  	cout << endl << "# begin faces" << endl;
  	for(map<int, Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
  		Face* f = (*it).second;
  	 	cout << "f " << f->getA()->getId() << " " << f->getB()->getId() << " " << f->getC()->getId()<< endl;
  	 }


	// box_min.set(10000, 10000, 10000);
	// box_max.set(-10000, -10000, -10000);
	
	map<int, Face*> f;
	f.insert(faces.begin(), faces.end());

	//update Bounding box
	// for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
	// 	updateMins(box_min, (*it)->getPoint());
	// 	updateMaxs(box_max, (*it)->getPoint());
	// }
	
	//every edge is shared by two faces
	while(f.size() > 0){

		Face* face = (*(f.begin())).second;
		f.erase(f.begin()); //pop's the back
		FaceVertex* to;
		FaceVertex* from;
		FaceVertex* op_face;
		
		for(int i = 0; i < 3; i++){
			int num_found = 0;
			if(i == 0){
				from = face->getA();
				to = face->getB();
			}else if(i == 1){
				from = face->getB();
				to = face->getC();
			}else{
				from = face->getC();
				to = face->getA();
			}
			
			
			//make sure this hasn't already been assigned 
			if(to->hasNext()) continue;
			

			//check the remaining faces to see if one of them has a matching edge
			for(map<int, Face*>::iterator it = f.begin(); it != f.end(); it++){
				Face* x = (*it).second;
				if(x->hasEdge(to->getId(), from->getId())){
					to->setNext(x);
					op_face = x->getFaceVertexId(from->getId());
					op_face->setNext(face);
					num_found++;
				}
			}
			
			if(num_found > 1){
				
				//check the remaining faces to see if one of them has a matching edge
				for(map<int, Face*>::iterator it = f.begin(); it != f.end(); it++){
					Face* x = (*it).second;
					if(x->hasEdge(to->getId(), from->getId())){
						cout << "found: " << x->getA()->id << ", " << x->getB()->id << ", " << x->getC()->id << endl;
					}
				}
			}else if (num_found == 0) {
				cout << "nothing found along " << from->getId() << " to " << to->getId() << endl;
				cout << "on triangle : " << face->getA()->id << ", " << face->getB()->id << ", " << face->getC()->id << endl;

			}
						
		}
		
	}
	
	cout << "Finished Loop " << endl;
	//this is only done once here after the first mesh is initialized
	updateIncidentEdgeData();
	setFacePointers();
	updateNormals();
}


void Mesh::addModifications(vector<Delta*> changes){
	//go through and apply each change 

	for(vector<Delta*> :: iterator it = changes.begin(); it != changes.end(); it++){
		bool sym = (*it)->isSym();
		if(sym){
			mirrorMesh((*it)->getSymFaces());
			cout << " *****  EXIT MIRROR ******* " << endl << endl;
		}else{
			int vid = (*it)->getVertexId();
			ofVec3f d = (*it)->getChange();
			vList[vid]->offset(d);
		}
	}

}

void Mesh::setFacePointers(){
	for(map<int, Face*>::iterator it = faces.begin(); it != faces.end(); it++){
		Face* x = (*it).second;
		vList[x->getA()->getId()]->setFace(x);
		vList[x->getB()->getId()]->setFace(x);
		vList[x->getC()->getId()]->setFace(x);
	}

}


void Mesh::updateNormals(){
	for(map<int, Face*>::iterator it = faces.begin(); it != faces.end(); it++) ((*it).second)->setFaceNormal(computeFaceNormal((*it).second));
	for(vector<Vertex*>::iterator it = vList.begin(); it != vList.end(); it++) (*it)->setNormal(computeVertexNormal(*it));
}

bool Mesh::closeEnough(ofVec3f a, ofVec3f b){
	ofVec3f diff = a-b;
	if(diff.squareLength() < .01) return true;
	return false;
}

void Mesh::mirrorMesh(vector<int> planar_faces){
	map<int, Face*> mirror_faces;
	vector<Vertex*> mirror_vertices;
	map<int, Face*> updated_faces;
	int last_face_id;

	cout << "Num Planar Faces " << planar_faces.size() << endl;

	std::set<int> planar_set;
	for(vector<int>::iterator it = planar_faces.begin(); it != planar_faces.end(); it++){
		planar_set.insert(*it);
		Face* pf = faces[*it];
		cout << "Planar Face: " <<  pf->getId() << " : " << pf->getA()->getId() << " " << pf->getB()->getId() << " " << pf->getC()->getId() << endl;
	} 

	Face* pface = faces[(*planar_faces.begin())];
	ofVec3f o = vList[pface->getA()->getId()]->getPoint(); //get a point on this plane
	ofVec3f n = pface->getFaceNormal();
	double w = -(n.x * o.x) - (n.y * o.y) - (n.z * o.z);
	
	//first just lay across the vertices
	for(
		vector<Vertex*>::iterator it = vList.begin(); it != vList.end(); it++){
		ofVec3f p = (*it)->getPoint();
		double distance = n.x*p.x + n.y*p.y + n.z*p.z + w;
		ofVec3f pos = p + 2*distance * -1*n; //starting from the original the distance in the other direction
		int vid = vList.size()+mirror_vertices.size();

		if(!closeEnough(p, pos)){
			mirror_vertices.push_back(new Vertex(vid, pos.x, pos.y, pos.z));
			twin_vertices.insert(pair<int, int> ((*it)->getId(), vid));
		 }else{
		 	twin_vertices.insert(pair<int, int> ((*it)->getId(), (*it)->getId())); //link it to itself
		 	cout << "pointing " << (*it)->getId() << " to self " << endl;
		 }
	}
	vList.insert(vList.end(), mirror_vertices.begin(), mirror_vertices.end());

	cout << endl << "Size of Planar Set Pre: " << planar_set.size() << endl;
	//find any additional planar triangles 
	for(map<int, Face*>::iterator it = faces.begin(); it != faces.end(); it++){
		last_face_id = (*it).first;
		Face* x = (*it).second;
		if(planar_set.count(x->getId()) > 0) continue;
		int a = x->getA()->getId();
		int b = x->getB()->getId();
		int c = x->getC()->getId();
		if(twin_vertices[a] == a && twin_vertices[b] == b && twin_vertices[c] == c){
			cout << "Face Id: " << x->getId() << " vs index " << (*it).first << endl;
			cout << "Vertices: " << a << ", " << b << ", " << c << endl;
			cout << "Twin Vertices: " << twin_vertices[a] << ", " << twin_vertices[b] << ", " << twin_vertices[c] << endl;
			planar_set.insert(x->getId());
		} 
		
	}
	cout << "Size of Planar Set Post " << planar_set.size() << endl;

	//remove any triangles that lie on the mirror plane
   	for(map<int, Face*>::iterator it = faces.begin(); it != faces.end(); it++){
   		Face* x = (*it).second;
   		cout << (*it).first << " In Planar Set? " << (planar_set.count((*it).first) == 0) << endl;
   		if(planar_set.count(x->getId()) == 0) updated_faces.insert(*it);
   	}

   	cout << "Removed " << faces.size() - updated_faces.size() << " Faces " << endl;
   	faces.clear();
   	faces.insert(updated_faces.begin(), updated_faces.end());

	//make sure the face ids match their indices into the vector
	cout << faces.size() << " After Removals " << endl;
	//for(int i = 0; i < faces.size(); i++) faces[i]->setId(i+1);
	


	//now replicate remaining faces with new vertex values
	for(map<int, Face*>::iterator it = faces.begin(); it != faces.end(); it++){
		Face* x = (*it).second;
		int a = x->getA()->getId();
		int b = x->getB()->getId();
		int c = x->getC()->getId();

		x->resetFaceVertexNexts(); //this will make it so that it will look for connections

		int fid = last_face_id + mirror_faces.size() + 1; //okay to have mirror faces size because it will be sequential 
		cout << "adding new face at " << fid << endl;
		Face* n_face = new Face(fid, twin_vertices[b], twin_vertices[a], twin_vertices[c]); //flip to make ccw
		mirror_faces.insert(pair<int, Face*> (fid, n_face));
	}

	assert(mirror_faces.size() == faces.size());
	faces.insert(mirror_faces.begin(), mirror_faces.end());
	assert(faces.size() == 2*mirror_faces.size()); //make sure we didn't duplicate any indices
	constructTopology();
}

void Mesh::updateIncidentEdgeData(){
	set<int> checked_vtxs;
	int ni, v_id;
	Face* f;
	
	//check from face to face 
	for(map<int, Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
		Face* x = (*it).second;
		for(int i = 0; i < 3; i++){
		
			if(i == 0){
				v_id = x->getA()->id;
				f = x->getA()->getNext();

			}else if(i == 1){
				v_id = x->getB()->id;
				f = x->getB()->getNext();

			}else{
				v_id = x->getC()->id;
				f = x->getC()->getNext();

			}
		
			if(checked_vtxs.count(v_id) == 0){
				ni = 1;
			
				while(f != x){
				f = f->getFaceVertexId(v_id)->getNext();
				ni++;
				}
				
				vList[v_id]->setIncidentEdge(ni);
			}
		}
	}


}


void Mesh::resetVariables(){
	for(map<int, Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
		((*it).second)->setDivided(false);
	}

}


void Mesh::subdivide(){
	
	map<int, Face*> originals;
	vector<ofVec3f> placement;
	set<int> checked;

	for(map<int, Face*> :: iterator it = faces.begin(); it != faces.end(); it++) originals.insert(*it);
	ofVec3f temp;
	temp.set(0,0,0);

	
	Face* f = (*(faces.begin())).second;
		
	Vertex* A = vList[(f->getA()->id)];
	Vertex* B = vList[(f->getB()->id)];
	Vertex* C = vList[(f->getC()->id)];
	Vertex* D = getOrMakeVertex(f, f->getA(), f->getB());
	Vertex* E = getOrMakeVertex(f, f->getB(), f->getC());
	Vertex* F = getOrMakeVertex(f, f->getC(), f->getA());


	//split this face into four faces and add as much info as you can
	addNewFaces(f, A->getId(), B->getId(), C->getId(), D->getId(), E->getId(), F->getId());
	
	//recurse through the stack to subdivide the nest
	while(to_check.size() > 0){
		Face* top = to_check.top();
		to_check.pop();
		subdivideNeighbor(top);
	}
	


	//now that everything is subdivided, go ahead and update all the points	
	FaceVertex *from;
	FaceVertex *to;
	
	//this should match the size of the current vList
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++) placement.push_back(temp);
	for(map<int, Face*> :: iterator it = originals.begin(); it != originals.end(); it++){
		Face* f = (*it).second;
		for(int i = 0; i < 3; i++){
			
			if(i == 0){
				from = f->getA();
				to = f->getB();
				
			}else if(i == 1){
				from = f->getB();
				to = f->getC();
			}else{
				from = f->getC();
				to = f->getA();
			}
			
			//place the original vertex value
			if(!checked.count(from->id)){
				checked.insert(from->id);
				Vertex* v = vList[from->id];
				placement[from->id] = loopVertexValue(f, from);
			}
			
			int vtx_id = f->getVertexOnEdgeId(from->id, to->id);
			Vertex* vmid = vList[vtx_id];
				if(!checked.count(vtx_id)){
					checked.insert(vtx_id);
					placement[vtx_id] = loopEdgeValue(f, from, to, vmid);

			}
		}
	}

	
	
	//once we've computed all the points move them
	for(unsigned int i = 0; i < placement.size(); i++){
		vList[i]->setPoint(placement[i]);
	}

	deleteOldFaces();

	
}


ofVec3f Mesh::loopVertexValue(Face* f, FaceVertex* A){
	
	int vid = A->id;
	int K = vList[vid]->getIncident();
	int count = 1;
	Face* fn;
	FaceVertex* fv;
	ofVec3f value; 
	value.set(0, 0, 0);
	ofVec3f average;
	double alpha;
	
	assert(K >= 3);
	
	//push all the neighboring vertices into a list
	fv = f->ccwNeighbor(vid);
	average = vList[fv->id]->getPoint();

	fn = A->getNext();  
	while(fn != f){
		count++;
		fv = fn->ccwNeighbor(vid);
		fn = fn->getFaceVertexId(vid)->getNext();
		average += vList[fv->id]->getPoint();
	}
	
	assert(count == K);	
	average /= K;
	alpha = pow(((.375) + (.25)*cos((TWO_PI/(double)K))), 2.) + .375;
	
	value = (alpha*vList[vid]->getPoint()) + ((1. - alpha)*average);
	
	
	return value;
	
}


ofVec3f Mesh::loopEdgeValue(Face* f, FaceVertex* A, FaceVertex* B, Vertex* v){

	FaceVertex* C = f->getOtherVertex(A->id, B->id);
	FaceVertex* D = (B->getNext())->getOtherVertex(A->id, B->id);

	ofVec3f va1 = (vList[(A->id)])->getPoint();
	ofVec3f va2 = (vList[(B->id)])->getPoint();
		
	ofVec3f vb1 = (vList[(C->id)])->getPoint();
	ofVec3f vb2 = (vList[(D->id)])->getPoint();
	
	return (.375)*(va1+va2) + (.125)*(vb1+vb2);
}




Vertex* Mesh::getOrMakeVertex(Face* f, FaceVertex* from, FaceVertex* to){
	
	if(to->hasNext() && to->getNext()->isDivided()){
		Face* n = to->getNext();
		int shared = n->getVertexOnEdgeId(from->id, to->id);
		return vList[shared];
	}else{
		ofVec3f mid = (vList[(from->id)]->getPoint() + vList[(to->id)]->getPoint()) / 2.;
		Vertex* v = new Vertex(vList.size(), mid.x, mid.y, mid.z );
		v->setIncidentEdge(6); //any subdivided edge will have 6 incident edges
		vList.push_back(v);		
		if(to->hasNext() && !to->getNext()->isDivided()) to_check.push(to->getNext());
		return v;
	}
	
}


void Mesh::subdivideNeighbor(Face* f){
		
	if(f->isDivided()) return;
	
	Vertex* D;
	Vertex* E;
	Vertex* F;
	
	Vertex* A = vList[f->getA()->id];
	Vertex* B = vList[f->getB()->id];
	Vertex* C = vList[f->getC()->id];
		
	//make any new vertices as needed
	D = getOrMakeVertex(f, f->getA(), f->getB());
	E = getOrMakeVertex(f, f->getB(), f->getC());
	F = getOrMakeVertex(f, f->getC(), f->getA());
	
	//split this face into and add the allowable pointers
	addNewFaces(f, A->getId(), B->getId(), C->getId(), D->getId(), E->getId(), F->getId());

	//connect neighboring triangles across edges
	if(f->getB()->hasNext() && f->getB()->getNext()->isDivided()) connectEdgeFaces(f, f->getB()->getNext(), D->getId(), A->getId(), B->getId());
	if(f->getC()->hasNext() && f->getC()->getNext()->isDivided()) connectEdgeFaces(f, f->getC()->getNext(), E->getId(), B->getId(), C->getId());
	if(f->getA()->hasNext() && f->getA()->getNext()->isDivided()) connectEdgeFaces(f, f->getA()->getNext(), F->getId(), C->getId(), A->getId());
	
	
}

//this function finds new faces from the old faces and connects them together
void Mesh::connectEdgeFaces(Face* cur, Face* last, int shared, int from, int to){

	cur->getFaceVertexId(to)->getFace()->getFaceVertexId(to)->setNext(last->getFaceVertexId(to)->getFace());
	last->getFaceVertexId(to)->getFace()->getFaceVertexId(shared)->setNext(cur->getFaceVertexId(to)->getFace());

	cur->getFaceVertexId(from)->getFace()->getFaceVertexId(shared)->setNext(last->getFaceVertexId(from)->getFace());
	last->getFaceVertexId(from)->getFace()->getFaceVertexId(from)->setNext(cur->getFaceVertexId(from)->getFace());
	
}


//computes the vertex normal by taking the average of the adjacent face normals
ofVec3f Mesh::computeVertexNormal(Vertex* v){

	//start at one face on the vertex
	Face* f = v->getFace();

	//get the face vertex pointing to this vertex
	FaceVertex* fv = f->getFaceVertexId(v->getId());
	Face* cur = fv->getNext();

	ofVec3f sum;
	sum.set(f->getFaceNormal());

	//add the additional points
	while(cur != f){
		sum += cur->getFaceNormal();
		fv = cur->getFaceVertexId(v->getId());
		cur = fv->getNext();
	}

	sum /= v->getIncident();

	return sum.normalize();

}
	

ofVec3f Mesh::computeFaceNormal(Face* f){
	ofVec3f A = vList[f->getA()->getId()]->getPoint();
	ofVec3f B = vList[f->getB()->getId()]->getPoint();
	ofVec3f C = vList[f->getC()->getId()]->getPoint();

	ofVec3f e1 = B-A;
	ofVec3f e2 = C-B;

	return (e1.cross(e2)).normalize();

}


//this function is common to splitting phases
void Mesh::addNewFaces(Face* f, int a, int b, int c, int d, int e, int g){

	//create the four subdividing faces;
	Face* F1 = new Face(new_faces.size(), d,  b,  e); 
	new_faces.insert(pair<int, Face*> (F1->getId(), F1));
	Face* F2 = new Face(new_faces.size(), d,  e,  g); 
	new_faces.insert(pair<int, Face*> (F2->getId(), F2));
	Face* F3 = new Face(new_faces.size(), a,  d,  g); 
	new_faces.insert(pair<int, Face*> (F3->getId(), F3));
	Face* F4 = new Face(new_faces.size(), g,  e,  c); 
	new_faces.insert(pair<int, Face*> (F4->getId(), F4));
	
	//link up everything in the triangle that can be linked up
	//add all pointers to F2 - the central triangle
	F2->setANext(F3);
	F2->setBNext(F1);
	F2->setCNext(F4);
	
	//point the other 3 triangles to F2, the central triangle, when available
	F1->setANext(F2);
	F4->setBNext(F2);
	F3->setCNext(F2);
	
	//order matters here 
	f->setNewFaces(F3, F1, F4);
	
}
	

void Mesh::deleteOldFaces(){
	faces.clear();
	faces.insert(new_faces.begin(), new_faces.end());
	//for(map<int, Face*> ::iterator it = new_faces.begin(); it != new_faces.end(); it++) faces.push_back(*it);
	//new_faces.clear(); //make sure they doesn't call the destructor!
	
}


map<int, Face*> Mesh::getFaces(){
	return faces;
}

vector<Vertex*> Mesh::getVList(){
	return vList;
}


Mesh :: Mesh(const Mesh& m){
	map<int, Face*> f = m.faces;
	vector<Vertex*> v = m.vList;
	
	for(map<int, Face*> :: iterator it = f.begin(); it != f.end(); it++) faces.insert(*it);
	for(vector<Vertex*> :: iterator it = v.begin(); it != v.end(); it++) vList.push_back(*it);
}


ofVec3f Mesh::getBoxMin(){
	return box_min;
}

ofVec3f Mesh::getBoxMax(){
	return box_max;
}

double Mesh::getBoxSize(){

	ofVec3f dims = box_max - box_min;
	if(dims.x > dims.y && dims.x > dims.z) return dims.x;
	else if(dims.y > dims.x && dims.y > dims.z) return dims.y;
	else return dims.z;
}

void Mesh::updateMins(ofVec3f& m, ofVec3f c){
	ofVec3f v = c - m;
	if(v.x < 0) m.x = c.x;
	if(v.y < 0) m.y = c.y;
	if(v.z < 0) m.z = c.z;

}

void Mesh::updateMaxs(ofVec3f& m, ofVec3f c){
	ofVec3f v = m - c;
	if(v.x < 0) m.x = c.x;
	if(v.y < 0) m.y = c.y;
	if(v.z < 0) m.z = c.z;
}

void Mesh::applyScaling(ofVec3f s){
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		ofVec3f p = (*it)->getOriginalPoint(); //make sure to get the original point here, not hte delta changed point
		p*= s;
		(*it)->setPoint(p);
	}

}





