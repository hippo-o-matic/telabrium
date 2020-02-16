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

    static double mouse_lastX;
    static double mouse_lastY;

    const std::function<void(GLFWwindow*, double, double)> mouse_control = [this](GLFWwindow* w, double xpos, double ypos) {
        float xoffset = xpos - mouse_lastX;
        float yoffset = mouse_lastY - ypos; // reversed since y-coordinates go from bottom to top

        mouse_lastX = xpos;
        mouse_lastY = ypos;

        xoffset *= MouseSensitivity;
	    yoffset *= MouseSensitivity;

        rot += glm::vec3(yoffset, xoffset, 0);
    };

    const std::function<void(GLFWwindow*, double, double)> scroll_control = [this](GLFWwindow* w, double xoff, double yoff) {
        if (fov >= 1.0f && fov <= 60.0f)
		    fov -= yoff;
        if (fov <= 1.0f)
            fov = 1.0f;
        if (fov >= 60.0f)
            fov = 60.0f;
    };
};