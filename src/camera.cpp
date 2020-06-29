#include "telabrium/camera.h"

std::unique_ptr<Camera> Camera::ACTIVE = nullptr;

//Generic Camera Class
//--------------------

Camera::Camera(glm::vec3 position, glm::vec3 rotation, glm::vec3 up) : Object3d(position, rotation) {
	WorldUp = up;
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix(){
	return glm::lookAt(position, position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix(){
	return glm::perspective(glm::radians(fov), Aspect, 0.1f, 100.0f);
}

void Camera::updateCameraVectors(){
	// TODO: No fuckin clue if this works, just override for child classes for now
	Front = glm::normalize(glm::vec3(0,0,1) * rotation);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
	
};