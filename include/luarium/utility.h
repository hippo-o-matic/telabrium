#ifndef LUARIUM_UTILITY_H
#define LUARIUM_UTILITY_H

#include <iostream>
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include <fstream>
#include <time.h>

const std::string LUARIUM_CACHE_PATH = "tests/cache/";

namespace Luarium{
	// Seperates a string into a vector of strings, separated by [seperator] (input string, seperator)
	std::vector<std::string> segment(std::string &in, char seperator);

	void log(std::string message, const char* file, int line, short int severity = 0, const char* outputfile = "tests/logs/log.txt");


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

#define LuariumLog(MESSAGE, SEVERITY) \
Luarium::log(MESSAGE, __FILE__, __LINE__)

#endif

