#pragma once

#include "telabrium/camera.h"
#include "telabrium/input.h"

class FreeCam : public Camera {
public:
    FreeCam(float &deltaTime, glm::vec3 position = glm::vec3(0));

    float Speed = 2.5;
	float MouseSensitivity = 0.1;
	Input* controls;

private:
    double mouse_lastX;
    double mouse_lastY;
	bool last_focus;

    const std::function<void(GLFWwindow*, double, double)> mouse_control = [this](GLFWwindow* w, double xpos, double ypos) {
		if(glfwGetInputMode(w, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			if(last_focus) { // Correct jumps on returning window focus
				mouse_lastX = xpos;
				mouse_lastY = ypos;
				last_focus = false;
			}

			float xoffset = xpos - mouse_lastX;
			float yoffset = mouse_lastY - ypos; // reversed since y-coordinates go from bottom to top

			mouse_lastX = xpos;
			mouse_lastY = ypos;

			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			rot += glm::vec3(yoffset, xoffset, 0);
		}
    };

    const std::function<void(GLFWwindow*, double, double)> scroll_control = [this](GLFWwindow* w, double xoff, double yoff) {
        if (fov >= 1.0f && fov <= 60.0f)
		    fov -= yoff;
        if (fov <= 1.0f)
            fov = 1.0f;
        if (fov >= 60.0f)
            fov = 60.0f;
    };

	// TEST: This isn't ideal and should be a bind or function in Input:: or something, but for now there isnt a great way
	// to get access to the window and the camera at the same time
	const std::function<void(GLFWwindow*, int, int)> mb_callback = [this](GLFWwindow* w, int button, int action) {
		if (glfwGetInputMode(w, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
				glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	};
	const std::function<void(GLFWwindow*, int, int)> key_callback = [this](GLFWwindow* w, int key, int action) {
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			last_focus = true;
		}
	};
};