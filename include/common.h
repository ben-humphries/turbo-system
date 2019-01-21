#pragma once

char * load_string_from_file(const char * path);

namespace Math {

	struct vec3 {
		float x;
		float y;
		float z;

		vec3();
		
		vec3(float value);

		vec3(float xv, float yv, float zv);

		vec3 operator+(const vec3 & v) const;
		vec3 operator-(const vec3 & v) const;
		vec3 operator*(const vec3 & v) const;
		vec3 operator/(const vec3 & v) const;
	};

	struct mat4 {
		float values[16];

		mat4();

		float get(int x, int y);
		void set(int x, int y, float val);
		float * get_ptr();

		mat4 operator*(mat4 m);
	};

	float radians(float degrees);

	float dot(const vec3 & a, const vec3 & b);
	vec3 cross(const vec3 & a, const vec3 & b);
	vec3 normalize(const vec3 & v);

	mat4 look_at(const vec3 & eye, const vec3 & center, const vec3 & up);
	mat4 perspective(float fov, float aspect, float near, float far);
	mat4 get_translation_matrix(const vec3 & v);
	//TODO(ben-humphries): make this use euler angles
	mat4 get_rotation_matrix(const vec3 & axis, const float theta);
	mat4 get_scale_matrix(const vec3 & v);
	
	mat4 get_model_matrix(
		mat4 & translation_matrix,
		mat4 & rotation_matrix,
		mat4 & scale_matrix);

	void print_mat4(mat4 & m);
	void print_vec3(vec3 & v);
	
	void test_mat4();
	void test_vec3();

}//namespace Math
