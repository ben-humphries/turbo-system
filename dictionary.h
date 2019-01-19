#pragma once
#include "list.h"

struct Dictionary {

	List<char *> names;
	List<void *> resources;

	void alloc();
	
	void add(char * name, void * resource);
	void * lookup(char * name);
};
