#include "telabrium/camera.h"

std::unique_ptr<Camera> Camera::ACTIVE = nullptr;

//Generic Camera Class
//--------------------

// Vector Constructor
Camera::Camera(glm::vec3 position, glm::vec3 rotation, glm::vec3 up) {
	pos = position;
	WorldUp = up;
	rot = rotation;
	updateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix(){
	return glm::lookAt(pos, pos + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix(){
	return glm::perspective(glm::radians(fov), Aspect, 0.1f, 100.0f);
}
	// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors(){
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(rot.y)) * cos(glm::radians(rot.x));
	front.y = sin(glm::radians(rot.x));
	front.z = sin(glm::radians(rot.y)) * cos(glm::radians(rot.x));
	
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
	
};
