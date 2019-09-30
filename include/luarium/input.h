#pragma once

#include <map>
#include <functional>

#include "GLFW/glfw3.h"
#include "json/json.h"

#include "luarium/utility.h"

// A class for setting up input binds, assigning keys to them, and polling those keys
class Input {
public:
    struct bind {
        const char* name;
        std::function<void()> func;
        int key;
        int action;
    };

    Input(const char* path = "");

    /*  loadConfig(Json::Value& j) 
        Loads a json configuration from (j).

        The configuration file should be organized as such:
        "keybinds" : [
            ["bind_name", <int representing a GLFW_KEY]
        ],
        "mousebinds" : [
            ["bind_name", ]
        ]
    */
    void loadConfig(Json::Value& j);  
    void loadConfigFile(const char* path);

    void saveConfig(Json::Value&); // Outputs
    void saveConfigFile(const char* path);

    void addBind(const char* name, std::function<void()> func, int key = -2, int action = GLFW_PRESS); // Adds a keybind to run func() when pressed
    void removeBind(const char* name); // Removes the bind (name)
    void removeBind(int key); // Removes all binds from (key)
    void editBindFunc(const char* name, std::function<void()> func);
    void editBindKey(const char* name, int key, int action = -2);

    void setMouseCallback(std::function<void(GLFWwindow*, double, double)>);
    void setScrollCallback(std::function<void(GLFWwindow*, double, double)>);

    void process(GLFWwindow*);

    static void setActiveInput(Input*);

    static void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
    static double mouse_lastX;
    static double mouse_lastY;

    std::vector<bind> binds;
    std::function<void(GLFWwindow*, double, double)> mouse_c;
    std::function<void(GLFWwindow*, double, double)> scroll_c;

    static Input* activeInput;
}; 