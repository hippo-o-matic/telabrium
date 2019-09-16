#include "luarium/camera.h"

float Camera::YAW = -90.0f;
float Camera::PITCH = 0.0f;
float Camera::ROLL = 0.0f;
float Camera::SPEED = 2.5f;
float Camera::SENSITIVTY = 0.1f;
float Camera::FOV = 60.0f;
//Generic Camera Class
//--------------------

//Vector Constructor
Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch, float roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Speed(SPEED), MouseSensitivity(SENSITIVTY), fov(FOV) {
	pos = pos;
	WorldUp = up;
	rot.y = yaw;
	rot.x = pitch;
	rot.z = roll;
	updateCameraVectors();
}
// Scalar Constructor
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Speed(SPEED), MouseSensitivity(SENSITIVTY), fov(FOV) {
	pos = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	rot.y = yaw;
	rot.x = pitch;
	rot.z = roll;
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

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	rot.y += xoffset;
	rot.x += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (rot.x > 89.0f)
			rot.x = 89.0f;
		if (rot.x < -89.0f)
			rot.x = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	if (fov >= 1.0f && fov <= 60.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 60.0f)
		fov = 60.0f;
}
