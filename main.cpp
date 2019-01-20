#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common.h"
#include "sdl-state.h"
#include "list.h"
#include "ply-parser.h"
#include "renderer.h"
#include "camera.h"

struct Entity {
	Renderer::Model *  model;
	Renderer::Texture * texture;
	Renderer::Shader  shader;

	glm::mat4 model_matrix;
	
	static Entity create(const char * model_path,
						 const char * texture_path,
						 Renderer::Shader shader);
};

Entity Entity::create(const char * model_path,
					  const char * texture_path,
					  Renderer::Shader shader)
{
	Entity entity;
	
	entity.model = Renderer::Model::get_model(model_path);
	entity.texture = Renderer::Texture::get_texture(texture_path);
	entity.shader = shader;

	entity.model_matrix = glm::mat4(1);

	return entity;
}

static List<Entity> entities;

int main()
{	
	// Tests
	test_lists();

	entities.alloc();
	
	SDL_State::state.init(800, 600);

	Renderer::initialize_renderer();

	Camera camera;
	camera.init(glm::vec3(0, 0, -3), 90, -20);

	Renderer::Shader default_shader = Renderer::Shader::load_from_source("vertex.glsl", "fragment.glsl");

	entities.push(Entity::create("example.ply", "texture.png", default_shader));
	Entity e = Entity::create("example.ply", "texture.png", default_shader);
	e.model_matrix = glm::translate(e.model_matrix, glm::vec3(0,1.5,0));
	entities.push(e);

	
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
		x -= SDL_State::state.width / 2;
		y -= SDL_State::state.height / 2;
		SDL_WarpMouseInWindow(SDL_State::state.window, SDL_State::state.width / 2, SDL_State::state.height / 2);
		camera.rotate(x, -y);

		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < entities.size; i++) {
			Entity e = entities[i];
			Renderer::render(e.model, e.texture, e.shader, &camera, e.model_matrix);
		}
		
		SDL_GL_SwapWindow(SDL_State::state.window);
		
		// delta time
		{
			uint64_t new_dt_count = SDL_GetPerformanceCounter();
			SDL_State::state.delta_time = (float) (new_dt_count - SDL_State::state.last_dt_count) / SDL_GetPerformanceFrequency();
			SDL_State::state.last_dt_count = new_dt_count;
			//printf("%f                  \r", 1.0 / SDL_State::state.delta_time);
		}
	}
	
	SDL_GL_DeleteContext(SDL_State::state.gl_context);
	return 0;
}
