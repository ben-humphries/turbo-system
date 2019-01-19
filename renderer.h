#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera; // "camera.h"

namespace Renderer {
	struct Mesh {
		float * vertices;
		size_t vertices_count;
		int * indices;
		size_t indices_count;
		float * texture_coordinates;
		size_t texture_coordinates_count;
	};

	struct Model {
		Mesh * mesh;
		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;
		//float * vertices_and_tex_coords;
		static Model create(Mesh * mesh);
		void bind();
		void render();
		void cleanup();
	};
	
	struct Texture {
		GLuint texture;
		GLenum texture_type;
		void use_texture();
		static Texture load_from_file(const char * texture_path, GLenum texture_type = GL_TEXTURE_2D);
	};
	
	struct Shader {
		const char * vertex_path;
		const char * fragment_path;
		GLuint program;

		void set_mat4_uniform(const char * uniform_name, glm::mat4 value);
		void use_program();
	
		static Shader load_from_source(const char * vertex_path,
									   const char * fragment_path);
	};

	void initialize_renderer();
	void render(Model model, Texture texture, Shader shader, Camera * camera);
}
