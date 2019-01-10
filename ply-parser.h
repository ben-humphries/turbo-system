#pragma once

#include "renderer.h"

namespace PLY {
	Renderer::Mesh * load_mesh_from_ply_file(const char * path);
	void mesh_test();
};
