#include "luarium/core/mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, RenderMat mat){
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = mat;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture tex, RenderMat mat){
	this->vertices = vertices;
	this->indices = indices;
	this->textures.push_back(tex);
	this->material = mat;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
}

// render the mesh
void Mesh::Draw(Shader &shader){
	// bind appropriate textures
	unsigned int diffuseNr = 1, specularNr = 1, normalNr = 1, heightNr = 1, cubemapNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++){
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		std::string number;// retrieve texture number (the N in diffuse_textureN)
		std::string type = textures[i].type;
		if (type == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (type == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (type == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (type == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream
		else if (type == "texture_cubemap"){
			number = std::to_string(cubemapNr++);
		}
		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader.ID, (type + number).c_str()), i);
		// and finally bind the texture
		if (type != "texture_cubemap")
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		else
			glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].id);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, *Skybox::env_map);
	}	

	shader.set("mat.diffuse_color", material.diffuse_color);
	shader.set("mat.specular_color", material.specular_color);
	shader.set("mat.ambient_color", material.ambient_color);

	shader.set("mat.shininess", material.shininess);
	shader.set("mat.IOR", material.IOR);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh(){
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}


