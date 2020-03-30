#include "telabrium/light.h"

// Static initialization
Task<DirLight, Shader&> DirLight::updateT(&DirLight::updateOF, &DirLight::updateTF);

Task<PointLight, Shader&> PointLight::updateT(&PointLight::updateOF, &PointLight::updateTF);

Task<SpotLight, Shader&> SpotLight::updateT(&SpotLight::updateOF, &SpotLight::updateTF);

Light::Light(glm::vec3 amb, glm::vec3 dif, glm::vec3 spec) {
	Ambient = amb; Diffuse = dif; Specular = spec;
};

Light::Light(Json::Value j) : Object(j) {
	Json::Value jAmbient = j["ambient"];
	Json::Value jDiffuse = j["diffuse"];
	Json::Value jSpecular = j["specular"];

	this->Ambient = glm::vec3(jAmbient[0].asFloat(), jAmbient[1].asFloat(), jAmbient[2].asFloat());
	this->Diffuse = glm::vec3(jDiffuse[0].asFloat(), jDiffuse[1].asFloat(), jDiffuse[2].asFloat());
	this->Specular = glm::vec3(jSpecular[0].asFloat(), jSpecular[1].asFloat(), jSpecular[2].asFloat());
}

// Directional Light: Provides light from one direction
DirLight::DirLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec) {
	rot = dir; Ambient = amb; Diffuse = dif; Specular = spec;
	updateT.addObj(this);
}

DirLight::DirLight(Json::Value j) : Light(j) {
	updateT.addObj(this);
}

DirLight::~DirLight() {
	updateT.removeObj(this);
}

void DirLight::updateTF(Shader& s) {
	s.use();
	s.set("dirLight_AMT", (int) updateT.getObjCount()); // Tell the shader how many lights to expect
}

void DirLight::updateOF(Shader& s) {
	auto& lights = updateT.task_objects;
	unsigned index = std::find(lights.begin(), lights.end(), this) - lights.begin();
	std::string i = std::to_string(index);

	s.set("dirLights[" + i + "].direction", glm::radians(rot()));
	s.set("dirLights[" + i + "].ambient", Ambient);
	s.set("dirLights[" + i + "].diffuse", Diffuse);
	s.set("dirLights[" + i + "].specular", Specular);
}


// Point Light: Creates a spherical light originating from a single point
PointLight::PointLight(glm::vec3 position, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec, float constant, float lin, float quad) : Light(){
	pos = position; Ambient = amb; Diffuse = dif; Specular = spec;
	Constant = constant; Linear = lin; Quadratic = quad;

	updateT.addObj(this);
}

PointLight::PointLight(Json::Value j) : Light(j) {
	this->Constant = j.get("constant", default_constant).asFloat();
	this->Linear = j.get("linear", default_lin).asFloat();
	this->Quadratic = j.get("quadratic", default_quad).asFloat();

	updateT.addObj(this);
}

PointLight::~PointLight() {
	updateT.removeObj(this);
}

void PointLight::updateTF(Shader& s) {
	s.use();
	s.set("pointLight_AMT", (int) updateT.getObjCount()); // Tell the shader how many lights to expect
}

void PointLight::updateOF(Shader& s) {
	auto& lights = updateT.task_objects;
	unsigned index = std::find(lights.begin(), lights.end(), this) - lights.begin();
	std::string i = std::to_string(index);

	s.set("pointLights[" + i + "].pos", pos());
	s.set("pointLights[" + i + "].ambient", Ambient);
	s.set("pointLights[" + i + "].diffuse", Diffuse);
	s.set("pointLights[" + i + "].specular", Specular);
	s.set("pointLights[" + i + "].constant", Constant);
	s.set("pointLights[" + i + "].linear", Linear);
	s.set("pointLights[" + i + "].quadratic", Quadratic);
}


// Spot Light: Creates a conic light originating from a single point
SpotLight::SpotLight(glm::vec3 position, glm::vec3 rotation, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec, float constant, float lin, float quad, float cut, float ocut){
	pos = position; rot = rotation; 
	Ambient = amb; Diffuse = dif; Specular = spec;
	Constant = constant; Linear = lin; Quadratic = quad;
	CutOff = cut; OuterCutOff = ocut;

	updateT.addObj(this);
}

SpotLight::SpotLight(Json::Value j) : Light(j) {
	this->Constant = j.get("constant", default_constant).asFloat();
	this->Linear = j.get("linear", default_lin).asFloat();
	this->Quadratic = j.get("quadratic", default_quad).asFloat();

	this->CutOff = j.get("cutoff", default_cut).asFloat();
	this->OuterCutOff = j.get("outercutoff", default_ocut).asFloat();

	updateT.addObj(this);
}

SpotLight::~SpotLight() {
	updateT.removeObj(this);
}

void SpotLight::updateTF(Shader& s) {
	s.use();
	s.set("spotLight_AMT", (int) updateT.getObjCount()); // Tell the shader how many lights to expect
}

void SpotLight::updateOF(Shader& s) {
	auto& lights = updateT.task_objects;
	unsigned index = std::find(lights.begin(), lights.end(), this) - lights.begin();
	std::string i = std::to_string(index);

	s.set("spotLights[" + i + "].pos", pos());
	s.set("spotLights[" + i + "].direction", glm::radians(rot()));
	s.set("spotLights[" + i + "].ambient", Ambient);
	s.set("spotLights[" + i + "].diffuse", Diffuse);
	s.set("spotLights[" + i + "].specular", Specular);
	s.set("spotLights[" + i + "].constant", Constant);
	s.set("spotLights[" + i + "].linear", Linear);
	s.set("spotLights[" + i + "].quadratic", Quadratic);
	s.set("spotLights[" + i + "].cutOff", glm::cos(glm::radians(CutOff)));
	s.set("spotLights[" + i + "].outerCutOff", glm::cos(glm::radians(OuterCutOff)));
}


void Light::updateLights(Shader& s) {
	DirLight::updateT.exec(s);
	PointLight::updateT.exec(s);
	SpotLight::updateT.exec(s);
}