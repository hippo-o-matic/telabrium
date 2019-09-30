#include "luarium/input.h"

double Input::mouse_lastX = 0;
double Input::mouse_lastY = 0;

Input* Input::activeInput = nullptr;


Input::Input(const char* path) {
    loadConfigFile(path);
}

void Input::loadConfigFile(const char* path) {

}

void Input::loadConfig(Json::Value& json) {
    Json::Value b = json["keybinds"];

    // Iterate through the array of keybind assignments
    for(unsigned i = 0; i < b.size(); i++) {
        std::string name = b[i][0].asString();
        auto name_it = std::find_if(binds.begin(), binds.end(), [name](bind b){ return b.name == name; }); // Find the bind by name
        if(name_it != binds.end())
            binds[i].key = b[i][1].asInt(); // Set the key stored in json (b[i][1]) to the appropriate bind in the vector
    }
}

void Input::saveConfig(Json::Value& json) {
    for (unsigned i = 0; i < binds.size(); i++) {
        json["keybinds"][i][0] = binds.at(i).name;
        json["keybinds"][i][1] = binds.at(i).key;
    }
}


void Input::addBind(const char* name, std::function<void()> func, int key, int action) {
    if(key == GLFW_KEY_UNKNOWN)
        LuariumLog("Can't add a bind to an unknown key", 2); return;

    // If the bind doesn't already exist, push a new one to the binds vector
    if(std::find_if(binds.begin(), binds.end(), [name](bind b){ return b.name == name; }) != binds.end()) { // Find the bind by name
        binds.push_back({name, func, key, action,});
    } else {
        LuariumLog("A bind with the id \"" + std::string(name) + "\" already exists", 2);
    }
        
}

void Input::removeBind(const char* name) {
    binds.erase(std::find_if(binds.begin(), binds.end(), [name](bind b){ return b.name == name; })); // Find the bind by name
}

void Input::removeBind(int key) {
    for(auto it = binds.begin(); it != binds.end(); it++) {
        if(it->key == key)
            binds.erase(it);
    }
}


void Input::editBindFunc(const char* name, std::function<void()> func) {
    auto it = std::find_if(binds.begin(), binds.end(), [name](bind b){ return b.name == name; }); // Find the bind by name
    if(it != binds.end()) {
        it->func = func;
    } else {
        LuariumLog("Can't find bind \"" + std::string(name) + "\" to edit", 1);
    }
}

void Input::editBindKey(const char* name, int key, int action) {
    auto it = std::find_if(binds.begin(), binds.end(), [name](bind b){ return b.name == name; }); // Find the bind by name
    if(it != binds.end()) {
        it->key = key; // Assign values if it does
        it->action = (action == -2) ? it->action : action; // If the action is the default (-2), leave it as the same
    } else {
        LuariumLog("Can't find bind \"" + std::string(name) + "\" to edit", 1);
    }
}


void setMouseCallback(std::function<void(GLFWwindow*, double, double)>) {

}

void setScrollCallback(std::function<void(GLFWwindow*, double, double)>) {

}



void Input::process(GLFWwindow* win) {
    for(auto it = binds.begin(); it != binds.end(); it++) {
        if (glfwGetKey(win, it->key) == it->action)
            it->func();
    }
}

void Input::setActiveInput(Input* in) {
    activeInput = in;
}


void Input::key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    for(auto it = activeInput->binds.begin(); it != activeInput->binds.end(); it++) {
        if(it->key == key && it->action == action)
            it->func();
    }
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

    for(auto it = activeInput->binds.begin(); it != activeInput->binds.end(); it++) {
        if(it->key == button && it->action == action)
            it->func();
    }
}

void Input::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // if (firstMouse)	{
	// 	lastX = xpos;
	// 	lastY = ypos;
	// 	firstMouse = false;
	// }

	float xoffset = xpos - mouse_lastX;
	float yoffset = mouse_lastY - ypos; // reversed since y-coordinates go from bottom to top

	mouse_lastX = xpos;
	mouse_lastY = ypos;

    activeInput->mouse_c(window, xoffset, yoffset);
}

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}
