#include "telabrium/object3d.hpp"

Object3d::Object3d(glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scl) : position(_pos), scale(_scl) {
	rotation = glm::quat(glm::radians(_rot));
}

Object3d::Object3d(Object3d* _parent, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scl) : position(_pos), scale(_scl) {
	parent = _parent;
	rotation = glm::quat(glm::radians(_rot));
}

Object3d::Object3d(glm::vec3 _pos, glm::quat _rot, glm::vec3 _scl) : position(_pos), rotation(_rot), scale(_scl) {}

Object3d::Object3d(Object3d* _parent, glm::vec3 _pos, glm::quat _rot, glm::vec3 _scl) : position(_pos), rotation(_rot), scale(_scl) { parent = _parent; }

Object3d::Object3d(Json::Value j) : Object(j) {
	Json::Value jPos = j["pos"];
	Json::Value jRot = j["rot"];
	Json::Value jScl = j["scl"];
	Json::ArrayIndex x=0, y=1, z=2; // Gee thanks implicit conversion of int to const char*, now we have to explicitly specify the type here

	position.x = jPos.get(x, default_pos.x).asFloat();
	position.y = jPos.get(y, default_pos.y).asFloat();
	position.z = jPos.get(z, default_pos.z).asFloat();

	glm::vec3 e_rotation;
	e_rotation.x = jRot.get(x, default_rot.x).asFloat();
	e_rotation.y = jRot.get(y, default_rot.y).asFloat();
	e_rotation.z = jRot.get(z, default_rot.z).asFloat();
	rotation = glm::quat(e_rotation);

	scale.x = jScl.get(x, default_scl.x).asFloat();
	scale.y = jScl.get(y, default_scl.y).asFloat();
	scale.z = jScl.get(z, default_scl.z).asFloat();
}

Object3d::~Object3d() {}


glm::vec3 Object3d::getWorldPos() {
	return glm::vec3(getWorldTransform()[3]);
}

glm::vec3 Object3d::setWorldPos(glm::vec3 dest) {
	return position = dest - getWorldPos();
}


glm::quat Object3d::getWorldRot() {
	return glm::toQuat(getWorldTransform());
}

glm::vec3 Object3d::getWorldRotEuler() {
	return glm::degrees(glm::eulerAngles(glm::toQuat(getWorldTransform())));
}

// Returns the objects rotation in euler angles in degrees
glm::vec3 Object3d::getRotEuler() {
	return glm::degrees(glm::eulerAngles(rotation));
}

glm::quat Object3d::setRotEuler(glm::vec3 dest) {
	return rotation = glm::quat(glm::radians(dest));
}

glm::quat Object3d::setWorldRot(glm::vec3 dest) {
	return rotation = glm::quat(glm::radians(dest)) * glm::inverse(getWorldRot());
}

glm::quat Object3d::setWorldRot(glm::quat dest) {
	return rotation = dest * glm::inverse(getWorldRot());
}


glm::quat Object3d::rotate(glm::vec3 in) {
	glm::vec3 inr = glm::radians(in);
	glm::quat test = glm::quat(inr);
	rotation = glm::normalize(glm::normalize(test) * glm::normalize(rotation));
	return rotation;
	// return rotation = glm::quat(glm::radians(in)) * rotation;
}
glm::quat Object3d::rotate(glm::quat in) {
	return rotation = in * rotation;
}

// glm::quat Object3d::rotateAboutPoint(glm::quat rot, glm::vec3 point) {
// 	glm::mat4 translate = glm::translate(glm::mat4(), point);
// 	glm::mat4 reverse = glm::inverse(translate);

// 	// Translate to point, rotate by rot, reverse the translation, apply to transform
// 	transform = translate * glm::toMat4(rot) * reverse * transform;
// 	return ;
// }


glm::vec3 Object3d::getWorldScl() {
	glm::mat4 transform = getWorldTransform();
	return glm::vec3(
		glm::length(transform[0]),
		glm::length(transform[1]),
		glm::length(transform[2])
	);
}

glm::vec3 Object3d::setWorldScl(glm::vec3 dest) {
	return scale = dest / getWorldScl();
}

// Returns an objects local transformation matrix
glm::mat4 Object3d::getTransform() {
	// Get matricies for each transformation
	glm::mat4 translate_mat = glm::translate(glm::mat4(1), position);
	glm::mat4 rotate_mat = glm::toMat4(rotation);

	// Check that scale is nonzero
	if(scale.x == 0)
		scale.x = 1;
	if(scale.y == 0)
		scale.y = 1;
	if(scale.z == 0)
		scale.z = 1;
	glm::mat4 scale_mat = glm::scale(glm::mat4(1), scale);

	// Combine them all here
	glm::mat4 transform = translate_mat * rotate_mat * scale_mat;

	return transform;
}

// Applies parent transformation matricies if any
glm::mat4 Object3d::getWorldTransform() {
	glm::mat4 transform = getTransform();

	if(parent) {
		// This function checks if a parent has a transform, and applies it if it does
		std::function<void(Object*, glm::mat4&)> parentTransform = [](Object* _parent, glm::mat4& _transform) {
			try { // Try to convert the parent to Object3d
				Object3d* ptr_3d = _parent->to<Object3d>();
				_transform = ptr_3d->getTransform() * _transform; // Apply the transformation if it worked
			} catch(ObjectCastException &e) {
				return; // If it failed, continue to the next parent
			} 
		};

		// Now that the function is defined, run it for each parent in the hierarchy
		parent->runup<glm::mat4&>(parentTransform, transform);
		// The reason we use runup instead of regular recursion is that in the case
		// one of the parents in the path upwards isn't Object3d, the recursion would stop
	}

	return transform;
}

void Object3d::updateVectors() {
	// Potentially have this as a static? idk
	glm::vec3 WorldUp = glm::vec3(0,1,0);
	// Calculate the new Front vector
	// glm::vec3 front;
	// glm::vec3 rad = glm::radians(getWorldRotEuler());
	// front.x = cos(rad.y) * cos(rad.x);
	// front.y = sin(rad.x);
	// front.z = sin(rad.y) * cos(rad.x);
	
	forward = glm::normalize(rotation * glm::vec3(0,0,1));
	// Also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(forward, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, forward));
	
};

// The benefit of this is: vectors dont get outdated and also updateVectors() isnt being called everytime they're accessed(if they arent moving)
// glm::vec3 Object3d::getForwardVec() {
// 	if(rotation != lastRot)
// 		updateVectors();
// 	return forward;
// }
