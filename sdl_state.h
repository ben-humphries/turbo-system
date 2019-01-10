#pragma once

#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

struct SDL_State {
	int width;
	int height;
	SDL_Window * window;
	SDL_GLContext gl_context;
	uint64_t last_dt_count;
	float delta_time;
	void init(int width, int height);
	static SDL_State state;
};
