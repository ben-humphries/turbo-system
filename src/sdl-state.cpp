#include "sdl-state.h"

SDL_State SDL_State::state;

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
