#include "luarium/skybox.h"


Skybox::Skybox(Texture cubemap) : Mesh(Luarium::calcVertex(Luarium::cubeVerts), Luarium::cubeIndices, cubemap) {
	setupMesh();
}

void Skybox::Draw(Shader &shader){
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shader.use();
	shader.set("view", glm::mat4(glm::mat3(Camera::ACTIVE->GetViewMatrix()))); // remove translation from the view matrix
	shader.set("projection", Camera::ACTIVE->GetProjectionMatrix());

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].id);
	glDrawArrays(GL_TRIANGLES, 0, indices.size());
	glBindVertexArray(0);

	glDepthFunc(GL_LESS); // set depth function back to default
	glActiveTexture(GL_TEXTURE0);

}