#include "luarium/object.h"

Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
	Position = pos; Rotation = rot; Scale = scl;
}

Object::~Object(){
	if (parent != nullptr){
		std::vector<Object*>* vec = &parent->components; // Get a simplified pointer to the parents components
		vec->erase(std::remove(vec->begin(), vec->end(), this), vec->end()); // Hunt down and kill itself from the parent vector
		parent = NULL;
	}
}

void Object::adopt(Object* C){
	C->parent = this;
	components.push_back(C);
}

void Object::adopt(std::vector<Object*> &C_vec){
	for(unsigned int i=0; i < C_vec.size(); i++){
		C_vec[i]->parent = this;
		components.push_back(C_vec[i]);
	}
}

Object Object::realize(){
	Object o;
	if(parent != nullptr) {
		o.Position = Position + parent->realize().Position;
		o.Rotation = (Rotation + parent->realize().Rotation);
		o.Scale = Scale + parent->realize().Scale;
	}

	return o;
}

std::vector<Object*> Object::operator+(Object* o){
	components.push_back(o);
	o->parent = this;
	return components;
}

std::vector<Object*> Object::operator-(Object* o){
	std::vector<Object*>* vec = &components; // Same thing as object destructor but with itself instead of parent
	vec->erase(std::remove(vec->begin(), vec->end(), o), vec->end());
	o->parent = nullptr;
	return components;
}

