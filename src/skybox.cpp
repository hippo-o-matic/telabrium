#include "telabrium/skybox.h"


Skybox::Skybox(Texture cubemap) : Mesh(calcVertex(Telabrium::cubeVerts), Telabrium::cubeIndices, std::vector<Texture>{cubemap}, std::vector<std::shared_ptr<Material>>{}) {
	auto mat = Material::requestMaterial("materials/skybox.json");
	mat->predraw_func = []() {
		glFrontFace(GL_CW); // Allows us to render the cube so it isn't culled
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	};
	mat->postdraw_func = []() {
		glDepthFunc(GL_LESS); // set depth function back to default
		glFrontFace(GL_CCW); // Set front face back to default
	};
	materials.push_back(mat);
}