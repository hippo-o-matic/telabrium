#include "luarium/devices/skybox.h"

Skybox::Skybox(Texture cubemap) : Mesh(Luarium::calcVertex(Luarium::cubeVerts), Luarium::cubeIndices, cubemap) {
	setupMesh();
}

void Skybox::Draw(Shader &shader){
	
	
	// skybox cube
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].id);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS); // set depth function back to default
	glActiveTexture(GL_TEXTURE0);

}