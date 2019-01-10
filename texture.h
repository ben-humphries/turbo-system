#pragma once

#include <GL/glew.h>

struct Texture {
	GLuint texture;
	GLenum texture_type;
	void use_texture();
	static Texture load_from_file(const char * texture_path, GLenum texture_type = GL_TEXTURE_2D);
};
