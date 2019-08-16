#ifndef LIGHT_H
#define LIGHT_H

#include <sstream>
#include <vector>
#include <memory>

#include "luarium/object.h"
#include "luarium/shader.h"
#include "luarium/model.h"

class Light : public Object {
public:
	Light();

	Shader* shader;
	glm::mat4 model;

	glm::vec3 Ambient; 
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	// Gather all the lights and send them to the shader
	static void updateLights(Shader &shader);
	
protected:
	void jload(Json::Value j);
};

// Directional Light: Provides light from one direction
class DirLight : public Light {
public:
	DirLight(
		glm::vec3 dir = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 amb = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 dif = glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3 spec = glm::vec3(1.0f, 1.0f, 1.0f));

	DirLight(const DirLight &obj);

	~DirLight();

	static std::vector<DirLight*> list; // The vector that contains all the lights of this class, for shader processing
	int id = 0;

	void jload(Json::Value j);

private:
	static int idStep; // Remember where we left off when assigning id's
};

// Point Light: Creates a spherical light originating from a single point
class PointLight : public Light {
public:	
	PointLight(
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 amb = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 dif = glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3 spec = glm::vec3(1.0f, 1.0f, 1.0f),
		float constant = 1.0f,
		float lin = 0.9f,
		float quad = 0.032f);

	PointLight(const PointLight &obj);

	~PointLight();

	float Constant;
	float Linear;
	float Quadratic;

	static std::vector<PointLight*> list;
	int id = 0;

	void jload(Json::Value j);

private:
	static int idStep;
};

// Spotlight: Creates a conic light originating from a single point
class SpotLight : public Light {
public:
	SpotLight(
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 amb = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 dif = glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3 spec = glm::vec3(1.0f, 1.0f, 1.0f),
		float constant = 1.0f,
		float lin = 0.9f,
		float quad = 0.032f,
		float cut = 12.5f,
		float ocut = 15.0f
	);

	SpotLight(const SpotLight &obj);

	~SpotLight();

	float Constant;
	float Linear;
	float Quadratic;
	float CutOff; // The start of the cutoff fade, in degrees
	float OuterCutOff; // The edge of the cutoff fade, in degrees

	static std::vector<SpotLight*> list;
	int id;

	void jload(Json::Value j);

protected:
	static std::function<Object::ptr()> luarium_obj_create;
    std::function<void(Json::Value)> value_f;
    static bool luarium_obj_reg;

private:
	static int idStep;
}; 

#endif
