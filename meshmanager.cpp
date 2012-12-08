#include "meshmanager.h"

/*this class is in charge of handling all of the different meshes, scaling and updating them as needed */

MeshManager::MeshManager(){
	cout << "I SHOULDN'T BE CALLED!" << endl;
}

MeshManager :: MeshManager(ofVec3f s, int d, bool hf, char* fname){
	hasfile = hf;
	current = 0;
	depth = d;
	limit_depth = d;
	filename = fname;
	scale.set(1.,1.,1.);


	if(hasfile) meshes.push_back(loadObj());
	else meshes.push_back(loadMesh());


	vector<Delta*> changes;
	for(int i = 1; i < depth; i++) meshes.push_back(new Mesh(i, meshes.back(), changes));



}

Mesh* MeshManager::loadMesh(){

	//read in each vertex into a list
	vector<Vertex*> vlist;
	vlist.push_back(new Vertex(1, 0, 0, 0));
	vlist.push_back(new Vertex(2, 100, 0, 0));
	vlist.push_back(new Vertex(3, 100, 0, -100));
	vlist.push_back(new Vertex(4, 0, 0, -100));
	vlist.push_back(new Vertex(5, 0, 100, 0));
	vlist.push_back(new Vertex(6, 100, 100, 0));
	vlist.push_back(new Vertex(7, 100, 100, -100));
	vlist.push_back(new Vertex(8, 0, 100, -100));
	
	vector<Face*> faces;
	faces.push_back(new Face(1, 1, 4, 3));
	faces.push_back(new Face(2, 2, 1, 3));
	faces.push_back(new Face(3, 4, 1, 8));
	faces.push_back(new Face(4, 1, 5, 8));
	faces.push_back(new Face(5, 4, 8, 7));
	faces.push_back(new Face(6, 3, 4, 7));
	faces.push_back(new Face(7, 2, 3, 7));
	faces.push_back(new Face(8, 2, 7, 6));
	faces.push_back(new Face(9, 7, 8, 5));
	faces.push_back(new Face(10,7, 5, 6));
	faces.push_back(new Face(11,6, 5, 1));
	faces.push_back(new Face(12, 6, 1, 2));


	for(vector<Vertex*> :: iterator it = vlist.begin(); it != vlist.end(); it++){
		ofVec3f p = (*it)->getPoint();
		p *= scale;
		(*it)->setPoint(p);
	}


	return new Mesh(0, vlist, faces);	

}



Mesh* MeshManager::loadObj(){
	vector<Vertex*> vlist;
	vector<Face*> faces;

	int size;
	string in, type, data, remaining;
	size_t space, slash;
	
	
	cout << "Loading Shape File: " << filename << endl;
	ifstream myfile(filename);
 	
 	if(myfile.is_open()){
 		cout << "File is open " << endl;
 		while(myfile.good()){
 			getline (myfile,in);

 			space = in.find(" ");
			type = in.substr(0, space);

			remaining = in.substr(space+1);
			

			if(type.compare("v") == 0){
				//get the next three int
				
				space = remaining.find(" ");
				float x = atof((remaining.substr(0, space)).data());
				remaining = remaining.substr(space+1);
				
				space = remaining.find(" ");
				float y = atof((remaining.substr(0, space)).data());
				remaining = remaining.substr(space+1);
				
				space = remaining.find(" ");
				float z = atof((remaining.substr(0, space)).data());
				remaining = remaining.substr(space+1);
			
				vlist.push_back(new Vertex(vlist.size()+1, x*scale.x, y*1.5*scale.y, z*scale.z));
			}
			
			
			if(type.compare("f") == 0){
				//get the next three int
				
				space = remaining.find(" ");
				data = remaining.substr(0, space); //contains faces and slashes
				slash = remaining.find("/");
				int f1 = atoi((remaining.substr(0, slash)).data());
				remaining = remaining.substr(space+1);

				space = remaining.find(" ");
				data = remaining.substr(0, space); //contains faces and slashes
				slash = remaining.find("/");
				int f2 = atoi((remaining.substr(0, slash)).data());
				remaining = remaining.substr(space+1);
				
				space = remaining.find(" ");
				data = remaining.substr(0, space); //contains faces and slashes
				slash = remaining.find("/");
				int f3 = atoi((remaining.substr(0, slash)).data());
				remaining = remaining.substr(space+1);
				
			faces.push_back(new Face(faces.size()+1, f1, f2, f3));
			}

 		}
 	}	
	
	cout << "Num Vertices " <<vlist.size() << endl;
	cout << "Num Faces" << faces.size() << endl;
				

	return new Mesh(0,vlist, faces);

}



