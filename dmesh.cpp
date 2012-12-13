
#include "dmesh.h"

//this constructs an initial mesh from a data file
Mesh::Mesh(int s, vector<Vertex*> v, vector<Face*> f){
	stage = s;
	box_min.set(10000, 10000, 10000);
	box_max.set(-10000, -10000, -10000);
	stage = s;
	
	for(vector<Face*> :: iterator it = f.begin(); it != f.end(); it++) faces.insert(pair<int, Face*> ((*it)->getId(), *it));
	for(vector<Vertex*> :: iterator it = v.begin(); it != v.end(); it++){
		vList.push_back(*it);
		twin_vertices.insert(pair<int, int> ((*it)->getId(), (*it)->getId())); // to start, everything points to itself
	} 

	box_min.set(10000, 10000, 10000);
	box_max.set(-10000, -10000, -10000);
	
	//update Bounding box
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		updateMins(box_min, (*it)->getPoint());
		updateMaxs(box_max, (*it)->getPoint());
	}

	constructTopology();
	updateMeshData();

//	setFacePointers();
//	updateNormals();
}

//this constructs an initial mesh from a data file and applies modifications
Mesh::Mesh(int s, vector<Vertex*> v, vector<Face*> f, vector<Delta*> changes){
	stage = s;
	box_min.set(10000, 10000, 10000);
	box_max.set(-10000, -10000, -10000);
	stage = s;
	
	for(vector<Face*> :: iterator it = f.begin(); it != f.end(); it++) faces.insert(pair<int, Face*> ((*it)->getId(), *it));
	for(vector<Vertex*> :: iterator it = v.begin(); it != v.end(); it++){
		vList.push_back(*it);
		twin_vertices.insert(pair<int, int> ((*it)->getId(), (*it)->getId())); // to start, everything points to itself
	} 

	box_min.set(10000, 10000, 10000);
	box_max.set(-10000, -10000, -10000);
	
	//update Bounding box
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		updateMins(box_min, (*it)->getPoint());
		updateMaxs(box_max, (*it)->getPoint());
	}
	
	constructTopology();
	addModifications(changes);
	updateMeshData();


}



Mesh::Mesh(int s, Mesh* old, vector<Delta*> changes){

	stage = s;
	
	map<int, Face*> f = old->getFaces();
	vector<Vertex*> v = old->getVList();
	map<int, int> t = old->getTwins();
	
	for(map<int, Face*> :: iterator it = f.begin(); it != f.end(); it++) faces.insert(*it);
	for(vector<Vertex*> :: iterator it = v.begin(); it != v.end(); it++){
		vList.push_back(new Vertex(*it));
	} 

	for(map<int, int> :: iterator it = t.begin(); it != t.end(); it++) twin_vertices.insert(*it);

	subdivide();
	//linkChildren();
	addModifications(changes);
	updateMeshData();
}

void Mesh::constructTopology(){

	// box_min.set(10000, 10000, 10000);
	// box_max.set(-10000, -10000, -10000);
	
	map<int, Face*> f;
	f.insert(faces.begin(), faces.end());

	//update Bounding box
	// for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
	// 	updateMins(box_min, (*it)->getPoint());
	// 	updateMaxs(box_max, (*it)->getPoint());
	// }
	

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
						
		}
		
	}

}


