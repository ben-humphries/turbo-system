#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>

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

char * load_string_from_file(const char * path)
{
	FILE * file = fopen(path, "r");
	if (file == NULL) return NULL;
	int file_len = 0;
	while (fgetc(file) != EOF) file_len++;
	char * str = (char*) malloc(file_len + 1);
	str[file_len] = '\0';
	fseek(file, 0, SEEK_SET);
	for (int i = 0; i < file_len; i++) str[i] = fgetc(file);
	fclose(file);
	return str;
}

void check_shader_compilation(GLuint shader) {
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("ERROR: Shader Compilation failed...\n", infoLog);
		exit(1);
	}
}

GLuint compile_shader(const char * path, GLenum type)
{
	const char * source = load_string_from_file(path);
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	check_shader_compilation(shader);
	free((void*) source);
	return shader;
}

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
							  width, height, SDL_WINDOW_OPENGL);
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

const float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
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

int main()
{
	sdl_state.init(800, 600);
	
	glm::mat4 projection_matrix(1);
	projection_matrix = glm::perspective(glm::radians(45.0f), (float) sdl_state.width / sdl_state.height, 0.1f, 300.0f);

	Camera camera;
	camera.init(glm::vec3(0, 0, -3), 90, -20);
	
	// Load shaders
	GLuint program = glCreateProgram();
	{
		GLuint vertex = compile_shader("vertex.glsl", GL_VERTEX_SHADER);
		GLuint fragment = compile_shader("fragment.glsl", GL_FRAGMENT_SHADER);
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	glUseProgram(program);

	// Cube
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
	glEnableVertexAttribArray(0);

	bool directions[DIR_COUNT] = {0};
	
	SDL_Event event;
	bool running = true;
	while (running) {
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					directions[DIR_FORWARD] = true;
					break;
				case SDL_SCANCODE_A:
					directions[DIR_LEFT] = true;
					break;
				case SDL_SCANCODE_S:
					directions[DIR_BACK] = true;
					break;
				case SDL_SCANCODE_D:
					directions[DIR_RIGHT] = true;
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					directions[DIR_FORWARD] = false;
					break;
				case SDL_SCANCODE_A:
					directions[DIR_LEFT] = false;
					break;
				case SDL_SCANCODE_S:
					directions[DIR_BACK] = false;
					break;
				case SDL_SCANCODE_D:
					directions[DIR_RIGHT] = false;
					break;
				}
				break;
			}
		}

		for (int i = 0; i < DIR_COUNT; i++) {
			if (directions[i]) {
				camera.move_in_direction((Direction) i);
			}
		}
		
		// Set uniform
		glUniformMatrix4fv(glGetUniformLocation(program, "transform"),
						   1, GL_FALSE, glm::value_ptr(projection_matrix * camera.get_view_matrix()));
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		SDL_GL_SwapWindow(sdl_state.window);
		
		// delta time
		{
			uint64_t new_dt_count = SDL_GetPerformanceCounter();
			sdl_state.delta_time = (float) (new_dt_count - sdl_state.last_dt_count) / SDL_GetPerformanceFrequency();
			sdl_state.last_dt_count = new_dt_count;
			printf("%f                  \r", 1.0 / sdl_state.delta_time);
		}
	}
	
	SDL_GL_DeleteContext(sdl_state.gl_context);
	return 0;
}
