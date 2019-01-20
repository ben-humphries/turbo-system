#include "dictionary.h"
#include <string.h>

void Dictionary::alloc()
{
	names.alloc();
	resources.alloc();
}

void Dictionary::add(const char * name, void * resource)
{
	names.push(name);
	resources.push(resource);
}

void * Dictionary::lookup(const char * name)
{
	for (int i = 0; i < names.size; i++) {
		if (strcmp(names[i], name) == 0) {
			return resources[i];
		}
	}

	return NULL;
}
