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

#include "luarium/core/shader.h"
#include "luarium/core/object.h"
#include "luarium/core/generic.h"

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

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture tex, RenderMat mat = {});

	// render the mesh
	void Draw(Shader &shader);

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh();
};
#endif

