#include "shader.h"

#include "common.h"

#include <stdio.h>
#include <stdlib.h>

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

void Shader::set_mat4_uniform(const char * uniform_name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(program, uniform_name),
					   1, GL_FALSE, glm::value_ptr(value));
}

void Shader::use_program()
{
	glUseProgram(program);
}

Shader Shader::load_from_source(const char * vertex_path,
								const char * fragment_path)
{
	Shader shader;
	shader.vertex_path = vertex_path;
	shader.fragment_path = fragment_path;

	shader.program = glCreateProgram();
	
	GLuint vertex = compile_shader(vertex_path, GL_VERTEX_SHADER);
	GLuint fragment = compile_shader(fragment_path, GL_FRAGMENT_SHADER);

	glAttachShader(shader.program, vertex);
	glAttachShader(shader.program, fragment);
	glLinkProgram(shader.program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return shader;
}
