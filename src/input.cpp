#include "luarium/input.h"

std::vector<Input::bind> Input::binds;

void Input::addBind(int key, std::function<void(GLFWwindow *window)> func, int mode) {
    binds.push_back({key, mode, func});
}

void Input::removeBind(int key) {
    binds.erase(std::find_if(binds.begin(), binds.end(), [key](bind b){ return b.key == key; }));
}

void Input::process(GLFWwindow* win) {
    for (auto it = binds.begin(); it != binds.end(); it++) {
        if (glfwGetKey(win, it->key) == it->mode)
            it->func(win);
    }
}