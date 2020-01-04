#include "telabrium/generic/freecam.h"

double FreeCam::mouse_lastX;
double FreeCam::mouse_lastY;

extern float deltaTime;

FreeCam::FreeCam(float &deltaTime, glm::vec3 position) : Camera(position) {
    controls = new Input();

    controls->setMouseCallback(mouse_control);
    controls->setScrollCallback(scroll_control);

    controls->addBind("forward", [this, &deltaTime]() {
		setPos(getPos() + glm::normalize(glm::vec3(Front.x, 0, Front.z)) * Speed * deltaTime);
	}, GLFW_KEY_W);

	controls->addBind("back", [this, &deltaTime]() { 
		setPos(getPos() - glm::normalize(glm::vec3(Front.x, 0, Front.z)) * Speed * deltaTime);
	}, GLFW_KEY_S);

	controls->addBind("left", [this, &deltaTime]() {
		setPos(getPos() - glm::vec3(Right.x, 0, Right.z) * Speed * deltaTime);
	}, GLFW_KEY_A);

	controls->addBind("right", [this, &deltaTime]() {
		setPos(getPos() + glm::vec3(Right.x, 0, Right.z) * Speed * deltaTime);
	}, GLFW_KEY_D);
	
	controls->addBind("up", [this, &deltaTime](){ setPos(getPos() + glm::vec3(0, Speed * deltaTime, 0)); }, GLFW_KEY_SPACE);

	controls->addBind("down", [this, &deltaTime](){ setPos(getPos() - glm::vec3(0, Speed * deltaTime, 0)); }, GLFW_KEY_LEFT_CONTROL);

	controls->addBind("sprint_start", [this](){ Speed = 5; }, GLFW_KEY_LEFT_SHIFT);

	controls->addBind("sprint_stop", [this](){ Speed = 2.5; }, GLFW_KEY_LEFT_SHIFT, GLFW_RELEASE);

	controls->addBind("look_l", [this](){ setRot(getRot() + glm::vec3(0,1,0)); }, GLFW_KEY_J);
	controls->addBind("look_r", [this](){ setRot(getRot() + glm::vec3(0,-1,0)); }, GLFW_KEY_L);
	controls->addBind("look_u", [this](){ setRot(getRot() + glm::vec3(1,0,0)); }, GLFW_KEY_I);
	controls->addBind("look_d", [this](){ setRot(getRot() + glm::vec3(-1,0,0)); }, GLFW_KEY_K);

    controls->activate();
}