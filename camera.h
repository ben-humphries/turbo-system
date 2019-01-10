#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Direction {
	DIR_FORWARD,
	DIR_LEFT,
	DIR_BACK,
	DIR_RIGHT,
	DIR_COUNT,
};

struct Camera {
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 forward;
	float yaw;
	float pitch;
	glm::mat4 get_view_matrix();
	void init(glm::vec3 position, float yaw, float pitch);
	void update_vectors();
	void move_in_direction(Direction direction);
	void rotate(float dx, float dy, bool constrain_pitch = true);
};
