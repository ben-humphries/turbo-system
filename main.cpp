#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/vec3.hpp>

#include "common.h"
#include "sdl-state.h"
#include "list.h"
#include "ply-parser.h"
#include "renderer.h"
#include "camera.h"

struct Transform {
	glm::mat4 model_matrix;
	
	void init()
	{
		model_matrix = glm::mat4(1);
	}
	void move(glm::vec3 movement)
	{
		model_matrix = glm::translate(model_matrix, movement);
	}
};

struct Entity {
	Transform transform;
	Renderer::Shader * shader;

	Entity * parent;
	List<Entity*> children;
	
	void base_initialize()
	{
		shader = Renderer::Shader::get_shader("default", "vertex.glsl", "fragment.glsl");
		parent = NULL;
		children.alloc();
	}
	void add_child(Entity * entity)
	{
		children.push(entity);
		entity->base_initialize();
		entity->initialize();
	}
	virtual void initialize() {}
	virtual void update() {}
	virtual void render(Camera * camera) {}
};

struct Monkey : Entity {
	Renderer::Model * model;
	Renderer::Texture * texture;
	virtual void initialize()
	{
		model = Renderer::Model::get_model("example.ply");
		texture = Renderer::Texture::get_texture("texture.png");
	}
	virtual void render(Camera * camera)
	{
		Renderer::render(model, texture, shader, camera, transform.model_matrix);
	}
};

void update_entity_tree(Entity * root)
{
	root->update();
	for (int i = 0; i < root->children.size; i++) {
		update_entity_tree(root->children[i]);
	}
}

void render_entity_tree(Entity * root, Camera * camera)
{
	root->render(camera);
	for (int i = 0; i < root->children.size; i++) {
		render_entity_tree(root->children[i], camera);
	}
}

int main()
{
	// Program initialization
	SDL_State::state.init(800, 600);
	Renderer::initialize_renderer();

	Camera camera;
	camera.init(glm::vec3(0, 0, -3), 90, -20);

	// Initialize entity tree
	Entity * root = new Entity();
	root->base_initialize();

	for (int i = 0; i < 17; i++) {
		Monkey * monkey = new Monkey();
		root->add_child(monkey);
	}
	root->children[0]->add_child(new Monkey());

	for (int i = 0; i < root->children.size; i++) {
		root->children[i]->transform.move(glm::vec3(0, i, 0));
	}
	root->children[0]->children[0]->transform.move(glm::vec3(3, 0, 0));

	// Main loop
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

		// Camera movement
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
		
		// Update
		update_entity_tree(root);
		
		// Render
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_entity_tree(root, &camera);
		
		SDL_GL_SwapWindow(SDL_State::state.window);
		
		// delta time
		{
			uint64_t new_dt_count = SDL_GetPerformanceCounter();
			SDL_State::state.delta_time = (float) (new_dt_count - SDL_State::state.last_dt_count) / SDL_GetPerformanceFrequency();
			SDL_State::state.last_dt_count = new_dt_count;
			//printf("%f                  \r", 1.0 / SDL_State::state.delta_time);
		}
	}
}
