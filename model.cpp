#include "model.h"
#include "mesh.h"

#include <stdio.h>

#include <GL/glew.h>

void load_model(Model * model){

	//initialize buffer and array objects
	glGenVertexArrays(1, &model->vao);
	glGenBuffers(1, &model->vbo);
	glGenBuffers(1, &model->ebo);

	//bind vao
	glBindVertexArray(model->vao);

	//bind vbo and assign vertex and texture data
	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	/*
	size_t vertices_and_tex_coords_count = model->mesh->vertices_count + model->mesh->texture_coordinates_count;
	
	model->vertices_and_tex_coords = (float*) malloc(vertices_and_tex_coords_count * sizeof(float));

	for(int i = 0; i < vertices_and_tex_coords_count; i+=5){
		
		model->vertices_and_tex_coords[i]   = model->mesh->vertices[i - 2*(i/5)];
		model->vertices_and_tex_coords[i+1] = model->mesh->vertices[i - 2*(i/5) + 1];
		model->vertices_and_tex_coords[i+2] = model->mesh->vertices[i - 2*(i/5) + 2];

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
}

void bind_model(Model * model){

	glBindVertexArray(model->vao);
}

void render_model(Model * model){
	bind_model(model);
	glDrawElements(GL_TRIANGLES, model->mesh->indices_count, GL_UNSIGNED_INT, 0);
}

void cleanup_model(Model * model){

	free(model->vertices_and_tex_coords);

	glBindVertexArray(model->vao);
	glDeleteBuffers(1, &model->vbo);
	glDeleteBuffers(1, &model->ebo);
	glDeleteVertexArrays(1, &model->vao);
}


