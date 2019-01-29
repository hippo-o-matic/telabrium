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

	glm::vec3 Position; // Position in real space
	glm::vec3 Rotation;
	glm::vec3 Scale;

	void adopt(Object* C); // "Adopt" an object by setting its parent, and making it's coordinates relative to your own
	void adopt(std::vector<Object*> &C_vec); // Same thing for multiple objects

	void translate_from(Object &P); // Translate self about another Object (probably the parent)
	void translate_to(Object* P);
	void translate_to(std::vector<Object*> P);

	std::vector<Object*> operator+(Object* o);
	std::vector<Object*> operator-(Object* o);


protected:
	Object* parent;
	std::vector<Object*> components;

	glm::vec3 P_pos; // The position relative to the parent, if any
	glm::vec3 P_rot;
	glm::vec3 P_scl;
};

#endif