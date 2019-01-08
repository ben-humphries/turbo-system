#pragma once

#include "mesh.h"

namespace PLY {
	Mesh * load_mesh_from_ply_file(const char * path);
	void mesh_test();
};
