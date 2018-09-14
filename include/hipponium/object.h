#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <algorithm>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Object {
public:
	Object(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scl = glm::vec3(1.0f, 1.0f, 1.0f));

	Object(Object* P);

	Object(Object* P, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scl = glm::vec3(1.0f, 1.0f, 1.0f));

	~Object();

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;

//	std::shared_ptr<Object> ptr(*this);

protected:
	Object* parent;
	std::vector<Object*> Components;
};

#endif