//this updates and recalculates all meshes - called in scaling
void MeshManager::updateMeshScaling(ofVec3f oldScale){

	for(int i = 0; i < meshes.size(); i++){
		if(i == 0 ){
			meshes[0]->applyScaling(scale/oldScale);
		    meshes[0]->addModifications(makeDeltaVector(i));

		}else{
			Mesh* prev = meshes[i-1];
			prev->resetVariables();
			meshes[i] = new Mesh(i, prev, makeDeltaVector(i));
		}

	}

	
}


//does not recalculate any mesh before or at the current stage
void MeshManager::updateMeshes(unsigned int stage){

	int num_meshes = meshes.size();

	for(int i = 0; i <= stage; i++){
		meshes[i]->addModifications(makeDeltaVector(i));
	}

	//recalculate all meshes after the current stage mesh from scratch
	for(int i = stage+1; i < num_meshes; i++){
		Mesh* prev = meshes[i-1];
		prev->resetVariables();
		meshes[i] = new Mesh(i, prev, makeDeltaVector(i));
	}

	assert(meshes.size() == num_meshes);
	
}

void MeshManager::setCurrentMesh(unsigned int i){
	current = i;
	vector<Delta*> d;

	if(current == meshes.size()){
		meshes.push_back(new Mesh(meshes.size()+1, meshes.back(), d));
	}else if(current > limit_depth && current <= meshes.size()){
		meshes.erase(meshes.begin()+current, meshes.end());
	}
	
	depth = meshes.size();
}

Mesh* MeshManager::getStateMesh(int i){
	return meshes[i];
}

Mesh* MeshManager::getCurrentMesh(){
	return meshes[current];
}

int MeshManager::getCurrentMeshLevel(){
	return current;
}

Mesh* MeshManager::getLimitMesh(){
	return meshes.back();
}

Mesh* MeshManager::getOriginalMesh(){
	return meshes.front();
}

void MeshManager::printChanges(){
	for(map<int, map<int, int> > :: iterator it = vertex_changes.begin(); it != vertex_changes.end(); it++){
		cout << "Stage " << (*it).first << endl;
		map<int, int> stage_deltas = (*it).second;
		for(map<int, int> ::iterator iit = stage_deltas.begin(); iit != stage_deltas.end(); iit++){
			cout << "[" << (*iit).first << ", " << (*iit).second <<"]" << endl;
		}
	}
}

ofVec3f MeshManager::getVertexDeltaValue(int id){

	map<int, int> stage_deltas;	
	ofVec3f nothing;

	//first see if it exists at all
	if(vertex_changes.count(current) && vertex_changes[current].count(id)){
		return dList[vertex_changes[current][id]]->getChange();	
	}

	return nothing;
}

//returns the face from this set of faces that the delta change is indexed to
//returns -1 if none of those faces were indexed
int MeshManager::getStageFace(int stage, vector<int> faces){
	for(vector<int> :: iterator it = faces.begin(); it != faces.end(); it++){
		if(face_changes[stage].count(*it)) return *it;
	}
	return -1;
}



void MeshManager::applyMirroring(vector<int> faces){
	getOrMakeSymDelta(current, faces);
	updateMeshes(current);

}

Delta* MeshManager::getOrMakeSymDelta(int stage, vector<int> faces){

	map<int, int> stage_deltas;	
	int face_id = getStageFace(stage, faces);
	int id = *faces.begin();
	//if the stage and vertex id exists, send it back
	if(face_changes.count(stage) && face_id != -1){
		return dList[face_changes[stage][id]];

	}else if(face_changes.count(stage) && face_id == -1){
		//the stage was found but this vertex wasn't
		int did = dList.size();
		Delta* d = new Delta(did, id, faces);
		dList.push_back(d);
		face_changes[stage].insert(pair<int, int> (id, did));
		changed_order[stage].push_back(did);
		return d;

	}else if(!face_changes.count(stage)){
		//the stage didn't exist
		int did = dList.size();
		Delta* d = new Delta(did, id, faces);
		dList.push_back(d);
		map<int, int> face_deltas;
		face_deltas.insert(pair<int, int> (id, did));
		face_changes.insert(pair<int, map<int, int> >(stage, face_deltas));

		vector<int> ordering;
		ordering.push_back(did);
		changed_order.insert(pair<int, vector<int> >(stage, ordering));

		return d;	
	}else{
		cout << "Nothing Matched" << endl;
	}

	return NULL;

}



