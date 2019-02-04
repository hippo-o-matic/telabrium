#include "luarium/object.h"

Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
	Position = pos; Rotation = rot; Scale = scl;
}

Object::~Object(){
	if (parent != NULL){
		std::vector<Object*>* vec = &parent->components; // Get a simplified pointer to the parents components
		vec->erase(std::remove(vec->begin(), vec->end(), this), vec->end()); // Hunt down and kill itself from the parent vector
		parent = NULL;
	}
}

void Object::adopt(Object* C){
	C->parent = this;
	C->P_pos = C->Position - Position;
	C->P_rot = C->Rotation - Rotation;
	C->P_scl = C->Scale - Scale;
	components.push_back(C);
}

void Object::adopt(std::vector<Object*> &C_vec){
	for(unsigned int i=0; i < C_vec.size(); i++){
		C_vec[i]->parent = this;
		C_vec[i]->P_pos = C_vec[i]->Position - Position;
		C_vec[i]->P_rot = C_vec[i]->Rotation - Rotation;
		C_vec[i]->P_scl = C_vec[i]->Scale - Scale;
		components.push_back(C_vec[i]);
	}
}

void Object::translate_from(Object& P){
	Rotation = P_rot + P.Rotation; // Real rot = Relative rot + Parent rot

}

std::vector<Object*> Object::operator+(Object* o){
	this->components.push_back(o);
	return this->components;
}

std::vector<Object*> Object::operator-(Object* o){
	std::vector<Object*>* vec = &this->components; // Same thing as object destructor but with itself instead of parent
	vec->erase(std::remove(vec->begin(), vec->end(), o), vec->end());
	return this->components;
}

