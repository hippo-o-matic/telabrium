#ifndef LUARIUM_UTILITY_H
#define LUARIUM_UTILITY_H

#include <iostream>
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include <fstream>
#include <time.h>

const std::string LUARIUM_CACHE_PATH = "tests/cache/";

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

struct RenderMat {
	glm::vec3 diffuse_color = glm::vec3(0,0,0);
	glm::vec3 specular_color = glm::vec3(0,0,0);
	glm::vec3 ambient_color = glm::vec3(0,0,0);

	float shininess = 0;
	float IOR = 0;
	float opacity = 0;
};

namespace Luarium{
	// Seperates a string into a vector of strings, separated by [seperator] (input string, seperator)
	std::vector<std::string> segment(std::string &in, char seperator);

	// Translates a basic array of verticies into a format readable by the Mesh class
	std::vector<Vertex> calcVertex(const std::vector<float> &verticies, const std::vector<float> &texcoords = {});

	void log(std::string message, short int severity = 0, const char* outputfile = "tests/logs/log.txt");



	const std::vector<float> cubeVerts = {
		// positions          
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		
		// Back face
		-1.0, -1.0, -1.0,
		-1.0,  1.0, -1.0,
		1.0,  1.0, -1.0,
		1.0, -1.0, -1.0,
		
		// Top face
		-1.0,  1.0, -1.0,
		-1.0,  1.0,  1.0,
		1.0,  1.0,  1.0,
		1.0,  1.0, -1.0,
		
		// Bottom face
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,
		
		// Right face
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		1.0,  1.0,  1.0,
		1.0, -1.0,  1.0,
		
		// Left face
		-1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0
	};

	const std::vector<unsigned int> cubeIndices = {
		0,  1,  2,      0,  2,  3, // front
		4,  5,  6,      4,  6,  7, // back
		8,  9,  10,     8,  10, 11, // top
		12, 13, 14,     12, 14, 15, // bottom
		16, 17, 18,     16, 18, 19, // right
		20, 21, 22,     20, 22, 23 // left
	};
}

#endif

