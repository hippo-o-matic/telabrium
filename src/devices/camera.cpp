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
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), fov(FOV) {
	Position = position;
	WorldUp = up;
	Rotation.y = yaw;
	Rotation.x = pitch;
	Rotation.z = roll;
	updateCameraVectors();
}
// Scalar Constructor
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), fov(FOV) {
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Rotation.y = yaw;
	Rotation.x = pitch;
	Rotation.z = roll;
	updateCameraVectors();
}
// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix(){
	return glm::lookAt(Position, Position + Front, Up);
}
	// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors(){
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
	front.y = sin(glm::radians(Rotation.x));
	front.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
	
};

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
	if (direction == UP)
		Position[1] += velocity;
	if (direction == DOWN)
		Position[1] -= velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Rotation.y += xoffset;
	Rotation.x += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Rotation.x > 89.0f)
			Rotation.x = 89.0f;
		if (Rotation.x < -89.0f)
			Rotation.x = -89.0f;
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
