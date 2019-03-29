#ifndef OBJECT_H
#define OBJECT_H

#include <string>
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

	std::string Name;

	glm::vec3 Position; // Position relative to parent, if no parent then relative to space
	glm::vec3 Rotation;
	glm::vec3 Scale;

	void adopt(Object* C); // "Adopt" an object by setting its parent
	void adopt(std::vector<Object*> &C_vec); // Same thing for multiple objects

	Object realize();

	std::vector<Object*> operator+(Object* o);
	std::vector<Object*> operator-(Object* o);


protected:
	Object* parent = nullptr;
	std::vector<Object*> components;
};

#endif