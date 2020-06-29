#ifndef TELABRIUM_SHADER_H
#define TELABRIUM_SHADER_H

#include <string>
#include <fstream> // Shader fetching
#include <sstream> 
#include <iostream> // Debugging
#include <memory> // Smart pointers (active shader)

// For uniform blocks
#include <map>
#include <variant>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <assimp/material.h>

#include "telabrium/utility.h"

/// A class for interacting with an OpenGL shader
class Shader{
public:
	static std::unique_ptr<Shader> ACTIVE;

    unsigned int ID = 0;
	std::string vertexPath, fragmentPath, geometryPath;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = "\0");

	~Shader();

	void build(); // Create a new shader program

    void use(); // Activate the shader

	// <target string, result string> A map of strings to find and replace in the extracted shader, for configuration at compilation
	std::map<std::string, std::string> substitution_map; 

    // utility uniform functions
    // ------------------------------------------------------------------------
    void set(const std::string &name, bool value) const; // bool

    void set(const std::string &name, int value) const;// int

    void set(const std::string &name, float value) const; // float

    void set(const std::string &name, const glm::vec2 &value) const; // vec2

    void set(const std::string &name, float x, float y) const; // vec2

    void set(const std::string &name, const glm::vec3 &value) const; // vec3

    void set(const std::string &name, float x, float y, float z) const; // vec3

    void set(const std::string &name, const glm::vec4 &value) const; // vec4

    void set(const std::string &name, float x, float y, float z, float w) const; // vec4

    void set(const std::string &name, const glm::mat2 &mat) const; // mat2

    void set(const std::string &name, const glm::mat3 &mat) const; // mat3

    void set(const std::string &name, const glm::mat4 &mat) const; // mat4

    void set(const std::string &name, const aiColor3D &vec) const; // assimp data type

    void set(const std::string &name, const aiColor4D &vec) const; // assimp data type

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type);
};


#define TELA_SHADER_UNIFORM_TYPES bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4

// TODO: make full type
struct shader_ublock {
	typedef std::pair<std::string, std::variant<TELA_SHADER_UNIFORM_TYPES>> uniform_type;
	std::string block_name; // The name of the interface block in the shader
	// <key string, <item type as string, item> >
	// item type strings are as follows: bool, int, float, vec2, vec3, vec4, mat2, mat3, mat4
	std::map<std::string, uniform_type> uniforms;

	shader_ublock(std::string name = "") {
		block_name = name;
	}

	template<typename T>
	void add(std::string key, std::string type, T val) {
		std::variant<TELA_SHADER_UNIFORM_TYPES> value = val;
		uniforms.insert(std::pair<std::string, uniform_type>(key, uniform_type(type, val)) );
	}

	// OPTIMIZE: this whole thing could probably be made more elegant
	void send_block(Shader &sh) {
		for(auto it : uniforms) {
			std::string key = block_name + '.' + it.first;
			std::string type = it.second.first;
			try {
				if(type == "bool") {
					sh.set(key, std::get<bool>(it.second.second));
				} else if(type == "int") {
					sh.set(key, std::get<int>(it.second.second));
				} else if(type == "float") {
					sh.set(key, std::get<float>(it.second.second));
				} else if(type == "vec2") {
					sh.set(key, std::get<glm::vec2>(it.second.second));
				} else if(type == "vec3") {
					sh.set(key, std::get<glm::vec3>(it.second.second));
				} else if(type == "vec4") {
					sh.set(key, std::get<glm::vec4>(it.second.second));
				} else if(type == "mat2") {
					sh.set(key, std::get<glm::mat2>(it.second.second));
				} else if(type == "mat3") {
					sh.set(key, std::get<glm::mat3>(it.second.second));
				} else if(type == "mat4") {
					sh.set(key, std::get<glm::mat4>(it.second.second));
				} else {
					TelabriumLog("Key did not have an appropriate type! Key: " + key, 2);
				}
			} catch(std::bad_variant_access& e) {
				TelabriumLog("Uniform type did not match it's type string on: " + key + ", reported type was: " + type, 2);
			}
		}
	}
};

#endif