Delta* MeshManager::getOrMakeVertexDelta(int stage, int vertex_id){

	map<int, int> stage_deltas;	

	//if the stage and vertex id exists, send it back
	if(vertex_changes.count(stage) && vertex_changes[stage].count(vertex_id)){
		return dList[vertex_changes[stage][vertex_id]];

	}else if(vertex_changes.count(stage) && !vertex_changes[stage].count(vertex_id)){
		//the stage was found but this vertex wasn't
		int did = dList.size();
		Delta* d = new Delta(did, vertex_id);
		dList.push_back(d);
		vertex_changes[stage].insert(pair<int, int> (vertex_id, did));
		changed_order[stage].push_back(did);
		return d;

	}else if(!vertex_changes.count(stage)){
		//the stage didn't exist
		int did = dList.size();
		Delta* d = new Delta(did, vertex_id);
		dList.push_back(d);
		map<int, int> vertex_deltas;
		vertex_deltas.insert(pair<int, int> (vertex_id, did));
		vertex_changes.insert(pair<int, map<int, int> >(stage, vertex_deltas));

		vector<int> ordering;
		ordering.push_back(did);
		changed_order.insert(pair<int, vector<int> >(stage, ordering));

		return d;	
	}else{
		cout << "Nothing Matched" << endl;
	}

	return NULL;

}


//since we're only updating on component at a time, get the delta that 
//is at this vertex and stage, and just replace the exisitng delta component with the 
//new compnent in change. change will always have 2 zero components and 1 non-zero component
void MeshManager::adjustVertex(int stage, int id, ofVec3f change){


	Delta* d = getOrMakeVertexDelta(stage, id);
	ofVec3f existing = d->getChange();

	if(change.x == 0) change.x = existing.x;
	if(change.y == 0) change.y = existing.y;
	if(change.z == 0) change.z = existing.z;

	d->setChange(change);

	//printChanges();

	updateMeshes(stage);

}

//this constructs a list of deltas to update the scene with
vector<Delta*> MeshManager::makeDeltaVector(int stage){
	vector<Delta*> deltas;

	//nothing has been changed in this phase yet
	if(!changed_order.count(stage)) return deltas;


	vector<int> delta_ids = changed_order[stage];
	for(vector<int>::iterator it = delta_ids.begin(); it != delta_ids.end(); it++) deltas.push_back(dList[(*it)]);


	// for(vector<Delta*> :: iterator it = deltas.begin(); it != deltas.end(); it++){
	// 	ofVec3f d = (*it)->getChange();
	// 	d *= scale; //make sure that we're adjusting the delta by the global scale as well
	// }


	return deltas;

}

double MeshManager::getBoxSize(){
	return meshes[0]->getBoxSize();
}


bool MeshManager::doFlipNormal(){
	return hasfile;
}

ofVec3f MeshManager::getMinPoint(){
	return meshes[0]->getBoxMin();
}


void MeshManager::setScaleX(float f){
	ofVec3f oldScale;
	oldScale.set(scale);
	scale.x = f;

	cout << "Old Scale " << oldScale.x << " " << oldScale.y << " " << oldScale.z <<  endl;
	cout << "New Scale " << scale.x << " " << scale.y << " " << scale.z <<  endl;
	updateMeshScaling(oldScale);
}

void MeshManager::setScaleY(float f){

	ofVec3f oldScale;
	oldScale.set(scale);
	scale.y = f;
	updateMeshScaling(oldScale);

}

void MeshManager::setScaleZ(float f){

	ofVec3f oldScale;
	oldScale.set(scale);
	scale.z = f;
	updateMeshScaling(oldScale);
}






