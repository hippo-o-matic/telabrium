#ifndef TELABRIUM_COLLIDER_H
#define TELABRIUM_COLLIDER_H

#include "telabrium/object.h"
#include "telabrium/mesh.h"

struct physMat {
	float frct; //Friction
	float bounce; //Bounciness
	float soft; //Softness(Collision tolerance)
};

class Collider : public Object3d {
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