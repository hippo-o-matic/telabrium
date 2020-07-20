#include "telabrium/generic/freecam.h"

extern float deltaTime;

FreeCam::FreeCam(float &deltaTime, glm::vec3 _pos) : Camera(_pos) {    
    controls = new Input();

    controls->setMouseCallback(mouse_control);
    controls->setScrollCallback(scroll_control);

	// TEST: see freecam.h
	controls->setKeyCallback(key_callback);
	controls->setMouseButtonCallback(mb_callback);

    controls->addBind("forward", [this, &deltaTime]() {
		this->position += glm::normalize(glm::vec3(Front.x, 0, Front.z)) * Speed * deltaTime;
	}, GLFW_KEY_W);

	controls->addBind("back", [this, &deltaTime]() {
		this->position -= glm::normalize(glm::vec3(Front.x, 0, Front.z)) * Speed * deltaTime;
	}, GLFW_KEY_S);

	controls->addBind("left", [this, &deltaTime]() {
		this->position -= glm::vec3(Right.x, 0, Right.z) * Speed * deltaTime;
	}, GLFW_KEY_A);

	controls->addBind("right", [this, &deltaTime]() {
		this->position += glm::vec3(Right.x, 0, Right.z) * Speed * deltaTime;
	}, GLFW_KEY_D);
	
	controls->addBind("up", [this, &deltaTime](){ this->position += glm::vec3(0, Speed * deltaTime, 0); }, GLFW_KEY_SPACE);

	controls->addBind("down", [this, &deltaTime](){ this->position -= glm::vec3(0, Speed * deltaTime, 0); }, GLFW_KEY_LEFT_CONTROL);

	controls->addBind("sprint_start", [this](){ Speed = 5; }, GLFW_KEY_LEFT_SHIFT);

	controls->addBind("sprint_stop", [this](){ Speed = 2.5; }, GLFW_KEY_LEFT_SHIFT, GLFW_RELEASE);

    controls->activate();
}

void FreeCam::updateCameraVectors() {
	if(yaw > 360)
		yaw = -360;
	if(yaw < -360)
		yaw = 360;
	if(pitch > 89)
		pitch = 89;
	if(pitch < -89)
		pitch = -89;

	// Calculate the new Front vector
	glm::vec3 front;
	glm::vec3 rad = glm::radians(glm::vec3(pitch, yaw, 0));
	front.x = cos(rad.y) * cos(rad.x);
	front.y = sin(rad.x);
	front.z = sin(rad.y) * cos(rad.x);

	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
	
}