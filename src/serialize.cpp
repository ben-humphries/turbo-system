#include "serialize.h"

void serialize_tree_to_file(Entity * entity, const char * path)
{
	FILE * file = fopen(path, "w");
	
	List<uint8_t> base_serial = entity->base_serialize();
	fwrite(base_serial.arr, sizeof(uint8_t), base_serial.size, file);
	
	fclose(file);
}

Entity * unserialize_tree_from_file(const char * path)
{
	FILE * file = fopen(path, "r");

	fclose(file);
	
	Entity * root = new Entity();
	root->base_unserialize();
	
	return root;
}
