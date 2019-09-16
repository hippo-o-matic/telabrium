#ifndef LUARIUM_MESH_H
#define LUARIUM_MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "luarium/shader.h"
#include "luarium/object.h"

struct Vertex {
	// position
	glm::vec3 pos;
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

/// A class for storing vertex, texture, and material data to be drawn as a 3d object
class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	RenderMat material;
	unsigned int VAO;

	/*  Functions  */
	// constructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, RenderMat mat = {});

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture tex = {}, RenderMat mat = {});

	// render the mesh
	void Draw(Shader &shader);

protected:
	/*  Render data  */
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh();
};

// Translates a basic array of verticies into a format readable by the Mesh class
std::vector<Vertex> calcVertex(const std::vector<float> &verticies, const std::vector<float> &texcoords = {});

#endif

