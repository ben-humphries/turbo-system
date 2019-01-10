#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common.h"
#include "list.h"
#include "ply-parser.h"
#include "model.h"
#include "shader.h"
#include "texture.h"

enum Direction {
	DIR_FORWARD,
	DIR_LEFT,
	DIR_BACK,
	DIR_RIGHT,
	DIR_COUNT,
};

struct SDL_State {
	int width;
	int height;
	SDL_Window * window;
	SDL_GLContext gl_context;
	uint64_t last_dt_count;
	float delta_time;
	void init(int width, int height);
};

void SDL_State::init(int width, int height)
{
	this->width = width;
	this->height = height;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	
	window = SDL_CreateWindow("OpenGL",
							  SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED,
							  width, height,
							  SDL_WINDOW_OPENGL);
	SDL_ShowCursor(SDL_FALSE);
	gl_context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err) {
		printf("%s\n", glewGetErrorString(err));
	}

	glEnable(GL_DEPTH_TEST);

	last_dt_count = SDL_GetPerformanceCounter();
	delta_time    = 1.0 / 60.0;
}

static SDL_State sdl_state;

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
	float speed = 1.0 * sdl_state.delta_time;

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

struct Entity {
	Model * model;
	Texture texture;
	Shader  shader;
	static Entity create(const char * model_path,
						 const char * texture_path,
						 Shader shader);
};

Entity Entity::create(const char * model_path,
					  const char * texture_path,
					  Shader shader)
{
	Entity entity;
	
	entity.model = (Model *) malloc(sizeof(Model));
	entity.model->mesh = PLY::load_mesh_from_ply_file(model_path);
	load_model(entity.model);

	entity.texture = Texture::load_from_file(texture_path);

	entity.shader = shader;

	return entity;
}

static List<Entity> entities;

namespace Renderer {
	glm::mat4 projection_matrix(1);

	void initialize_renderer()
	{
		projection_matrix = glm::perspective(glm::radians(45.0f),
											 (float) sdl_state.width / sdl_state.height,
											 0.1f, 300.0f);
	}
	
	void render(Model * model, Texture texture, Shader shader, Camera * camera)
	{
		texture.use_texture();
		
		shader.use_program();
		shader.set_mat4_uniform("transform", projection_matrix * camera->get_view_matrix());
		
		render_model(model);
	}
}

int main()
{	
	// Tests
	test_lists();

	entities.alloc();
	
	sdl_state.init(800, 600);

	Renderer::initialize_renderer();

	Camera camera;
	camera.init(glm::vec3(0, 0, -3), 90, -20);

	Shader default_shader = Shader::load_from_source("vertex.glsl", "fragment.glsl");

	entities.push(Entity::create("example.ply", "texture.png", default_shader));
		
	bool directions[DIR_COUNT] = {0};
	
	SDL_Event event;
	bool running = true;
	
	while (running) {
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		const Uint8 * keystates = SDL_GetKeyboardState(NULL);
		
		if (keystates[SDL_SCANCODE_W]) {
			camera.move_in_direction(DIR_FORWARD);
		}
		if (keystates[SDL_SCANCODE_A]) {
			camera.move_in_direction(DIR_LEFT);
		}
		if (keystates[SDL_SCANCODE_S]) {
			camera.move_in_direction(DIR_BACK);
		}
		if (keystates[SDL_SCANCODE_D]) {
			camera.move_in_direction(DIR_RIGHT);
		}
		if (keystates[SDL_SCANCODE_ESCAPE]) {
			running = false;
		}

		// Rotate camera based on mouse movement
		int x, y;
		SDL_GetMouseState(&x, &y);
		x -= sdl_state.width / 2;
		y -= sdl_state.height / 2;
		SDL_WarpMouseInWindow(sdl_state.window, sdl_state.width / 2, sdl_state.height / 2);
		camera.rotate(x, -y);

		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < entities.size; i++) {
			Entity e = entities[i];
			Renderer::render(e.model, e.texture, e.shader, &camera);
		}
		
		SDL_GL_SwapWindow(sdl_state.window);
		
		// delta time
		{
			uint64_t new_dt_count = SDL_GetPerformanceCounter();
			sdl_state.delta_time = (float) (new_dt_count - sdl_state.last_dt_count) / SDL_GetPerformanceFrequency();
			sdl_state.last_dt_count = new_dt_count;
			//printf("%f                  \r", 1.0 / sdl_state.delta_time);
		}
	}
	
	SDL_GL_DeleteContext(sdl_state.gl_context);
	return 0;
}
