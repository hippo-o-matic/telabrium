#include "telabrium/camera.h"

std::vector<Camera*> Camera::cameras{};

Camera::Camera(glm::vec3 position, glm::vec3 rotation, glm::vec3 up) : Object3d(position, rotation) {
	WorldUp = up;
	cameras.push_back(this);
}

Camera::~Camera() {
	cameras.erase(std::find(cameras.begin(), cameras.end(), this));
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::getViewMatrix(){
	return glm::lookAt(position, position + Front, Up);
}

glm::mat4 Camera::getProjectionMatrix(){
	return glm::perspective(glm::radians(fov), getAspectRatio(), 0.1f, 100.0f);
}

glm::mat4 Camera::getOrthoMatrix() {
	return glm::ortho(0.0f, (float)(*display_width), 0.0f, (float)(*display_height), -1.0f, 1.0f);
}

float Camera::getAspectRatio() {
	float w = *display_width;
	float h = *display_height;
	return (w / h >= 1) ? w / h : h / w;
}

void Camera::updateCameraVectors(){
	// TODO: No fuckin clue if this works, just override for child classes for now
	Front = glm::normalize(glm::vec3(0,0,1) * rotation);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
	
};