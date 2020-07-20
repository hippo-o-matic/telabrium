#include "telabrium/material.h"

std::string Material::DEFAULT_VERTEX_PATH = "shaders/Standard/Standard.vert";
std::string Material::DEFAULT_FRAGMENT_PATH = "shaders/Standard/Standard.frag";
std::string Material::DEFAULT_MATERIAL_PATH = "materials/default.json";

std::vector<std::shared_ptr<Material>> Material::materials;


std::shared_ptr<Material> Material::requestMaterial(std::string _path) {
	// Check to see if such a material has already been created
	for(auto mat : materials) {
		if(mat->path == _path)
			return mat;
	}

	// If not, make one
	auto material = std::make_shared<Material>(_path);
	materials.push_back(material);
	return material;
}


Material::Material() {
	parseMaterial(Telabrium::getJsonFile(DEFAULT_MATERIAL_PATH));
}

Material::Material(std::string path) {
	parseMaterial(Telabrium::getJsonFile(path));
}


void Material::parseShader(Json::Value json) {
	std::string path = json.get("path", "").asString();
	if(path != "")
		json = Telabrium::getJsonFile(path);

	// this->id = json.get("id", "").asString();

	shader = Shader::requestShader(
		json.get("id", "").asString(),
		json.get("vpath", DEFAULT_VERTEX_PATH).asString(),
		json.get("fpath", DEFAULT_FRAGMENT_PATH).asString(),
		json.get("gpath", "").asString()
	);

	Json::Value default_value;

	// Because we need to construct the index as Json::ArrayIndex and a bunch of other boilerplate, use this to make the next bit look better
	std::function<float(Json::ArrayIndex)> getAt = [&default_value](Json::ArrayIndex index, float default_float = 0) {
		return default_value.get(index, default_float).asFloat();
	};

	for(unsigned j = 0; j < json["args"].size(); j++) {
		Json::Value arg = json["args"][j];

		std::string type = arg["type"].asString();
		std::string key = arg["key"].asString();
		default_value = arg["default"];

		if(type == "bool") {
			addKey<bool>(key, type, arg.get("default", false).asBool());
		} else if(type == "int") {
			addKey<int>(key, type, arg.get("default", 0).asInt());
		} else if(type == "float") {
			addKey<float>(key, type, arg.get("default", 0).asFloat());
		} else if(type == "vec2") {
			glm::vec2 temp = glm::vec2(getAt(0), getAt(1));
			addKey<glm::vec2>(key, type, temp);

		} else if(type == "vec3") {
			glm::vec3 temp = glm::vec3(getAt(0), getAt(1), getAt(2));
			addKey<glm::vec3>(key, type, temp);
			
		} else if(type == "vec4") {
			glm::vec4 temp = glm::vec4(getAt(0), getAt(1), getAt(2), getAt(3));           
			addKey<glm::vec4>(key, type, temp);

		} else if(type == "mat2") {
			glm::mat2 temp = glm::mat2(
				getAt(0), getAt(1),
				getAt(2), getAt(3)
			);
			addKey<glm::mat2>(key, type, temp);

		} else if(type == "mat3") {
			glm::mat3 temp = glm::mat3(
				getAt(0), getAt(1), getAt(2),
				getAt(3), getAt(4), getAt(5),
				getAt(6), getAt(7), getAt(8)
			);
			addKey<glm::mat3>(key, type, temp);

		} else if(type == "mat4") {
			glm::mat4 temp = glm::mat4(
				getAt(0), getAt(1), getAt(2), getAt(3),
				getAt(4), getAt(5), getAt(6), getAt(7),
				getAt(8), getAt(9), getAt(10), getAt(11),
				getAt(12), getAt(13), getAt(14), getAt(15)
			);
			addKey<glm::mat4>(key, type, temp);

		} else {
			TelabriumLog("Key did not have an appropriate type! Key: " + key, 2);
		}

	}
}

