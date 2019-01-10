#include "texture.h"

#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture::use_texture()
{
	glBindTexture(texture_type, texture);
}

Texture Texture::load_from_file(const char * texture_path, GLenum texture_type)
{
	int w, h, c;
	unsigned char * pixels = stbi_load(texture_path, &w, &h, &c, 4);
	assert(pixels);

	Texture texture;
	texture.texture_type = texture_type;
	
	glGenTextures(1, &texture.texture);
	glBindTexture(texture_type, texture.texture);

	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(texture_type, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	return texture;
}
