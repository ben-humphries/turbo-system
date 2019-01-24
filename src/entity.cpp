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

List<uint8_t> Entity::base_serialize()
{
	List<uint8_t> buffer;
	buffer.alloc();
	{ // name
		for (int i = 0; i < strlen(name); i++) {
			buffer.push(name[i]);
		}
		buffer.push(0x00);
	}
	return buffer;
}

void Entity::base_unserialize(uint8_t * buffer)
{
	{ // name
		List<char> name_buffer;
		name_buffer.alloc();
		size_t i = 0;
		while (buffer[i] != 0x00) {
			name_buffer.push(buffer[i++]);
		}
		name_buffer.push('\0');
		this->name = name_buffer.arr;
	}
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
