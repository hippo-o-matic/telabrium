#pragma once

#include "telabrium/shader.h"
#include "telabrium/task.h"

#include "json/json.h"

#include <map>

#define TELA_SHADER_UNIFORM_TYPES bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4

class Material {
public:
	static std::shared_ptr<Material> requestMaterial(std::string _path = DEFAULT_MATERIAL_PATH);
	
	Material();
	Material(std::string path);

	template<typename T>
	T modifyKey(std::string key, T val);

	template<typename T>
	T getKey(std::string key);

	void updateShader();

	std::function<void()> predraw_func; // A function called before a mesh is drawn, used to set specific gl parameters
	std::function<void()> postdraw_func; // A function called after a mesh is drawn, cleanup any predraw functions here

	std::string id;
	std::string path;

	// <key string, <item type as string, item> >
	typedef std::pair<std::string, std::variant<TELA_SHADER_UNIFORM_TYPES>> uniform_type;
	std::map<std::string, uniform_type> uniforms;

	std::shared_ptr<Shader> shader;

	static std::string DEFAULT_VERTEX_PATH;
	static std::string DEFAULT_FRAGMENT_PATH;
	static std::string DEFAULT_MATERIAL_PATH;

	static std::vector<std::shared_ptr<Material>> materials;

private:

	// item type strings are as follows: bool, int, float, vec2, vec3, vec4, mat2, mat3, mat4
	template<typename T>
	void addKey(std::string key, std::string type, T val);

	void parseMaterial(Json::Value j);
	void parseShader(Json::Value j);

	std::string material_name = "Material"; // The name of the interface block in the shader

};

// Template Definitions
// Add a key to the block that will be sent to the shader as a uniform, with value (val)
template<typename T>
void Material::addKey(std::string key, std::string type, T val) {
	if(uniforms.find(key) != uniforms.end()) {
		TelabriumLog("Key \"" + key + "\" already exists in block \"" + material_name + "\"", 1);
		return;
	}

	std::variant<TELA_SHADER_UNIFORM_TYPES> value = val;
	uniforms.insert(std::pair<std::string, uniform_type>(key, uniform_type(type, val)) );
}

// Change the value of (key) to (val)
template<typename T>
T Material::modifyKey(std::string key, T val) {
	auto it = uniforms.find(key);
	if(it == uniforms.end()) {
		TelabriumLog("Key \"" + key + "\" does not exist", 1);
		return T();
	}

	return it->second.second.emplace<T>(val);
}

// Get the value currently stored at (key)
template<typename T>
T Material::getKey(std::string key) {
	auto it = uniforms.find(key); 
	if(it == uniforms.end()) {
		TelabriumLog("Key \"" + key + "\" does not exist", 1);
		return T();
	}
	
	try {
		return std::get<T>(it->second.second);
	} catch(std::bad_variant_access& e) {
		std::string type = it->second.first;
		TelabriumLog("Uniform type did not match it's type string on: " + key + ", reported type was: " + type, 2);
		return T();
	}
}