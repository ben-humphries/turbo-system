#pragma once

#include "common.h"
#include "renderer.h"

struct Transform {
	Math::mat4 model_matrix;
	
	void init();
	void move(Math::vec3 movement);
};

struct Entity {
	const char * name = "default-name";
	Transform transform;
	Renderer::Shader * shader;

	Entity * parent;
	List<Entity*> children;
	
	void base_iI figure the default can just be the nitialize();
	void add_child(Entity * entity);
	virtual void initialize();
	virtual void update();
	virtual void render(Camera * camera);
};

void update_entity_tree(Entity * root);
void render_entity_tree(Entity * root, Camera * camera);
