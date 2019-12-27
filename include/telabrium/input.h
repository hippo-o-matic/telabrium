#pragma once

#include <map>
#include <functional>

#include "GLFW/glfw3.h"
#include "json/json.h"

#include "telabrium/utility.h"

// A class for setting up input binds, assigning keys to them, and polling those keys
class Input {
public:
    struct bind {
        const char* name;
        std::function<void()> func;
        int key;
        int action;
    };

    Input(std::string path = "");

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
    void loadConfigFile(std::string path);

    Json::Value saveConfig(); // Returns the binds of this profile in a json format
    void saveConfigFile(std::string path);

    void addBind(const char* name, std::function<void()> func, int key = -2, int action = GLFW_PRESS); // Adds a keybind to run func() when pressed
    void removeBind(const char* name); // Removes the bind (name)
    void removeBind(int key); // Removes all binds from (key)
    void editBindFunc(const char* name, std::function<void()> func);
    void editBindKey(const char* name, int key, int action = -2);

    void setMouseCallback(std::function<void(GLFWwindow*, double, double)>);
    void setScrollCallback(std::function<void(GLFWwindow*, double, double)>);

    void process(GLFWwindow*);
    static void processActive(GLFWwindow*);

    void activate();
    void deactivate();
    // void activate_solo();

    static void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
    std::vector<bind> binds; // The binds this profile will track
    std::function<void(GLFWwindow*, double, double)> mouse_c; // The actions performed by the mouse for this profile 
    std::function<void(GLFWwindow*, double, double)> scroll_c; // The actions performed by the scroll wheel for this profile

    static std::vector<Input*> active_profiles; // The list of profiles to process


    const char* config_file_prefix = (TELABRIUM_CONFIG_PATH + "controls/").c_str();
    std::string config_file_path = "default";
}; 