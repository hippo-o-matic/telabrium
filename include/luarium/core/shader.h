#ifndef LUARIUM_SHADER_H
#define LUARIUM_SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>

#include "glad/glad.h"
#include "glm/glm.hpp"

class Shader{
public:
	static Shader* ACTIVE;

    unsigned int ID;
	std::string vertexPath, fragmentPath, geometryPath;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = "\0");

	~Shader();

	void build(); //Create a new shader program

    void use(); //Activate the shader

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


private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif