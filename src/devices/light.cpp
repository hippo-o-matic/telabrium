#include "luarium/light.h"

Light::Light(){};

// Directional Light: Provides light from one direction
DirLight::DirLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec){
	Rotation = dir; Ambient = amb; Diffuse = dif; Specular = spec;
	id = idStep++;
	list.push_back(this);
}

DirLight::~DirLight() {
	list[id] = NULL; //Remove refrence from the list so the shader doesn't recieve outdated or garbage data
}

std::vector<DirLight*> DirLight::list; // Declare the list and idStep out here so it becomes static fully
int DirLight::idStep = 0;


// Point Light: Creates a spherical light originating from a single point
PointLight::PointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec, float constant, float lin, float quad){
	Position = pos; Ambient = amb; Diffuse = dif; Specular = spec;
	Constant = constant; Linear = lin; Quadratic = quad;
	id = idStep++;
	list.push_back(this);
//	Model temp("models/cube.obj");
//	bulb = &temp;
}

PointLight::~PointLight() {
	list[id] = NULL;
}

std::vector<PointLight*> PointLight::list;
int PointLight::idStep = 0;


// Spot Light: Creates a conic light originating from a single point
SpotLight::SpotLight(glm::vec3 pos, glm::vec3 rot, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec, float constant, float lin, float quad, float cut, float ocut){
	Position = pos; Rotation = rot; 
	Ambient = amb; Diffuse = dif; Specular = spec;
	Constant = constant; Linear = lin; Quadratic = quad;
	CutOff = cut; OuterCutOff = ocut;

	id = idStep++;
	list.push_back(this);
}

SpotLight::~SpotLight() {
	list[id] = NULL;
}

std::vector<SpotLight*> SpotLight::list;
int SpotLight::idStep = 0;


// Gather all the lights and send them to the shader
void updateLights(Shader &shader) {
	shader.set("dirLight_AMT", (int) DirLight::list.size()); //Tell the shader how many lights to expect
	shader.set("pointLight_AMT", (int) PointLight::list.size());
	shader.set("spotLight_AMT", (int) SpotLight::list.size());

	//Go through each light in the vector and set the attributes to its corresponding id in the shader's array of lights
	for (auto& e : SpotLight::list) { 
		std::string id = std::to_string(e->id);
		shader.set("spotLights[" + id + "].position", e->Position);
		shader.set("spotLights[" + id + "].direction", glm::radians(e->Rotation));
		shader.set("spotLights[" + id + "].ambient", e->Ambient);
		shader.set("spotLights[" + id + "].diffuse", e->Diffuse);
		shader.set("spotLights[" + id + "].specular", e->Specular);
		shader.set("spotLights[" + id + "].constant", e->Constant);
		shader.set("spotLights[" + id + "].linear", e->Linear);
		shader.set("spotLights[" + id + "].quadratic", e->Quadratic);
		shader.set("spotLights[" + id + "].cutOff", glm::cos(glm::radians(e->CutOff)));
		shader.set("spotLights[" + id + "].outerCutOff", glm::cos(glm::radians(e->OuterCutOff)));
	}
	for (auto& e : DirLight::list) {
		std::string id = std::to_string(e->id);
		shader.set("dirLights[" + id + "].direction", glm::radians(e->Rotation));
		shader.set("dirLights[" + id + "].ambient", e->Ambient);
		shader.set("dirLights[" + id + "].diffuse", e->Diffuse);
		shader.set("dirLights[" + id + "].specular", e->Specular);
	}
	for (auto& e : PointLight::list) {
		std::string id = std::to_string(e->id);
		shader.set("pointLights[" + id + "].position", e->Position);
		shader.set("pointLights[" + id + "].ambient", e->Ambient);
		shader.set("pointLights[" + id + "].diffuse", e->Diffuse);
		shader.set("pointLights[" + id + "].specular", e->Specular);
		shader.set("pointLights[" + id + "].constant", e->Constant);
		shader.set("pointLights[" + id + "].linear", e->Linear);
		shader.set("pointLights[" + id + "].quadratic", e->Quadratic);

		// Update the model for the light
		e->bulb->Position = e->Position;
		e->bulb->Rotation = e->Rotation;
		e->bulb->Scale = e->Scale;
//		e->bulb->Draw(shader);
	}
} 