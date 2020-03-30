#pragma once

#include <sstream>
#include <vector>
#include <memory>

#define GLM_FORCE_PURE
#include <glm/vec3.hpp>

#include "telabrium/object.h"
#include "telabrium/shader.h"
#include "telabrium/model.h"

class Light : public Object {
public:
	Light(glm::vec3 amb = default_amb, glm::vec3 dif = default_dif, glm::vec3 spec = default_spec);
	Light(Json::Value);

	Shader* shader;
	glm::mat4 model;

	glm::vec3 Ambient; 
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	// Gather all the lights and send them to the shader
	static void updateLights(Shader& shader);

protected:
	static constexpr glm::vec3 default_amb = glm::vec3(0);
	static constexpr glm::vec3 default_dif = glm::vec3(0.8);
	static constexpr glm::vec3 default_spec = glm::vec3(1);
};

// Directional Light: Provides light from one direction
class DirLight : public Light {
public:
	DirLight(
		glm::vec3 dir = default_rot,
		glm::vec3 amb = default_amb,
		glm::vec3 dif = default_dif,
		glm::vec3 spec = default_spec);

	DirLight(const DirLight &obj);
	DirLight(Json::Value);

	~DirLight();

	int nid = 0; // Numeric id for the shader

	static Task<DirLight, Shader&> updateT; // Task for sending light data to the shader
	static void updateTF(Shader&); // Task function for updateT
	void updateOF(Shader&); // Object function for updateT	

private:
	TELABRIUM_REGISTER_OBJECT(DirLight);
};

// Point Light: Creates a spherical light originating from a single point
class PointLight : public Light {
public:	
	PointLight(
		glm::vec3 pos = default_pos,
		glm::vec3 amb = default_amb,
		glm::vec3 dif = default_dif,
		glm::vec3 spec = default_spec,
		float constant = default_constant,
		float lin = default_lin,
		float quad = default_quad);

	PointLight(const PointLight &obj);
	PointLight(Json::Value);

	~PointLight();

	float Constant;
	float Linear;
	float Quadratic;

	static Task<PointLight, Shader&> updateT; // Task for sending light data to the shader
	static void updateTF(Shader& s); // Task function for updateT
	void updateOF(Shader&); // Object function for updateT

protected:
	static constexpr float default_constant = 1;
	static constexpr float default_lin = 0.9;
	static constexpr float default_quad = 0.032;

private:
	TELABRIUM_REGISTER_OBJECT(PointLight);
};

// Spotlight: Creates a conic light originating from a single point
class SpotLight : public Light {
public:
	SpotLight(
		glm::vec3 pos = default_pos,
		glm::vec3 dir = default_rot,
		glm::vec3 amb = default_amb,
		glm::vec3 dif = default_dif,
		glm::vec3 spec = default_spec,
		float constant = default_constant,
		float lin = default_lin,
		float quad = default_quad,
		float cut = default_cut,
		float ocut = default_ocut
	);

	SpotLight(const SpotLight &obj);
	SpotLight(Json::Value);

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

protected:
	static constexpr float default_constant = 1;
	static constexpr float default_lin = 0.9;
	static constexpr float default_quad = 0.032;
	static constexpr float default_cut = 12.5;
	static constexpr float default_ocut = 15;

private:
	TELABRIUM_REGISTER_OBJECT(SpotLight);
};