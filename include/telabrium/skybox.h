#ifndef TELABRIUM_SKYBOX
#define TELABRIUM_SKYBOX

#include "telabrium/utility.h"
#include "telabrium/camera.h"
#include "telabrium/mesh.h"
//lmao delete dis
class Skybox : public Mesh {
public:
	Skybox(Texture cubemap);
};

#endif