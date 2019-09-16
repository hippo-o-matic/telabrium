#include "luarium/light.h"

// Static initialization
Task<DirLight, Shader&> DirLight::updateT(&DirLight::updateOF, &DirLight::updateTF);
int DirLight::nidStep = 0;

Task<PointLight, Shader&> PointLight::updateT(&PointLight::updateOF, &PointLight::updateTF);
int PointLight::nidStep = 0;

Task<SpotLight, Shader&> SpotLight::updateT(&SpotLight::updateOF, &SpotLight::updateTF);
int SpotLight::nidStep = 0;

Light::Light(){};

void Light::jload(Json::Value j) {
	this->Object::jload(j);

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
	nid = nidStep++;
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
	std::string s_nid = std::to_string(nid);
	s.set("dirLights[" + s_nid + "].direction", glm::radians(rot));
	s.set("dirLights[" + s_nid + "].ambient", Ambient);
	s.set("dirLights[" + s_nid + "].diffuse", Diffuse);
	s.set("dirLights[" + s_nid + "].specular", Specular);
}

void DirLight::jload(Json::Value j) {
	this->Light::jload(j);
}


// Point Light: Creates a spherical light originating from a single point
PointLight::PointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec, float constant, float lin, float quad){
	pos = pos; Ambient = amb; Diffuse = dif; Specular = spec;
	Constant = constant; Linear = lin; Quadratic = quad;
	nid = nidStep++;
	updateT.addObj(this);
}

PointLight::~PointLight() {
	updateT.removeObj(this);
}

void PointLight::jload(Json::Value j) {
	this->Light::jload(j);

	this->Constant = j.get("constant", 1.0).asFloat();
	this->Linear = j.get("linear", 0.9f).asFloat();
	this->Quadratic = j.get("quadratic", 0.032f).asFloat();
}

void PointLight::updateTF(Shader& s) {
	s.use();
	s.set("pointLight_AMT", (int) updateT.getObjCount()); // Tell the shader how many lights to expect
}

void PointLight::updateOF(Shader& s) {
	std::string s_nid = std::to_string(nid);
	s.set("pointLights[" + s_nid + "].pos", pos);
	s.set("pointLights[" + s_nid + "].ambient", Ambient);
	s.set("pointLights[" + s_nid + "].diffuse", Diffuse);
	s.set("pointLights[" + s_nid + "].specular", Specular);
	s.set("pointLights[" + s_nid + "].constant", Constant);
	s.set("pointLights[" + s_nid + "].linear", Linear);
	s.set("pointLights[" + s_nid + "].quadratic", Quadratic);
}


// Spot Light: Creates a conic light originating from a single point
SpotLight::SpotLight(glm::vec3 pos, glm::vec3 rot, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec, float constant, float lin, float quad, float cut, float ocut){
	pos = pos; rot = rot; 
	Ambient = amb; Diffuse = dif; Specular = spec;
	Constant = constant; Linear = lin; Quadratic = quad;
	CutOff = cut; OuterCutOff = ocut;

	nid = nidStep++;
	updateT.addObj(this);
}

SpotLight::~SpotLight() {
	updateT.removeObj(this);
}

void SpotLight::jload(Json::Value j) {
	this->Light::jload(j);

	this->Constant = j.get("constant", 1).asFloat();
	this->Linear = j.get("linear", 0.9f).asFloat();
	this->Quadratic = j.get("quadratic", 0.032f).asFloat();

	this->CutOff = j.get("cutoff", 12.5f).asFloat();
	this->OuterCutOff = j.get("outercutoff", 15).asFloat();
}

void SpotLight::updateTF(Shader& s) {
	s.use();
	s.set("spotLight_AMT", (int) updateT.getObjCount()); // Tell the shader how many lights to expect
}

void SpotLight::updateOF(Shader& s) {
	std::string s_nid = std::to_string(nid);
	s.set("spotLights[" + s_nid + "].pos", pos);
	s.set("spotLights[" + s_nid + "].direction", glm::radians(rot));
	s.set("spotLights[" + s_nid + "].ambient", Ambient);
	s.set("spotLights[" + s_nid + "].diffuse", Diffuse);
	s.set("spotLights[" + s_nid + "].specular", Specular);
	s.set("spotLights[" + s_nid + "].constant", Constant);
	s.set("spotLights[" + s_nid + "].linear", Linear);
	s.set("spotLights[" + s_nid + "].quadratic", Quadratic);
	s.set("spotLights[" + s_nid + "].cutOff", glm::cos(glm::radians(CutOff)));
	s.set("spotLights[" + s_nid + "].outerCutOff", glm::cos(glm::radians(OuterCutOff)));
}


void Light::updateLights(Shader& s) {
	DirLight::updateT.exec(s);
	PointLight::updateT.exec(s);
	SpotLight::updateT.exec(s);
}