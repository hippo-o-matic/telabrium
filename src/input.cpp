#include "telabrium/input.h"

std::vector<Input*> Input::active_profiles;

Input::Input(std::string path) {
    loadConfigFile(path);
 
}

Input::Input() {
}

void Input::loadConfigFile(std::string path) {
    std::ifstream file(path);
    Json::Value j;
    file >> j;
    loadConfig(j);
}

void Input::loadConfig(Json::Value& json) {
    Json::Value b = json["keybinds"];

    // Iterate through the array of keybind assignments
    for(unsigned i = 0; i < b.size(); i++) {
        std::string name = b[i][0].asString();
        auto name_it = std::find_if(binds.begin(), binds.end(), [name](bind b){ return b.name == name; }); // Find the bind by name
        if(name_it != binds.end()) {
            binds[i].key = b[i][1].asInt(); // Set the key stored in json (b[i][1]) to the appropriate bind in the vector
            binds[i].action = b[i][2].asInt();
        }
    }
}

void Input::saveConfigFile(std::string path){
    std::ofstream file(path);
    file << saveConfig();
}

Json::Value Input::saveConfig() {
    Json::Value json;
    for (unsigned i = 0; i < binds.size(); i++) {
        json["keybinds"][i][0] = binds.at(i).name;
        json["keybinds"][i][1] = binds.at(i).key;
        json["keybinds"][i][2] = binds.at(i).action;
    }

    return json;
}


void Input::addBind(const char* name, std::function<void()> func, int key, int action) {
    if(key == GLFW_KEY_UNKNOWN) {
        TelabriumLog("Can't add a bind to an unknown key", 2); return;
    }

    // If the bind doesn't already exist, push a new one to the binds vector
    if(std::find_if(binds.begin(), binds.end(), [name](bind b){ return b.name == name; }) == binds.end()) { // Find the bind by name
        binds.push_back({name, func, key, action});
    } else {
        TelabriumLog("A bind with the id \"" + std::string(name) + "\" already exists", 2);
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
        TelabriumLog("Can't find bind \"" + std::string(name) + "\" to edit", 1);
    }
}

void Input::editBindKey(const char* name, int key, int action) {
    auto it = std::find_if(binds.begin(), binds.end(), [name](bind b){ return b.name == name; }); // Find the bind by name
    if(it != binds.end()) {
        it->key = key; // Assign values if it does
        it->action = (action == -2) ? it->action : action; // If the action is the default (-2), leave it as the same
    } else {
        TelabriumLog("Can't find bind \"" + std::string(name) + "\" to edit", 1);
    }
}


void Input::setMouseCallback(std::function<void(GLFWwindow*, double, double)> func) {
    mouse_c = func;
}

void Input::setScrollCallback(std::function<void(GLFWwindow*, double, double)> func) {
    scroll_c = func;
}


// Check each bind and run its function
void Input::process(GLFWwindow* win) {
    for(auto bt = binds.begin(); bt != binds.end(); bt++) {
        if (glfwGetKey(win, bt->key) == bt->action)
            bt->func();
    }
}

// Check for input for all active profiles
void Input::processActive(GLFWwindow* win) {
    for(auto it = active_profiles.begin(); it != active_profiles.end(); it++) { // Iterate through all active profiles
        (*it)->process(win);
    }
}


// Start tracking inputs from this profile
void Input::activate() {
    active_profiles.push_back(this);
}

// Stop tracking inputs from this profile
void Input::deactivate() {
    active_profiles.erase(std::find(active_profiles.begin(), active_profiles.end(), this));
}

// Ignore all other profiles while this one is active
// void Input::activate_solo() {
//     active_profiles.clear();
//     active_profiles.push_back(this);
// }

// Callbacks to be passed to GLFW
/////////////////////////////////

// NOTE: keys and mouse buttons should be handled with Input::process, as this callback doesn't continue running when keys are held
void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
    // for(auto it = active_profiles.begin(); it != active_profiles.end(); it++) { // Iterate through all active profiles
    //     for(auto bt = (*it)->binds.begin(); bt != (*it)->binds.end(); bt++) { // Iterate through that profiles binds
    //         if(bt->key == key && bt->action == action)
    //             bt->func();
    //     }
    // }
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // Focus the window on click
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
    
    // for(auto it = active_profiles.begin(); it != active_profiles.end(); it++) { // Iterate through all active profiles
    //     for(auto bt = (*it)->binds.begin(); bt != (*it)->binds.end(); bt++) { // Iterate through that profiles binds
    //         if(bt->key == button && bt->action == action)
    //             bt->func();
    //     }
    // }
}

void Input::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) { // Only run mouse events if the window is focused
        for(auto it = active_profiles.begin(); it != active_profiles.end(); it++) { // Iterate through all active profiles
            if((*it)->mouse_c)
                (*it)->mouse_c(window, xpos, ypos);
        }
    }
}

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) { // Only run mouse events if the window is focused
        for(auto it = active_profiles.begin(); it != active_profiles.end(); it++) { // Iterate through all active profiles
            if((*it)->scroll_c)
                (*it)->scroll_c(window, xoffset, yoffset);
        }
    }
}
