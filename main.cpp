#define GLEW_STATIC

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdio.h>

int main(){

	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window * window = SDL_CreateWindow("OpenGL", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	printf("%s\n", glewGetErrorString(err));


	glm::mat4 view(1);
	glm::mat4 proj(1);
	glm::mat4 model(1);

	view = glm::translate(view, glm::vec3(0,0,-3));
	projection = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 300.0f);
	
	SDL_Event windowEvent;
	while (true){
		if (SDL_PollEvent(&windowEvent)){
			if (windowEvent.type == SDL_QUIT) break;
		}

		GLuint vertexBuffer;
		glGenBuffers(1, &vertexBuffer);
		printf("%u\n", vertexBuffer);

		SDL_GL_SwapWindow(window);
	}
	
	SDL_GL_DeleteContext(context);
	return 0;
}
