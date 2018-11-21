#ifndef LUARIUM_MAIN_H
#define LUARIUM_MAIN_H

//#define _GLIBCXX_USE_CXX11_ABI 0

#include <iostream> //Console Stream
#include <unistd.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "luarium/core/shader.h"
#include "luarium/core/camera.h"
#include "luarium/core/model.h"
#include "luarium/core/light.h"
//#include "luarium/core/text.h"
//#include "luarium/core/rigidbody.h"
#include "luarium/core/luafile.h"
#include "luarium/core/generic.h"
#include "luarium/devices/skybox.h"


const char* SHADER_PATH_V = "shaders/Standard.vs";
const char* SHADER_PATH_F = "shaders/Standard.fs";

//0-Game is not running; 1-Game is running
int gameState;

GLFWwindow* window;

//unsigned int* Skybox::env_map = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void processInput(GLFWwindow *window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void simpleConsole();

#endif // LUARIUM_MAIN_H