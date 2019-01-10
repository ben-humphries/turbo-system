#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Shader {
	const char * vertex_path;
	const char * fragment_path;
	GLuint program;

	void set_mat4_uniform(const char * uniform_name, glm::mat4 value);
	void use_program();
	
	static Shader load_from_source(const char * vertex_path,
								   const char * fragment_path);
};

// Shader::load_from_source(...);