void Mesh::addModifications(vector<Delta*> changes){
	//go through and apply each change 

	for(vector<Delta*> :: iterator it = changes.begin(); it != changes.end(); it++){
		bool sym = (*it)->isSym();
		if(sym){
			//make sure to only do these once per add modifications cycle
			if(!(*it)->isCompleted()){
				mirrorMesh((*it)->getSymFaces());
				(*it)->markCompleted();
			}
		}else{
			int vid = (*it)->getVertexId();
			ofVec3f d = (*it)->getChange();
			vList[vid]->offset(d);
		}
	}

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
	map<int, int> temp_twins;

	int last_face_id;


	std::set<int> planar_set;
	for(vector<int>::iterator it = planar_faces.begin(); it != planar_faces.end(); it++) planar_set.insert(*it);
	 

	Face* pface = faces[(*planar_faces.begin())];
	ofVec3f o = vList[pface->getA()->getId()]->getPoint(); //get a point on this plane
	ofVec3f n = pface->getFaceNormal();
	double w = -(n.x * o.x) - (n.y * o.y) - (n.z * o.z);
	
	for(
		vector<Vertex*>::iterator it = vList.begin(); it != vList.end(); it++){
		ofVec3f p = (*it)->getPoint();
		double distance = n.x*p.x + n.y*p.y + n.z*p.z + w;
		ofVec3f pos = p + 2*distance * -1*n; //starting from the original the distance in the other direction
		int vid = vList.size()+mirror_vertices.size();

		if(!closeEnough(p, pos)){
			mirror_vertices.push_back(new Vertex(vid, pos.x, pos.y, pos.z));
			temp_twins.insert(pair<int, int> ((*it)->getId(), vid));
			insertTwin((*it)->getId(), vid);
		 }else{
		 	temp_twins.insert(pair<int, int> ((*it)->getId(), (*it)->getId())); //link it to itself
		 }
	}
	vList.insert(vList.end(), mirror_vertices.begin(), mirror_vertices.end());

	//find any additional planar triangles 
	for(map<int, Face*>::iterator it = faces.begin(); it != faces.end(); it++){
		last_face_id = (*it).first;
		Face* x = (*it).second;
		if(planar_set.count(x->getId()) > 0) continue;
		int a = x->getA()->getId();
		int b = x->getB()->getId();
		int c = x->getC()->getId();
		if(temp_twins[a] == a && temp_twins[b] == b && temp_twins[c] == c) planar_set.insert(x->getId());
		 
		
	}

	//remove any triangles that lie on the mirror plane
   	for(map<int, Face*>::iterator it = faces.begin(); it != faces.end(); it++){
   		Face* x = (*it).second;
   		if(planar_set.count(x->getId()) == 0) updated_faces.insert(*it);
   	}

   	faces.clear();
   	faces.insert(updated_faces.begin(), updated_faces.end());

	//now replicate remaining faces with new vertex values
	for(map<int, Face*>::iterator it = faces.begin(); it != faces.end(); it++){
		Face* x = (*it).second;
		int a = x->getA()->getId();
		int b = x->getB()->getId();
		int c = x->getC()->getId();

		x->resetFaceVertexNexts(); //this will make it so that it will look for connections

		int fid = last_face_id + mirror_faces.size() + 1; //okay to have mirror faces size because it will be sequential 
		Face* n_face = new Face(fid, temp_twins[b], temp_twins[a], temp_twins[c]); //flip to make ccw
		mirror_faces.insert(pair<int, Face*> (fid, n_face));
	}

	assert(mirror_faces.size() == faces.size());
	faces.insert(mirror_faces.begin(), mirror_faces.end());
	assert(faces.size() == 2*mirror_faces.size()); //make sure we didn't duplicate any indices
	constructTopology();
}

//find the lowest level twin to connect this to
//from is the original vertex and to is the new vertex
void Mesh::insertTwin(int from, int to){
	while(from != twin_vertices[from]) from = twin_vertices[from];
	twin_vertices.insert(pair<int, int> (to, from));
}



//1. Updates the incident edge data for the vertex
//2. Updates the Face Normals
//3. Updates the Vertex Normals

void Mesh::updateMeshData(){


	FaceVertex* from, *to;
	ofVec3f zeros;

	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++){
		(*it)->setIncidentEdge(0);
		(*it)->setNormal(zeros);
	} 

	
	//check from face to face 
	for(map<int, Face*> :: iterator it = faces.begin(); it != faces.end(); it++){
		Face* x = (*it).second;

		x->setFaceNormal(computeFaceNormal(x));
		ofVec3f n = x->getFaceNormal();

		for(int i = 0; i < 3; i++){
			
			switch (i){
				case 0: 
				from = x->getA();
				to = x->getB();
		
				break;

				case 1: 
				from = x->getB();
				to = x->getC();
				break;

				case 2:
				from = x->getC();
				to = x->getA();
			}

			vList[from->id]->incrementIncident();
			vList[from->id]->addComponentNormal(n);

			//if it's a boundary, then make sure to add both directions
			if(!to->hasNext()){
				vList[to->id]->incrementIncident();
				vList[to->id]->addComponentNormal(n);
			} 
		}



	}

	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++) (*it)->makeNormalAverage();

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
	std::set<int> midpoints;
	
	//this should match the size of the current vList
	for(vector<Vertex*> :: iterator it = vList.begin(); it != vList.end(); it++)placement.push_back(temp);

	
	//update the vertex values
	for(map<int, Face*> :: iterator it = originals.begin(); it != originals.end(); it++){
		Face* f = (*it).second;


		for(int i = 0; i < 3; i++){
			
			switch (i){
				case 0: 
				from = f->getA();
				to = f->getB();
		
				break;

				case 1: 
				from = f->getB();
				to = f->getC();
				break;

				case 2:
				from = f->getC();
				to = f->getA();
			}

			placement[from->id] += vList[to->id]->getPoint();
			counts[from->id]++;

			//if it's a boundary, then make sure to add both directions
			if(!to->hasNext())placement[to->id] += vList[from->id]->getPoint();
			


			int vtx_id = f->getVertexOnEdgeId(from->id, to->id);
			midpoints.insert(vtx_id);
			Vertex* vmid = vList[vtx_id];
				if(!checked.count(vtx_id)){
					checked.insert(vtx_id);
					placement[vtx_id] = loopEdgeValue(f, from, to, vmid);
			}
		}

	}

	for(unsigned int i = 0; i < placement.size(); i++){
		if(midpoints.count(i) == 0){
			int K = vList[i]->getIncident();
			placement[i] /= (double) K;
			double alpha = pow(((.375) + (.25)*cos((TWO_PI/(double)K))), 2.) + .375;
			ofVec3f value = (alpha*vList[i]->getPoint()) + ((1. - alpha)*placement[i]);
			placement[i].set(value); //overwrite placement with the new vertex value
		}

		vList[i]->setPoint(placement[i]);
	}

	//update values on edges
	for(map<int, Face*> :: iterator it = originals.begin(); it != originals.end(); it++){
	
	}


	deleteOldFaces();

	
}





