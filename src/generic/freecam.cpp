#include "telabrium/generic/freecam.h"

double FreeCam::mouse_lastX;
double FreeCam::mouse_lastY;

extern float deltaTime;

FreeCam::FreeCam(float &deltaTime, glm::vec3 position) : Camera(position) {
    controls = new Input();

    controls->setMouseCallback(mouse_control);
    controls->setScrollCallback(scroll_control);

    controls->addBind("forward", [this, &deltaTime]() {
		pos += glm::normalize(glm::vec3(Front.x, 0, Front.z)) * Speed * deltaTime;
	}, GLFW_KEY_W);

	controls->addBind("back", [this, &deltaTime]() { 
		pos -= glm::normalize(glm::vec3(Front.x, 0, Front.z)) * Speed * deltaTime;
	}, GLFW_KEY_S);

	controls->addBind("left", [this, &deltaTime]() {
		pos -= glm::vec3(Right.x, 0, Right.z) * Speed * deltaTime;
	}, GLFW_KEY_A);

	controls->addBind("right", [this, &deltaTime]() {
		pos += glm::vec3(Right.x, 0, Right.z) * Speed * deltaTime;
	}, GLFW_KEY_D);
	
	controls->addBind("up", [this, &deltaTime](){ pos.y += Speed * deltaTime; }, GLFW_KEY_SPACE);

	controls->addBind("down", [this, &deltaTime](){ pos.y -= Speed * deltaTime; }, GLFW_KEY_LEFT_CONTROL);

	controls->addBind("sprint_start", [this](){ Speed = 5; }, GLFW_KEY_LEFT_SHIFT);

	controls->addBind("sprint_stop", [this](){ Speed = 2.5; }, GLFW_KEY_LEFT_SHIFT, GLFW_RELEASE);

    controls->activate();
}