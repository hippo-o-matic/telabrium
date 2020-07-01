#define STB_IMAGE_IMPLEMENTATION
#include "telabrium/model.h"

// Static initialization
Task<Model, Shader&> Model::drawT(&Model::Draw);
Texture Model::tesst;

// constructor, expects a filepath to a 3D model.
Model::Model(std::string const &path, glm::vec3 _pos, glm::vec3 _rot, glm::vec3 _scl, bool gamma) : Object3d(_pos, _rot, _scl) {
	this->path = path;
	loadModel(path);
	drawT.addObj(this);
}

Model::Model(Json::Value j) : Object3d(j) {
	this->path = j["path"].asString();
	loadModel(path);
	drawT.addObj(this);
}

Model::~Model() {
	drawT.removeObj(this);
}

// draws the model, and thus all its meshes
void Model::Draw(Shader &shader){
	for (auto it : meshes) {
		shader.set("model", getWorldTransform() * it.transform);
		it.Draw(shader);
	}
}
 
// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(std::string const &path){
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		TelabriumLog(importer.GetErrorString(), 2);
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene){
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(aimesh, scene, node));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, aiNode* node) {	
	std::vector<unsigned int> indices;
	// Walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Get the meshes transform relative to its parent
	glm::mat4 transform = glm::mat4(1); aiVector3D aiPos, aiRot, aiScl;
	node->mTransformation.Decompose(aiScl, aiRot, aiPos);
	glm::vec3 pos = aiVec3ToGlm(aiPos);
	glm::vec3 rot = aiVec3ToGlm(aiRot);
	glm::vec3 scl = aiVec3ToGlm(aiScl);


	transform = glm::translate(transform, pos);
	transform = glm::rotate(transform, glm::radians(rot.x), glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, glm::radians(rot.y), glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, glm::radians(rot.z), glm::vec3(0, 0, 1));
	transform = glm::scale(transform, scl);

	// return a mesh object created from the extracted mesh data
	return Mesh(extractVertices(mesh),
				indices,
				extractTextures(mesh, scene), 
				transform,
				extractMaterial(mesh,scene)
			);
}

glm::vec3 aiVec3ToGlm(aiColor3D in) {
	glm::vec3 out;
	out.x = in.r; out.y = in.g; out.z = in.b;
	return out;
}

glm::vec3 aiVec3ToGlm(aiVector3D in) {
	glm::vec3 out;
	out.x = in.x; out.y = in.y; out.z = in.z;
	return out;
}

glm::mat4 aiMatrixToMat4(aiMatrix4x4 in) {
	glm::vec4 A(in.a1, in.a2, in.a3, in.a4);
	glm::vec4 B(in.b1, in.b2, in.b3, in.b4);
	glm::vec4 C(in.c1, in.c2, in.c3, in.c4);
	glm::vec4 D(in.d1, in.d2, in.d3, in.d4);

	return glm::mat4(A, B, C, D);
}

std::vector<Vertex> Model::extractVertices(aiMesh* mesh) {
	std::vector<Vertex> vertices;

	for(unsigned i=0; i < mesh->mNumVertices; i++) {
		Vertex v;

		v.pos.x = mesh->mVertices[i].x;
		v.pos.y = mesh->mVertices[i].y;
		v.pos.z = mesh->mVertices[i].z;

		v.Normal.x = mesh->mNormals[i].x;
		v.Normal.y = mesh->mNormals[i].y;
		v.Normal.z = mesh->mNormals[i].z;

		if(mesh->mTextureCoords[0]) {
			v.TexCoords.x = mesh->mTextureCoords[0][i].x;
			v.TexCoords.y = mesh->mTextureCoords[0][i].y;
		} else {
			v.TexCoords = glm::vec2(0);
		}

		if(mesh->mTangents) {
			v.Tangent.x = mesh->mTangents[i].x;
			v.Tangent.y = mesh->mTangents[i].y;
			v.Tangent.z = mesh->mTangents[i].z;

			v.Bitangent.x = mesh->mBitangents[i].x;
			v.Bitangent.y = mesh->mBitangents[i].y;
			v.Bitangent.z = mesh->mBitangents[i].z;
		}

		vertices.push_back(v);
	}
	
	return vertices;
}

