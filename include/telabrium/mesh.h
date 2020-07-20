#pragma once

#include <string>
#include <vector>
#include <variant>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <assimp/material.h>

#include "telabrium/shader.h"
#include "telabrium/object3d.hpp"
#include "telabrium/material.h"

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
	unsigned int glID;
	std::string type;
	std::string path;
};

// A class for storing vertex, texture, and material data to be drawn as a 3d object
class Mesh : public Object3d {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	std::vector<std::shared_ptr<Material>> materials;

	unsigned int VAO;
	glm::mat4 transform;

	// constructor
	Mesh(std::vector<Vertex> vertices,
		std::vector<unsigned int> indices,
		std::vector<Texture> texs = std::vector<Texture>(),
		std::vector<std::shared_ptr<Material>> mats = std::vector<std::shared_ptr<Material>>{Material::requestMaterial()},
		glm::mat4 transform = glm::mat4()
	);

	~Mesh();

	// render the mesh
	void Draw(Shader &shader);

	static std::vector<Mesh*> meshes;

protected:
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh();
};

// Translates a basic array of verticies into a format readable by the Mesh class
std::vector<Vertex> calcVertex(const std::vector<float> &verticies, const std::vector<float> &texcoords = {});