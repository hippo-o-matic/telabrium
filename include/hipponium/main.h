#ifndef HIPPO_MAIN_H
#define HIPPO_MAIN_H

//#define _GLIBCXX_USE_CXX11_ABI 0

#include <iostream>
#include <unistd.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "hipponium/shader.h"
#include "hipponium/camera.h"
#include "hipponium/model.h"
#include "hipponium/light.h"
//#include "hipponium/text.h"
#include "hipponium/generic.h"
//#include "hipponium/rigidbody.h"
#include "hipponium/luafile.h"

const char* WORKING_DIRECTORY = "tests/";
//0-Game is not running; 1-Game is running
int gameState;

GLFWwindow* window;

//unsigned int* Skybox::env_map = nullptr;
Camera* Camera::ACTIVE = nullptr;

void processInput(GLFWwindow *window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif // HIPPO_MAIN_H