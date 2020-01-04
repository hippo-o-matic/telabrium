#pragma once

#include "telabrium/camera.h"
#include "telabrium/input.h"

class FreeCam : public Camera {
public:
    FreeCam(float &deltaTime, glm::vec3 position = glm::vec3(0));

    float Speed = 2.5;
	float MouseSensitivity = 0.1;

private:
    Input* controls;

    static double mouse_lastX;
    static double mouse_lastY;

    const std::function<void(GLFWwindow*, double, double)> mouse_control = [this](GLFWwindow* w, double xpos, double ypos) {
        float xoffset = xpos - mouse_lastX;
        float yoffset = mouse_lastY - ypos; // reversed since y-coordinates go from bottom to top

        mouse_lastX = xpos;
        mouse_lastY = ypos;

        xoffset *= MouseSensitivity;
	    yoffset *= MouseSensitivity;
        glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        setRot(getRot() + glm::vec3(yoffset, xoffset, 0));

        if (getRot().x > 89.0f)
            setRot(glm::vec3(89, getRot().y, getRot().z)); // Oh god we need a better interface than this. Hopefully we figure out how to do setRot.x() or something in the future. This is bad
        if (getRot().x < -89.0f)
            setRot(glm::vec3(-89, getRot().y, getRot().z));

        // Update Front, Right and Up Vectors using the updated Eular angles
        updateCameraVectors();
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