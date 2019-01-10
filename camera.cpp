#include "camera.h"
#include "sdl_state.h"

glm::mat4 Camera::get_view_matrix()
{
	return glm::lookAt(position, position + forward, up);
}

void Camera::init(glm::vec3 position, float yaw, float pitch)
{
	this->position = position;
	this->yaw = yaw;
	this->pitch = pitch;
	update_vectors();
}

void Camera::update_vectors()
{
    forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward.y = sin(glm::radians(pitch));
    forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward = glm::normalize(forward);

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
    up = glm::normalize(glm::cross(right, forward));
}

void Camera::move_in_direction(Direction direction)
{
	float speed = 1.0 * SDL_State::state.delta_time;

	glm::vec3 left_right = glm::normalize(glm::cross(forward, up)) * speed;
	glm::vec3 forward_back = glm::vec3(forward.x, 0.0, forward.z) * speed;
	
	switch (direction) {
	case DIR_LEFT:
		position -= left_right;
		break;
	case DIR_RIGHT:
		position += left_right;
		break;
	case DIR_FORWARD:
		position += forward_back;
		break;
	case DIR_BACK:
		position -= forward_back;
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
