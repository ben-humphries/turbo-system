#pragma once

#include "mesh.h"

struct Model {
	Mesh * mesh;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	float * vertices_and_tex_coords;
};

void load_model(Model * model);

void bind_model(Model * model);

void render_model(Model * model);

void cleanup_model(Model * model);
