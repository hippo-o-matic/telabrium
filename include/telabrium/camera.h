#ifndef TELABRIUM_CAMERA_H
#define TELABRIUM_CAMERA_H

#include <vector>
#include <memory>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "telabrium/object3d.hpp"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
// enum Camera_Movement {
// 	FORWARD,
// 	BACKWARD,
// 	LEFT,
// 	RIGHT,
// 	UP,
// 	DOWN,
// };

// A Camera object that processes a view for displaying in OpenGL. 
class Camera : public Object3d {
public:
	// Camera Attributes
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// Camera options
	float fov = 60;
	unsigned int* display_height;
	unsigned int* display_width;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
	~Camera();
	
	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getOrthoMatrix();

	float getAspectRatio();

	static std::vector<Camera*> cameras;

protected:
	void updateCameraVectors();
};

#endif

