#ifndef LUARIUM_SKYBOX
#define LUARIUM_SKYBOX

#include "luarium/core/generic.h"
#include "luarium/core/camera.h"
#include "luarium/core/mesh.h"

class Skybox : public Mesh {
public:
	Skybox(Texture cubemap);

	void Draw(Shader &shader);
};

#endif