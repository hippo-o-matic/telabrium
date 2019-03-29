#ifndef LUARIUM_SKYBOX
#define LUARIUM_SKYBOX

#include "luarium/utility.h"
#include "luarium/camera.h"
#include "luarium/mesh.h"

class Skybox : public Mesh {
public:
	Skybox(Texture cubemap);

	void Draw(Shader &shader);
};

#endif