#pragma once

#include <map>
#include <functional>

#include "GLFW/glfw3.h"

namespace Input {
    struct bind {
        int key;
        int mode;
        std::function<void(GLFWwindow *window)> func;
    };

    extern std::vector<bind> binds;

    void addBind(int, std::function<void(GLFWwindow *window)>, int mode = GLFW_PRESS);
    void removeBind(int);
    void process(GLFWwindow*);
}