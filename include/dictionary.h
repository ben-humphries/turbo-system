#pragma once
#include "list.h"

struct Dictionary {

	List<const char *> names;
	List<void *> resources;

	void alloc();
	
	void add(const char * name, void * resource);
	void * lookup(const char * name);
};
