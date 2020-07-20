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
class Shader {
public:
	static std::shared_ptr<Shader> requestShader(std::string id, std::string vertexPath, std::string fragmentPath, std::string geometryPath);

	Shader(std::string id, std::string vertexPath, std::string fragmentPath, std::string geometryPath = "\0");
	~Shader();

	std::string id;
	unsigned int glID = 0;
	std::string vertexPath, fragmentPath, geometryPath;

	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------

	void build(); // Create a new shader program

	void use(); // Activate the shader

	// <target string, result string> A map of strings to find and replace in the extracted shader, for configuration at compilation
	std::map<std::string, std::string> substitution_map;

	static std::vector<unsigned int> shader_draw_order;
	static std::map<unsigned int, std::shared_ptr<Shader>> shaders;

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

#endif