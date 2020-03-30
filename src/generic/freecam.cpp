#include "telabrium/generic/freecam.h"

extern float deltaTime;

FreeCam::FreeCam(float &deltaTime, glm::vec3 position) : Camera(position) {
    rot.setFuncs(
		std::function<glm::vec3()>([this](){ return this->getRot(); }), 
		std::function<glm::vec3(glm::vec3)>([this](glm::vec3 val) {
            if (val.x > 89 && val.x < 180)
                val.x = 89; 
            if (val.x < 271 && val.x > 180)
                val.x = 271;

			updateCameraVectors(); // Recalculate rotation vectors whenever the rotation is modified
			return this->setRot(val);
		})
	);

    
    controls = new Input();

    controls->setMouseCallback(mouse_control);
    controls->setScrollCallback(scroll_control);

	// TEST: see freecam.h
	controls->setKeyCallback(key_callback);
	controls->setMouseButtonCallback(mb_callback);

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
	
	controls->addBind("up", [this, &deltaTime](){ pos += glm::vec3(0, Speed * deltaTime, 0); }, GLFW_KEY_SPACE);

	controls->addBind("down", [this, &deltaTime](){ pos -= glm::vec3(0, Speed * deltaTime, 0); }, GLFW_KEY_LEFT_CONTROL);

	controls->addBind("sprint_start", [this](){ Speed = 5; }, GLFW_KEY_LEFT_SHIFT);

	controls->addBind("sprint_stop", [this](){ Speed = 2.5; }, GLFW_KEY_LEFT_SHIFT, GLFW_RELEASE);

    controls->activate();
}