shader_ublock Model::extractMaterial(aiMesh* mesh, const aiScene* scene) {
	shader_ublock block("Material");
	aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex]; // Get <mesh>'s aiMat from the <scene>
	
	// Material properties
	aiColor3D placeholder_color;
	float placeholder_float;

	aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, placeholder_color);
	block.add("color_diffuse", "vec3", aiVec3ToGlm(placeholder_color));
	
	aiMat->Get(AI_MATKEY_COLOR_SPECULAR, placeholder_color);
	block.add("color_specular", "vec3", aiVec3ToGlm(placeholder_color));

	aiMat->Get(AI_MATKEY_COLOR_AMBIENT, placeholder_color);
	block.add("color_ambient", "vec3", aiVec3ToGlm(placeholder_color));
	
	aiMat->Get(AI_MATKEY_COLOR_TRANSPARENT, placeholder_color);
	block.add("color_transparent", "vec3", aiVec3ToGlm(placeholder_color));

	aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, placeholder_color);
	block.add("color_emissive", "vec3", aiVec3ToGlm(placeholder_color));

	aiMat->Get(AI_MATKEY_SHININESS, placeholder_float);
	block.add("shine", "float", placeholder_float);

	aiMat->Get(AI_MATKEY_REFRACTI, placeholder_float);
	block.add("color_diffuse", "float", placeholder_float);

	aiMat->Get(AI_MATKEY_OPACITY, placeholder_float);
	block.add("color_diffuse", "float", placeholder_float);

	// aiMat->Get(AI_MATKEY_TWOSIDED, mat.twosided);
	// block.add("color_diffuse", "vec3", placeholder_bool);

	return block;
}

std::vector<Texture> Model::extractTextures(aiMesh* mesh, const aiScene* scene) {
	std::vector<Texture> textures;
	aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex]; // Get <mesh>'s aiMat from the <scene>

	// Check for embedded textures
	if(scene->HasTextures()) {

	}

	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	std::map<aiTextureType, const char*> texture_types = { // All the different types of textures to check for and load
		{aiTextureType_DIFFUSE, "tx_diffuse"},
		{aiTextureType_SPECULAR, "tx_specular"},
		{aiTextureType_AMBIENT, "tx_ambient"},
		{aiTextureType_EMISSIVE, "tx_emissive"},
		{aiTextureType_HEIGHT, "tx_height"},
		{aiTextureType_NORMALS, "tx_normal"},
		{aiTextureType_SHININESS, "tx_shine"},
		{aiTextureType_OPACITY, "tx_opacity"},
		{aiTextureType_REFLECTION, "tx_reflection"}
	};

	std::vector<Texture> extracted; // Placeholder to put into mat.textures
	for(auto it : texture_types) {
		extracted = loadMaterialTextures(aiMat, it.first, it.second);
		if(extracted.empty()) {
			Texture m = tesst;
			m.type = it.second;
			textures.push_back(m);
		} else {
			textures.insert(textures.end(), extracted.begin(), extracted.end());
		}
	}

	return textures;
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {
			// if texture hasn't been loaded already, load it
			Texture tex = loadTexture(std::string(str.C_Str()), this->directory);
			tex.type = typeName;
			textures.push_back(tex);
			textures_loaded.push_back(tex);  // store it as texture loaded for entire model, to ensure we won't load duplicate textures.
		}
	}
	return textures;
}

Texture loadTexture(std::string path, std::string directory){
	std::string filename = Telabrium::segment(path, '/').back(); // Get the filename from the path
	if (directory != "")
    	filename = directory + '/' + filename;
    Texture tex{0, "", filename};

	if(!Model::tesst.id) {
		glGenTextures(1,&Model::tesst.id);
	}

	// Generate 1 id for this texture
    glGenTextures(1, &tex.id);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = 4;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
		    format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, tex.id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    } else {
		GLenum err;
		if((err = glGetError()) != GL_NO_ERROR){
			std::cerr << "GL Error: \"" << err << "\"" << std::endl;
		}
		TelabriumLog("Texture failed to load at path: \"" + filename + "\"", 2);
    }

	stbi_image_free(data);
    return tex;
}

Texture loadCubemap(std::vector<std::string> faces, std::string path) {
	Texture tex{0, "texture_cubemap", path};
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex.id);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load((path + std::string("/") + faces[i]).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			TelabriumLog("Cubemap texture failed to load at path: \"" + path + "/" + faces[i] + "\"", 2);
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return tex;
}