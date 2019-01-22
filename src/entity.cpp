#include "entity.h"

void Transform::init()
{
	model_matrix = Math::mat4();
	position = Math::vec3();
}

void Transform::move(Math::vec3 movement)
{
	position = position + movement;
}

Math::mat4 Transform::get_model_matrix()
{
	return Math::get_translation_matrix(position);
}

void Entity::base_initialize()
{
	name = "ROOT";
	shader = Renderer::Shader::get_shader("default", "res/vertex.glsl", "res/fragment.glsl");
	parent = NULL;
	children.alloc();
}

void Entity::add_child(Entity * entity)
{
	children.push(entity);
	entity->base_initialize();
	entity->initialize();
	entity->parent = this;
}

void Entity::initialize() {}
void Entity::update() {}
void Entity::render(Camera * camera) {}

void update_entity_tree(Entity * root)
{
	root->update();
	for (int i = 0; i < root->children.size; i++) {
		update_entity_tree(root->children[i]);
	}
}

void render_entity_tree(Entity * root, Camera * camera)
{
	root->render(camera);
	for (int i = 0; i < root->children.size; i++) {
		render_entity_tree(root->children[i], camera);
	}
}
