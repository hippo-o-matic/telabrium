#ifndef HIPPO_MESH_H
#define HIPPO_MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "hipponium/shader.h"
#include "hipponium/object.h"

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
	glm::vec3 diffuse_color;
	glm::vec3 specular_color;
	glm::vec3 ambient_color;
	glm::vec3 trans_color;

	float shininess;
	float IOR;
	float opacity;
};

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
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, RenderMat mat);

	// render the mesh
	void Draw(Shader* shader);

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh();
};
#endif

