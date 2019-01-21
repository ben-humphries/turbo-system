#include "camera.h"
#include "sdl-state.h"

#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>

Math::mat4 Camera::get_view_matrix()
{
	return Math::look_at(position, position + forward, up);
}

void Camera::init(Math::vec3 position, float yaw, float pitch)
{
	this->position = position;
	this->yaw = yaw;
	this->pitch = pitch;
	update_vectors();
}

void Camera::update_vectors()
{
    forward.x = cos(Math::radians(yaw)) * cos(Math::radians(pitch));
    forward.y = sin(Math::radians(pitch));
    forward.z = sin(Math::radians(yaw)) * cos(Math::radians(pitch));
    forward = Math::normalize(forward);

    Math::vec3 right = Math::normalize(Math::cross(forward, Math::vec3(0,1,0)));
    up = Math::normalize(Math::cross(right, forward));
}

void Camera::move_in_direction(Direction direction)
{
	float speed = 1.0 * SDL_State::state.delta_time;

	Math::vec3 left_right = Math::normalize(Math::cross(forward, up)) * speed;
	Math::vec3 forward_back = Math::vec3(forward.x, 0.0, forward.z) * speed;

	//TODO(ben-humphries): implement += and -= operators so this can be cleaner
	switch (direction) {
	case DIR_LEFT:
		position = position - left_right;
		break;
	case DIR_RIGHT:
		position = position + left_right;
		break;
	case DIR_FORWARD:
		position = position + forward_back;
		break;
	case DIR_BACK:
		position = position - forward_back;
		break;
	}

	update_vectors();
	/*
	if (direction == UP) {
		cameraPos += glm::vec3(0, 1, 0) * speed;
	}
	else if (direction == DOWN) {
		cameraPos -= glm::vec3(0, 1, 0) * speed;
		}*/
}

void Camera::rotate(float dx, float dy, bool constrain_pitch)
{
	float mouse_sensitivity = 0.1;
	yaw = glm::mod(yaw + dx * mouse_sensitivity, 360.0f); //ensure yaw is between 0-360
	pitch += dy * mouse_sensitivity;


	if (constrain_pitch) {
		if (pitch > 89.0f) pitch = 89.0f;
		else if (pitch < -89.0f) pitch = -89.0f;
	}

	update_vectors();
}
