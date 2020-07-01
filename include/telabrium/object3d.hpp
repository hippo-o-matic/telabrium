#pragma once

#include "telabrium/object.h"
#include "telabrium/object3d.hpp"

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"

class Object3d : public Object {
public:
	Object3d(
		glm::vec3 _pos = default_pos,
		glm::vec3 _rot = default_rot,
		glm::vec3 _scl = default_scl
	);
	Object3d(
		Object3d* _parent,
		glm::vec3 _pos = default_pos,
		glm::vec3 _rot = default_rot,
		glm::vec3 _scl = default_scl
	);
	Object3d(
		glm::vec3 _pos,
		glm::quat _rot,
		glm::vec3 _scl = default_scl
	);
	Object3d(
		Object3d* _parent,
		glm::vec3 _pos,
		glm::quat _rot,
		glm::vec3 _scl = default_scl
	);

	Object3d(Json::Value j);

	~Object3d();

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	
	glm::vec3 getWorldPos();
	glm::vec3 setWorldPos(glm::vec3);

	glm::quat getWorldRot();
	glm::vec3 getWorldRotEuler();
	glm::quat setWorldRot(glm::vec3);
	glm::quat setWorldRot(glm::quat);
	
	glm::vec3 getRotEuler();
	glm::quat setRotEuler(glm::vec3);
	// glm::quat rotateAboutPoint(glm::quat, glm::vec3);
	glm::quat rotate(glm::vec3);
	glm::quat rotate(glm::quat);

	glm::vec3 getWorldScl();
	glm::vec3 setWorldScl(glm::vec3);
	// glm::vec3 scaleBy(glm::vec3);

	glm::mat4 getTransform();
	glm::mat4 getWorldTransform();
	// glm::mat4 transformBy(glm::mat4);
	// glm::mat4 transformBy(glm::vec3, glm::quat, glm::vec3);
	// glm::mat4 transformBy(glm::vec3, glm::vec3, glm::vec3);
	// glm::mat4 setTransform(glm::mat4);

	// Directional vectors
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
	// TODO: not a fan of having to call a function as values can become outdated, maybe use getters and setters?
	void updateVectors();
	// Prototype
	// glm::quat lastRot;
	// glm::vec3 getForwardVec(); 

protected:
	// Default values for spatial values
	static constexpr glm::vec3 default_pos = glm::vec3(0);
	static constexpr glm::vec3 default_rot = glm::vec3(0);
	static constexpr glm::vec3 default_scl = glm::vec3(1);
};
