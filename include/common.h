#pragma once

char * load_string_from_file(const char * path);

namespace Math {

	struct vec3 {
		float x;
		float y;
		float z;

		vec3(float value);

		vec3(float xv, float yv, float zv);
	};

	struct mat4 {
		float values[16];

		mat4();

		float get(int x, int y);
		float set(int x, int y, float val);
		float * get_ptr();
	};
	
	void test_mat4();

}//namespace Math
