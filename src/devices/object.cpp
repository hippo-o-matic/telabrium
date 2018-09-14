#include "hipponium/object.h"

Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
	Position = pos; Rotation = rot; Scale = scl;
}

Object::Object(Object* P) {
	parent = P;
	parent->Components.push_back(this);
	Position = glm::vec3(0.0f, 0.0f, 0.0f); Rotation = glm::vec3(0.0f, 0.0f, 0.0f); Scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Object::Object(Object* P, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) {
	parent = P;
	parent->Components.push_back(this);
	Position = pos; Rotation = rot; Scale = scl;
}

Object::~Object(){
	std::vector<Object*>* vec = &parent->Components;
	vec->erase(std::remove(vec->begin(), vec->end(), this), vec->end());
	parent = NULL;
}