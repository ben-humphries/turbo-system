#pragma once

#include "entity.h"

void serialize_tree_to_file(Entity * entity,
							const char * path);

Entity * unserialize_tree_from_file(const char * path);
