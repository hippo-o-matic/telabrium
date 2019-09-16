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
	static void updateLights(Shader& shader);

	virtual void jload(Json::Value j);

private:
	LUARIUM_REGISTER_OBJECT(Light);
};

// Directional Light: Provides light from one direction
class DirLight : public Light {
public:
	DirLight(
		glm::vec3 dir = glm::vec3(0.0f),
		glm::vec3 amb = glm::vec3(0.0f),
		glm::vec3 dif = glm::vec3(0.8f),
		glm::vec3 spec = glm::vec3(1.0f));

	DirLight(const DirLight &obj);

	~DirLight();

	int nid = 0; // Numeric id for the shader

	static Task<DirLight, Shader&> updateT; // Task for sending light data to the shader
	static void updateTF(Shader&); // Task function for updateT
	void updateOF(Shader&); // Object function for updateT

	virtual void jload(Json::Value j);

private:
	LUARIUM_REGISTER_OBJECT(DirLight);

	static int nidStep; // Remember where we left off when assigning nid's
};

// Point Light: Creates a spherical light originating from a single point
class PointLight : public Light {
public:	
	PointLight(
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 amb = glm::vec3(0.0f),
		glm::vec3 dif = glm::vec3(0.8f),
		glm::vec3 spec = glm::vec3(1.0f),
		float constant = 1.0f,
		float lin = 0.9f,
		float quad = 0.032f);

	PointLight(const PointLight &obj);

	~PointLight();

	float Constant;
	float Linear;
	float Quadratic;

	int nid = 0;

	static Task<PointLight, Shader&> updateT; // Task for sending light data to the shader
	static void updateTF(Shader& s); // Task function for updateT
	void updateOF(Shader&); // Object function for updateT

	virtual void jload(Json::Value j);

private:
	LUARIUM_REGISTER_OBJECT(PointLight);

	static int nidStep;
};

// Spotlight: Creates a conic light originating from a single point
class SpotLight : public Light {
public:
	SpotLight(
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 rot = glm::vec3(0.0f),
		glm::vec3 amb = glm::vec3(0.0f),
		glm::vec3 dif = glm::vec3(0.8f),
		glm::vec3 spec = glm::vec3(1.0f),
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

	int nid;

	static Task<SpotLight, Shader&> updateT; // Task for sending light data to the shader
	static void updateTF(Shader& s); // Task function for updateT
	void updateOF(Shader&); // Object function for updateT

	virtual void jload(Json::Value j);

private:
	LUARIUM_REGISTER_OBJECT(SpotLight);

	static int nidStep;
}; 

#endif
