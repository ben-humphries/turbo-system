#include <stdio.h>
#include <math.h>

#include <string.h>
#include <stdlib.h>

#undef	IMGUI_IMPL_OPENGL_LOADER_GL3W
#undef	IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "common.h"
#include "sdl-state.h"
#include "list.h"
#include "ply-parser.h"
#include "renderer.h"
#include "camera.h"
#include "entity.h"

struct Monkey : Entity {
	Renderer::Model * model;
	Renderer::Texture * texture;
	virtual void initialize()
	{
		name = "Monkey";
		model = Renderer::Model::get_model("res/player.ply");
		texture = Renderer::Texture::get_texture("res/marble.png");
	}
	virtual void render(Camera * camera)
	{
		Renderer::render(model, texture, shader, camera, transform.model_matrix);
	}
};

void draw_debug(Entity * root);

int main()
{
	// Program initialization
	SDL_State::state.init(800, 600);
	Renderer::initialize_renderer();
	
	Camera camera;
	camera.init(Math::vec3(0, 0, -3), 90, -20);

	// Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//(void) io; // what the fuck is this for?
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(SDL_State::state.window, SDL_State::state.gl_context);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	// Initialize entity tree
	Entity * root = new Entity();
	root->base_initialize();

	root->add_child(new Monkey());
	root->add_child(new Monkey());
	root->add_child(new Monkey());
	root->add_child(new Monkey());
	root->children[2]->add_child(new Monkey());

	for(int i = 0; i < 10; i++) {
		Monkey * m = new Monkey();
		m->transform.model_matrix = Math::get_translation_matrix(Math::vec3(2,i,0));
		root->children[0]->add_child(m);
	}

	// Main loop
	SDL_Event event;
	bool running = true;
	bool debug_mode = false;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (debug_mode) {
				ImGui_ImplSDL2_ProcessEvent(&event);
			}
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_F3) {
					debug_mode = !debug_mode;
					SDL_ShowCursor(debug_mode);
				}
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
		if (!debug_mode) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			x -= SDL_State::state.width / 2;
			y -= SDL_State::state.height / 2;
			SDL_WarpMouseInWindow(SDL_State::state.window, SDL_State::state.width / 2, SDL_State::state.height / 2);
			camera.rotate(x, -y);
		}
		
		// Update
		update_entity_tree(root);
		
		// Render
		glClearColor(0.7,0.8,0.9,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_entity_tree(root, &camera);

		// ImGui
		if (debug_mode) {
			draw_debug(root);
		}

		// Finish render
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


static int hierarchy_counter = 0;
void add_entity_to_hierarchy(Entity * root)
{
	hierarchy_counter++;
	char buffer[100];
	sprintf(buffer, "%s##%d",root->name, hierarchy_counter);
	
	if (ImGui::TreeNode(buffer)){
		//ImGui::Indent();
		//ImGui::Text("test");
		for(int i = 0; i < root->children.size; i++) {
			add_entity_to_hierarchy(root->children[i]);
		}
		//ImGui::Unindent();
		ImGui::TreePop();
	}
}

void draw_debug(Entity * root)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(SDL_State::state.window);
	ImGui::NewFrame();
	
	{	
		ImGui::SetNextWindowPos(ImVec2(0,0));
		ImGui::SetNextWindowSize(ImVec2(200,400));
		ImGui::Begin("Entity Hierarchy");
		add_entity_to_hierarchy(root);
		hierarchy_counter = 0;
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
