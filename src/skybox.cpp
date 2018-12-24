#include "luarium/devices/skybox.h"

Skybox::Skybox(Texture cubemap) : Mesh(Luarium::calcVertex(Luarium::cubeVerts), Luarium::cubeIndices, cubemap) {
	setupMesh();
}

void Skybox::Draw(Shader &shader){
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shader.use();
	glm::mat4 newView = glm::mat4(glm::mat3(Camera::ACTIVE->GetViewMatrix())); // remove translation from the view matrix
	shader.set("view", newView);
	shader.set("projection", newView);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].id);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS); // set depth function back to default
	glActiveTexture(GL_TEXTURE0);

}