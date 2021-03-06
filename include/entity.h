#pragma once

#include "common.h"
#include "renderer.h"

struct Transform {
	Math::mat4 model_matrix;

	Math::vec3 position;
	
	void init();
	void move(Math::vec3 movement);
	Math::mat4 get_model_matrix();
};

struct Entity {
	const char * name = "default-name";
	Transform transform;
	Renderer::Shader * shader;

	Entity * parent;
	List<Entity*> children;
	
	void base_initialize();
	void add_child(Entity * entity);
	virtual void initialize();
	virtual void update();
	virtual void render(Camera * camera);
};

void update_entity_tree(Entity * root);
void render_entity_tree(Entity * root, Camera * camera);
