#pragma once

#include "common.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

enum Direction {
	DIR_FORWARD,
	DIR_LEFT,
	DIR_BACK,
	DIR_RIGHT,
	DIR_COUNT,
};

struct Camera {
	Math::vec3 position;
	Math::vec3 up;
	Math::vec3 forward;

	float yaw;
	float pitch;
	Math::mat4 get_view_matrix();
	void init(Math::vec3 position, float yaw, float pitch);
	void update_vectors();
	void move_in_direction(Direction direction);
	void rotate(float dx, float dy, bool constrain_pitch = true);
};
