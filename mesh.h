#pragma once

#include <glm/glm.hpp>

struct Mesh {
	float * vertices;
	size_t vertices_count;
	int * indices;
	size_t indices_count;
	float * texture_coordinates;
	size_t texture_coordinates_count;
};
