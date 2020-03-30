#pragma once

#include <map>
#include <functional>

#include "GLFW/glfw3.h"
#include "json/json.h"

#include "telabrium/utility.h"

// A class for setting up input binds, assigning keys to them, and remmaping those keys through config files
class Input {
public:
    struct bind {
        const char* name;
        std::function<void()> func;
        int key;
        int action;
    };

	Input();
    Input(std::string path);

    /*  loadConfig(Json::Value& j) 
        Loads a json configuration from (j).

        The configuration file should be organized as such:
        "keybinds" : [
            ["bind_name", <int representing a GLFW_KEY>]
        ],
        "mousebinds" : [
            ["bind_name", <int representing a GLFW_KEY>]
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

	void setKeyCallback(std::function<void(GLFWwindow*,int, int)>);
	void setMouseButtonCallback(std::function<void(GLFWwindow*,int, int)>);
    void setMouseCallback(std::function<void(GLFWwindow*, double, double)>); // The actions performed by the mouse for this profile
    void setScrollCallback(std::function<void(GLFWwindow*, double, double)>); // The actions performed by the scroll wheel for this profile
	void setFocusCallback(std::function<void(GLFWwindow*, bool)>); // The actions performed whenever the window gains or loses focus

    void process(GLFWwindow*);
    static void processActive(GLFWwindow*);



    void activate();
    void deactivate();
    // void activate_solo();

	static void assignCallbacks(GLFWwindow* window); // Shortcut to give our callback functions to glfw for <window>>

private:
    std::vector<bind> binds; // The binds this profile will track
	
	// Profile callback functions
	std::function<void(GLFWwindow*,int, int)> key_c; // Profile key callback
	std::function<void(GLFWwindow*,int, int)> button_c; // Profile mouse button callback
    std::function<void(GLFWwindow*, double, double)> mouse_c; // Profile mouse callback 
    std::function<void(GLFWwindow*, double, double)> scroll_c; // Profile scroll callback 
	std::function<void(GLFWwindow*, bool)> focus_c; // Profile focus callback

	// Callback functions to be handed off to GLFW
    static void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* win, int button, int action, int mods);
    static void mouse_callback(GLFWwindow* win, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);
	static void focus_callback(GLFWwindow* win, int focused);

	static bool focused; // Whether or not the window is focused

    static std::vector<Input*> active_profiles; // The list of profiles to process

    const char* config_file_prefix = (TELABRIUM_CONFIG_PATH + "controls/").c_str();
    std::string config_file_path = "default";
}; 