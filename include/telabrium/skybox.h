#ifndef TELABRIUM_SKYBOX
#define TELABRIUM_SKYBOX

#include "telabrium/utility.h"
#include "telabrium/camera.h"
#include "telabrium/mesh.h"

class Skybox : public Mesh {
public:
	Skybox(Texture cubemap);

	void Draw(Shader &shader);
};

#endif