ofVec3f Mesh::loopEdgeValue(Face* f, FaceVertex* A, FaceVertex* B, Vertex* v){
 	FaceVertex* D;
	FaceVertex* C = f->getOtherVertex(A->id, B->id);
	
	if(!B->hasNext()) D = C;
	else  D = (B->getNext())->getOtherVertex(A->id, B->id);

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
		//v->setIncidentEdge(6); //any subdivided edge will have 6 incident edges :: not the case with open meshes
		vList.push_back(v);
		twin_vertices.insert(pair<int, int> (v->getId(), v->getId())); //assume that its a base 

		addFamily(v->getId(), from->id, to->id);

		if(to->hasNext() && !to->getNext()->isDivided()) to_check.push(to->getNext());
		return v;
	}
	
}

void Mesh::linkChildren(){


	//go through each edge and map it to another
	for(map <int, map <int, int> > :: iterator it = lineage.begin(); it != lineage.end(); it++){
		int p1 = (*it).first;
		map<int, int> parentchild = (*it).second;

		for(map<int, int> :: iterator mit = parentchild.begin(); mit != parentchild.end(); mit++){
			int p2 = (*mit).first;
			int c = (*mit).second;

			if(twin_vertices[p1] != p1 || twin_vertices[p2] != p2){
				int orig1  = twin_vertices[p1];
				int orig2 = twin_vertices[p2];
				int origc;

				if(orig1 > orig2){
					int temp = orig2;
					orig2 = orig1;
					orig1 = temp;
				}

				
				origc = lineage[orig1][orig2];
				twin_vertices[c] = origc;
			}

		} 

	}

}

void Mesh::addFamily(int child, int p1, int p2){
	assert(p1 != p2);
	assert(child != 0);

	// std::set parents;
	// parents.insert(p1);
	// parents.insert(p2);
	// families.insert(pair<int, std::set<int> >(child, parents));		

	//make sure they are inserted least to least
	if(p1 > p2){
		int temp = p2;
		p2 = p1;
		p1 = temp;
	}

	assert(p1 < p2);

	//add this to a reverse look up list 
	map<int, int> parentchild;
	parentchild.insert(pair<int, int> (p2, child));
	if(lineage.count(p1)) lineage[p1].insert(pair<int, int>(p2, child));
	else lineage.insert(pair<int, map<int, int> > (p1, parentchild) );

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


// //computes the vertex normal by taking the average of the adjacent face normals
// ofVec3f Mesh::computeVertexNormal(Vertex* v){

// 	//start at one face on the vertex
// 	Face* f = v->getFace();
// 	int face_count = 0;

// 	//get the face vertex pointing to this vertex
// 	FaceVertex* fv = f->getFaceVertexId(v->getId());
// 	Face* cur = fv->getNext();

// 	ofVec3f sum;
// 	sum.set(f->getFaceNormal());
// 	face_count++;

// 	//add the additional points
// 	while(cur != f){
// 		face_count++;
// 		sum += cur->getFaceNormal();
// 		fv = cur->getFaceVertexId(v->getId());
// 		cur = fv->getNext();
// 	}

// 	sum /= v->getIncident();
// 	assert(v->getIncident() == face_count);

// 	return sum.normalize();

// }
	

ofVec3f Mesh::computeFaceNormal(Face* f){


	ofVec3f A = vList[f->getA()->getId()]->getPoint();
	ofVec3f B = vList[f->getB()->getId()]->getPoint();
	ofVec3f C = vList[f->getC()->getId()]->getPoint();

	ofVec3f e1 = B-A;
	ofVec3f e2 = C-B;

	ofVec3f normal = (e2.cross(e1)).normalize();
	return normal;

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

map<int, int> Mesh::getTwins(){
	return twin_vertices;
}


// Mesh :: Mesh(const Mesh& m){
// 	map<int, Face*> f = m.faces;
// 	vector<Vertex*> v = m.vList;
	
// 	for(map<int, Face*> :: iterator it = f.begin(); it != f.end(); it++) faces.insert(*it);
// 	for(vector<Vertex*> :: iterator it = v.begin(); it != v.end(); it++) vList.push_back(*it);
// }


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





