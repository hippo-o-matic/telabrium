#include "telabrium/renderer.h"

void Telabrium::render() {
	// Go along the order, finding whichever
	for(unsigned order : Shader::shader_draw_order) {
		auto shader_it = Shader::shaders.find(order);
		if(shader_it != Shader::shaders.end()) {
			// Shader setup
			auto& shader = shader_it->second;
			shader->use();

			Light::updateLights(*shader); // Shouldn't really be here
			
			// Cycle through materials that use this shader
			for(auto material : Material::materials) {
				if(material->shader == shader) {
					if(material->predraw_func)
						material->predraw_func();

					material->updateShader(); // Send the shader this material's uniforms

					// Cycle through all the cameras
					for(auto camera : Camera::cameras) {
						shader->set("ViewPos", camera->getWorldPos());

						// view/projection transformations
						shader->set("projection", camera->getProjectionMatrix());
						shader->set("view", camera->getViewMatrix());

						// Iterate through the meshes and find any that use this material
						for(auto mesh : Mesh::meshes) {
							if(std::find(mesh->materials.begin(), mesh->materials.end(), material) != mesh->materials.end())
								mesh->Draw(*shader);
						}
					}

					if(material->postdraw_func)
						material->postdraw_func();
				}
			}
		}
	}
}