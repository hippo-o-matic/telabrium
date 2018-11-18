#ifndef LUARIUM_MODEL_H
#define LUARIUM_MODEL_H

#include <map>

#include "stb_image.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "luarium/core/mesh.h"

unsigned int loadTexture(const char *path, const std::string &directory);

class Model : public Object{
public:
	/*  Model Data */
	std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	// constructor, expects a filepath to a 3D model.
	Model(std::string const &path,
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 scl = glm::vec3(1.0f, 1.0f, 1.0f),
		bool gamma = false);

	// draws the model, and thus all its meshes
	void Draw(Shader &shader);

private:
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(std::string const &path);

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

Texture loadTexture(const char *path);

Texture loadCubemap(std::vector<std::string> faces, std::string path);

#endif