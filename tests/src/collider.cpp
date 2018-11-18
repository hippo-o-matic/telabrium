#ifndef LUARIUM_COLLIDER_H
#define LUARIUM_COLLIDER_H

#include "luarium/core/object.h"
#include "luarium/core/mesh.h"

struct physMat {
	float frct; //Friction
	float bounce; //Bounciness
	float soft; //Softness(Collision tolerance)
};

class Collider : public Object {
public:
	Collider(Mesh m, physMat p) {
		mesh = m; mat = p;
		list.push_back(this);
	}
	~Collider() {
		list.erase(find(list.begin(), list.end(), this));
	}

	void detectCollision() {

	}

	void resolveCollision() {

	}

	Mesh mesh;
	physMat p;

	static std::vector<Collider*> list;
};
std::vector<Collider*> Collider::list;

#endif