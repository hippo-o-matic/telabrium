#include "telabrium/skybox.h"


Skybox::Skybox(Texture cubemap) : Mesh(calcVertex(Telabrium::cubeVerts), Telabrium::cubeIndices, cubemap) {
	setupMesh();
}

void Skybox::Draw(Shader &shader){
	glFrontFace(GL_CW); // Allows us to render the cube so it isn't culled
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shader.use();
	shader.set("view", glm::mat4(glm::mat3(Camera::ACTIVE->GetViewMatrix()))); // remove translation from the view matrix
	shader.set("projection", Camera::ACTIVE->GetProjectionMatrix());

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].id);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS); // set depth function back to default
	glFrontFace(GL_CCW); // Set front face back to default
	glActiveTexture(GL_TEXTURE0);
}