#include "common.h"

#include "renderer.h"
#include "camera.h"
#include "sdl-state.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Renderer {

	Dictionary Model::models;
	Dictionary Texture::textures;
	Dictionary Shader::shaders;
	
	/*
	 * Rendering
	 */
	
	glm::mat4 projection_matrix(1);

	void initialize_renderer()
	{
		projection_matrix = glm::perspective(glm::radians(45.0f),
											 (float) SDL_State::state.width / SDL_State::state.height,
											 0.1f, 300.0f);

		Model::models.alloc();
		Texture::textures.alloc();
		Shader::shaders.alloc();
	}
	
	void render(Model * model, Texture * texture, Shader shader, Camera * camera)
	{
		texture->use_texture();
		shader.use_program();
		shader.set_mat4_uniform("transform", projection_matrix * camera->get_view_matrix());
		model->render();
	}
	
	/*
	 * Shaders
	 */
	
	static void check_shader_compilation(GLuint shader) {
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			printf("ERROR: Shader Compilation failed...\n", infoLog);
			exit(1);
		}
	}

	static GLuint compile_shader(const char * path, GLenum type)
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
	/* TODO(ben-humphries): figure out how to make shaders work with this 
	Shader * Shader::get_shader(char * name)
	{
		Shader * shader = (Shader *) shaders.lookup(name);
		if(shader) {
			return shader;
		}

		Shader s = load_from_source(name + .vert, name+.frag)
	}
	*/
	/*
	 * Textures
	 */
	
	void Texture::use_texture()
	{
		glBindTexture(texture_type, texture);
	}

	Texture * Texture::load_from_file(const char * texture_path, GLenum texture_type)
	{
		int w, h, c;
		unsigned char * pixels = stbi_load(texture_path, &w, &h, &c, 4);
		assert(pixels);

		Texture * texture = (Texture *) malloc(sizeof(Texture));
		texture->texture_type = texture_type;
	
		glGenTextures(1, &texture->texture);
		glBindTexture(texture_type, texture->texture);

		// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		glTexImage2D(texture_type, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		return texture;
	}

	Texture * Texture::get_texture(char * name)
	{
		Texture * texture = (Texture *) textures.lookup(name);

		if(texture) {
			return texture;
		}

		Texture * t = load_from_file(name);
		textures.add(name, t);
		return t;
	}

	/*
	 * Models
	 */
	
	Model * Model::create(Mesh * mesh)
	{
		Model * model = (Model *) malloc(sizeof(Model));
		model->mesh = mesh;
		
		//initialize buffer and array objects
		glGenVertexArrays(1, &model->vao);
		glGenBuffers(1, &model->vbo);
		glGenBuffers(1, &model->ebo);

		//bind vao
		glBindVertexArray(model->vao);

		//bind vbo and assign vertex and texture data
		glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
		/*
		  size_t vertices_and_tex_coords_count = model.mesh->vertices_count + model.mesh->texture_coordinates_count;
	
		  model.vertices_and_tex_coords = (float*) malloc(vertices_and_tex_coords_count * sizeof(float));

		  for(int i = 0; i < vertices_and_tex_coords_count; i+=5){
		
		  model.vertices_and_tex_coords[i]   = model.mesh->vertices[i - 2*(i/5)];
		  model.vertices_and_tex_coords[i+1] = model.mesh->vertices[i - 2*(i/5) + 1];
		  model.vertices_and_tex_coords[i+2] = model.mesh->vertices[i - 2*(i/5) + 2];

		  model->vertices_and_tex_coords[i+3] = model->mesh->texture_coordinates[i - 3*(i/5)];
		  model->vertices_and_tex_coords[i+4] = model->mesh->texture_coordinates[i - 3*(i/5) + 1];
		  }
		*/
		//glBufferData(GL_ARRAY_BUFFER, vertices_and_tex_coords_count * sizeof(float), model->vertices_and_tex_coords, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, model->mesh->vertices_count * sizeof(float), model->mesh->vertices, GL_STATIC_DRAW);
	
		//bind ebo and assign index data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->mesh->indices_count * sizeof(int), model->mesh->indices, GL_STATIC_DRAW);

		//vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
		glEnableVertexAttribArray(0);

		//texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//unbind vao
		glBindVertexArray(0);

		return model;
	}

	void Model::bind()
	{
		glBindVertexArray(vao);
	}

	void Model::render()
	{
		bind();
		glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_INT, 0);
	}

	void Model::cleanup()
	{
		//free(vertices_and_tex_coords);

		glBindVertexArray(vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
	}	
}
