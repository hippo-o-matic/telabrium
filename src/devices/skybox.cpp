#include "luarium/devices/skybox.h"

Skybox::Skybox(Texture cubemap) : Mesh(Luarium::calcVertex(Luarium::cubeVerts), Luarium::cubeIndices, cubemap) {}

void Skybox::Draw(Shader &shader){
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shader.use();
	
	// skybox cube
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glDepthFunc(GL_LESS); // set depth function back to default
}