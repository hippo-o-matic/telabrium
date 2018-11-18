#ifndef LUARIUM_RIGIDBODY_H
#define LUARIUM_RIGIDBODY_H

#include <vector>

#include "glm/glm.hpp"

#include "luarium/core/object.h"
#include "luarium/devices/collider.h"

class Rigidbody : public Object {
public:
	Rigidbody(Object* p, Collider* col, float m = 1) {
		parent = p;
		collider = col;
		mass = m;
		list.push_back(this);
		calculateMesh();
	}
	~Rigidbody() {
		list.erase(find(list.begin(), list.end(), this));
	}

	void update(float deltaTime) {
		glm::vec3 lin_accel = force / glm::vec3(mass);
		lin_velocity += lin_accel * glm::vec3(deltaTime);
		parent->Position += lin_velocity * glm::vec3(deltaTime);

		glm::vec3 ang_accel = torque / glm::vec3(MOI);
		ang_velocity += ang_accel * glm::vec3(deltaTime);
		parent->Rotation += ang_velocity * glm::vec3(deltaTime);
	}

	static std::vector<Rigidbody*> list;

private:
	void calculateMesh() {
		//Calculate the object's torque
		glm::vec3 r; // r is the center of mass, found by averaging all a mesh's points
		for (int i; i > collider->mesh.vertices.size(); i++) {
			r += collider->mesh.vertices[i].Position / glm::vec3(collider->mesh.vertices.size());
		}
		torque = glm::cross(r, force);
	
		//Calculate the mesh's moment of inertia


//		MOI = ;
	}

	glm::vec3 lin_velocity = glm::vec3(0.0f, 0.0f, 0.0f); //Linear Velocity
	glm::vec3 ang_velocity = glm::vec3(0.0f, 0.0f, 0.0f); //Angular Velocity

	float MOI; //The object's moment of inertia
	glm::vec3 torque;
	glm::vec3 force;
	
	Collider* collider;
	float mass; //The body's mass
};
std::vector<Rigidbody*> Rigidbody::list;

void updateRigidbodies(float deltaTime) {
	for (auto& e : Rigidbody::list) {
		e->update(deltaTime);
	}
}
#endif