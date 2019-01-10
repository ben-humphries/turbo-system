#include "list.h"

void test_lists()
{
	List<int> list;
	list.alloc();

	for (int i = 0; i < 100; i++) {
		list.push(i);
	}

	assert(list.size == 100);
	for (int i = 0; i < list.size; i++) {
		assert(list[i] == i);
	}

	list[26] = 12;
	assert(list[26] == 12);

	for (int i = 0; i < 99; i++) {
		list.pop();
	}

	assert(list.size == 1);
	
	list.dealloc();
}
