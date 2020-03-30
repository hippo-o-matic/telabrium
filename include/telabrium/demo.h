#pragma once

//#define _GLIBCXX_USE_CXX11_ABI 0

#include <iostream> //Console Stream
#include <unistd.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "telabrium/shader.h"
#include "telabrium/camera.h"
#include "telabrium/model.h"
#include "telabrium/light.h"
//#include "telabrium/text.h"
//#include "telabrium/rigidbody.h"
#include "telabrium/luafile.h"
#include "telabrium/utility.h"
#include "telabrium/skybox.h"
#include "telabrium/input.h"
#include "telabrium/level.h"
#include "telabrium/generic/freecam.h"

#define TELABRIUM_MODE_DEBUG

const char* SHADER_PATH_V = "shaders/Standard.vert";
const char* SHADER_PATH_F = "shaders/Standard.frag";

//0-Game is not running; 1-Game is running
int gameState;

GLFWwindow* window;

//unsigned int* Skybox::env_map = nullptr;

GLFWwindow* init_main_window();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void setupInputBinds();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void cleanup();

void simpleConsole();

extern Level* test;