void Material::parseMaterial(Json::Value json) {
	id = json["id"].asString();
	if(json["type"].asString() != "Material") {
		TelabriumLog("Json Object given was not of type \"Material\"", 1);
		return;
	}

	parseShader(json["shader"]);

	Json::Value default_value;

	// Because we need to construct the index as Json::ArrayIndex and a bunch of other boilerplate, use this to make the next bit look better
	std::function<float(Json::ArrayIndex)> getAt = [&default_value](Json::ArrayIndex index, float default_float = 0) {
			return default_value.get(index, default_float).asFloat();
	};

	for(unsigned j = 0; j < json["args"].size(); j++) { 	// Start going through the provided arguments
		std::string key = json["args"][j]["key"].asString();
		auto pair_it = uniforms.find(key);

		if(pair_it != uniforms.end()) { // If this one's stored as a key,
			std::string type = pair_it->second.first; // Get the type stored as a string in the uniforms map
			default_value = json["args"][j]["default_value"]; // Isolate the default_value

			// Run through types
			if(type == "bool") {
				modifyKey<bool>(key, default_value.asBool());
			} else if(type == "int") {
				modifyKey<int>(key, default_value.asInt());
			} else if(type == "float") {
				modifyKey<float>(key, default_value.asFloat());
			} else if(type == "vec2") {
				glm::vec2 temp = glm::vec2(getAt(0), getAt(1));
				modifyKey<glm::vec2>(key, temp);

			} else if(type == "vec3") {
				glm::vec3 temp = glm::vec3(getAt(0), getAt(1), getAt(2));
				modifyKey<glm::vec3>(key, temp);
				
			} else if(type == "vec4") {
				glm::vec4 temp = glm::vec4(getAt(0), getAt(1), getAt(2), getAt(3));           
				modifyKey<glm::vec4>(key, temp);

			} else if(type == "mat2") {
				glm::mat2 temp = glm::mat2(
					getAt(0), getAt(1),
					getAt(2), getAt(3)
				);
				modifyKey<glm::mat2>(key, temp);

			} else if(type == "mat3") {
				glm::mat3 temp = glm::mat3(
					getAt(0), getAt(1), getAt(2),
					getAt(3), getAt(4), getAt(5),
					getAt(6), getAt(7), getAt(8)
				);
				modifyKey<glm::mat3>(key, temp);

			} else if(type == "mat4") {
				glm::mat4 temp = glm::mat4(
					getAt(0), getAt(1), getAt(2), getAt(3),
					getAt(4), getAt(5), getAt(6), getAt(7),
					getAt(8), getAt(9), getAt(10), getAt(11),
					getAt(12), getAt(13), getAt(14), getAt(15)
				);
				modifyKey<glm::mat4>(key, temp);
			}
		}
	}
}

void Material::updateShader() {
	for(auto it : uniforms) {
		std::string key = material_name + '.' + it.first;
		std::string type = it.second.first;
		try {
			if(type == "bool") {
				shader->set(key, std::get<bool>(it.second.second));
			} else if(type == "int") {
				shader->set(key, std::get<int>(it.second.second));
			} else if(type == "float") {
				shader->set(key, std::get<float>(it.second.second));
			} else if(type == "vec2") {
				shader->set(key, std::get<glm::vec2>(it.second.second));
			} else if(type == "vec3") {
				shader->set(key, std::get<glm::vec3>(it.second.second));
			} else if(type == "vec4") {
				shader->set(key, std::get<glm::vec4>(it.second.second));
			} else if(type == "mat2") {
				shader->set(key, std::get<glm::mat2>(it.second.second));
			} else if(type == "mat3") {
				shader->set(key, std::get<glm::mat3>(it.second.second));
			} else if(type == "mat4") {
				shader->set(key, std::get<glm::mat4>(it.second.second));
			} else {
				TelabriumLog("Key did not have an appropriate type! Key: " + key, 2);
			}
		} catch(std::bad_variant_access& e) {
			TelabriumLog("Uniform type did not match it's type string on: " + key + ", reported type was: " + type, 2);
		}
	}
}

// Material::Material(std::vector<unsigned int> shaders) {
//     for(auto it : shaders) {
//         addShader(*it);
//     }
// }

// void Material::addShader(unsigned int glID, Shader* shader) {
//     if(registered_shaders.find(glID) != registered_shaders.end()) {
//         if(shader) {
//             registerShader(shader);
//         } else {
//             TelabriumLog("The shader glID specified hasn't been registered, and no valid shader was given to register!", 2);
//             return;
//         }
//     }

//     material_shaders.push_back(std::pair<unsigned, Shader*>(glID, shader));
// }

// void Material::removeShader(unsigned glID) {
//     material_shaders.erase(glID);
// }

// void Material::registerShader(Shader* shader) {
//     if(registered_shaders.find(shader->glID) != registered_shaders.end()) {
//         TelabriumLog("A shader with this glID is already registered!",2);
//     } else {
//         material_shaders.push_back(std::pair<unsigned, Shader*>(glID, shader));
//     }
// }
