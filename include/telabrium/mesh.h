#ifndef TELABRIUM_MESH_H
#define TELABRIUM_MESH_H

#include <string>
#include <vector>
#include <variant>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <assimp/material.h>

#include "telabrium/shader.h"
#include "telabrium/object.h"

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
	aiColor3D color_diffuse;
	aiColor3D color_specular;
	aiColor3D color_ambient;
	aiColor3D color_transparent;
	aiColor3D color_emissive;

	float shine;
	float ior;
	float opacity;

	bool twosided = false;
};


/// A class for storing vertex, texture, and material data to be drawn as a 3d object
class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	shader_ublock block; // Uniform block of material data, colors, etc

	unsigned int VAO;
	glm::mat4 transform;

	// constructor
	Mesh(std::vector<Vertex> vertices,
		std::vector<unsigned int> indices,
		std::vector<Texture> texs,
		glm::mat4 transform = glm::mat4(),
		shader_ublock block = {}
	);

	// render the mesh
	void Draw(Shader &shader);

protected:
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh();
};

// Translates a basic array of verticies into a format readable by the Mesh class
std::vector<Vertex> calcVertex(const std::vector<float> &verticies, const std::vector<float> &texcoords = {});

#endif

