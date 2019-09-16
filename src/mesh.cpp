#include "luarium/mesh.h"

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
	unsigned int diffuseNr = 0, specularNr = 0, normalNr = 0, heightNr = 0, cubemapNr = 0;
	for (unsigned int i = 0; i < textures.size(); i++){
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		std::string number;// retrieve texture number (the N in diffuse_textureN)
		// check the textures type
		std::string type = textures[i].type;
		if (type == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (type == "texture_specular")
			number = std::to_string(specularNr++); 
		else if (type == "texture_normal")
			number = std::to_string(normalNr++); 
		else if (type == "texture_height")
			number = std::to_string(heightNr++); 
		else if (type == "texture_cubemap"){
			number = std::to_string(cubemapNr++);
		}
		// now set the sampler to the correct texture unit
		shader.set(("mat." + type).c_str(), (int)i);
		// and finally bind the texture
		if (type != "texture_cubemap")
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		else
			glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].id);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, *Skybox::env_map);
	}	

	// Set additonal material values
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


std::vector<Vertex> calcVertex(const std::vector<float> &verticies, const std::vector<float> &texcoords) {
	std::vector<Vertex> v;
	
	// Establish some empty Vertex structs
	for(long unsigned int i=0; i < verticies.size() / 3; i++){
		v.push_back(Vertex()); 
	}

	// Put all the vertex coords into the vertex vector
	for(long unsigned int i = 0, j = 0; i <= v.size(); i++, j+=3) {
		v[i].pos = glm::vec3(verticies[j], verticies[j+1], verticies[j+2]);
	}

	// Calculate vertex normals, 
	for(long unsigned int i=0, j=0; i <= v.size(); i++, j+=3){ //The i + 3 is to make sure we don't go past where there are values
		glm::vec3 U(v[j+1].pos - v[j].pos);
		glm::vec3 V(v[j+2].pos - v[j].pos);

		glm::vec3 N(
			U.y*V.z - U.z*V.y,
			U.z*V.x - U.x*V.z,
			U.x*V.y - U.y*V.x
		);
 
		v[i].Normal = N;
	}

	// Generate texcoords if there are none
	if (texcoords.empty()){
		for(long unsigned int i=0; i + 4 <= v.size(); i+=4){
			v[i].TexCoords = glm::vec2(0, 0);
			v[i+1].TexCoords = glm::vec2(1, 0);
			v[i+2].TexCoords = glm::vec2(1, 1);
			v[i+3].TexCoords = glm::vec2(0, 1);
		}
	} else {
		for(long unsigned int i=0; i < v.size(); i++){
			v[i].TexCoords = glm::vec2(texcoords[i], texcoords[i+1]);
		}
	}

	//Calculate the tangent and bitangents for each triangle
	for(long unsigned int i=0; i + 2 < v.size(); i+=3){
		glm::vec3 edge1 = v[i+1].pos - v[i].pos;
		glm::vec3 edge2 = v[i+2].pos - v[i].pos;

		glm::vec2 delta1 = v[i+1].TexCoords - v[i].TexCoords;
		glm::vec2 delta2 = v[i+2].TexCoords - v[i].TexCoords;

		// Scary math
		float f = 1.0f / (delta1.x * delta2.y - delta2.x * delta1.y);

		v[i].Tangent.x = f * (delta2.y * edge1.x - delta1.y * edge2.x);
		v[i].Tangent.y = f * (delta2.y * edge1.y - delta1.y * edge2.y);
		v[i].Tangent.z = f * (delta2.y * edge1.z - delta1.y * edge2.z);
		v[i].Tangent = glm::normalize(v[i].Tangent);
		v[i+1].Tangent = v[i].Tangent;
		v[i+2].Tangent = v[i].Tangent;

		v[i].Bitangent.x = f * (-delta2.x * edge1.x + delta1.x * edge2.x);
		v[i].Bitangent.y = f * (-delta2.x * edge1.y + delta1.x * edge2.y);
		v[i].Bitangent.z = f * (-delta2.x * edge1.z + delta1.x * edge2.z);
		v[i].Bitangent = glm::normalize(v[i].Bitangent);
		v[i+1].Bitangent = v[i].Bitangent;
		v[i+2].Bitangent = v[i].Bitangent;
	}
	return v;
}