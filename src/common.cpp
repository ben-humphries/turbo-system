#include <stdlib.h>
#include <stdio.h>

#include "common.h"

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>


char * load_string_from_file(const char * path)
{
	FILE * file = fopen(path, "r");
	if (file == NULL) return NULL;
	int file_len = 0;
	while (fgetc(file) != EOF) file_len++;
	char * str = (char*) malloc(file_len + 1);
	str[file_len] = '\0';
	fseek(file, 0, SEEK_SET);
	for (int i = 0; i < file_len; i++) str[i] = fgetc(file);
	fclose(file);
	return str;
}

namespace Math {

	vec3::vec3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	vec3::vec3(float value)
	{
		x = value;
		y = value;
		z = value;
	}
	
	vec3::vec3(float xv, float yv, float zv)
	{
		x = xv;
		y = yv;
		z = zv;
	}
	
	vec3 vec3::operator+(const vec3 & v) const
	{
		return vec3(
			this->x + v.x,
			this->y + v.y,
			this->z + v.z);
		
	}
	
	vec3 vec3::operator-(const vec3 & v) const
	{
		return vec3(
			this->x - v.x,
			this->y - v.y,
			this->z - v.z);
	}
	
	vec3 vec3::operator*(const vec3 & v) const
	{
		return vec3(
			this->x * v.x,
			this->y * v.y,
			this->z * v.z);
	}
	
	vec3 vec3::operator/(const vec3 & v) const
	{
		return vec3(
			this->x / v.x,
			this->y / v.y,
			this->z / v.z);
	}

	mat4::mat4()
	{
		for(int i = 0 ; i < 16; i++){
			values[i] = 0;
		}
		
		values[0]  = 1;
		values[5]  = 1;
		values[10] = 1;
		values[15] = 1;
	}
	
	float mat4::get(int x, int y)
	{
		return values[y + 4 * x];
	}
	
	void mat4::set(int x, int y, float val)
	{
		values[y + 4 * x] = val;
	}
	
	float * mat4::get_ptr()
	{
		return values;
	}
	
	mat4 mat4::operator*(mat4 m)
	{
		mat4 f = mat4();
		
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				float sum = 0.0f;
				for(int k = 0; k < 4; k++) {
					sum += this->get(i,k)*m.get(k,j);
				}
				
				f.set(i,j, sum);
			}
		}
		
		return f;
	}

	float radians(float degrees)
	{
		return degrees * 0.0174533f;
	}
	float dot(const vec3 & a, const vec3 & b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	vec3 cross(const vec3 & a, const vec3 & b)
	{
		return vec3(
			a.y*b.z - a.z*b.y,
			a.z*b.x - a.x*b.z,
			a.x*b.y - a.y*b.x);
	}

	vec3 normalize(const vec3 & v)
	{
		return v / sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	}

	mat4 look_at(const vec3 & eye, const vec3 & center, const vec3 & up)
	{
		mat4 m = mat4();


		vec3 x, y, z;
		
		z = normalize(eye - center);
		x = cross(up, z);
		y = normalize(cross(z, x));

		x = normalize(x);

		m.set(0,0, x.x);
		m.set(1,0, x.y);
		m.set(2,0, x.z);
		m.set(3,0, -dot(x, eye));

		m.set(0,1, y.x);
		m.set(1,1, y.y);
		m.set(2,1, y.z);
		m.set(3,1, -dot(y, eye));

		m.set(0,2, z.x);
		m.set(1,2, z.y);
		m.set(2,2, z.z);
		m.set(3,2, -dot(z, eye));
		

		return m;
	}

	mat4 perspective(float fov, float aspect, float near, float far)
	{
		mat4 m = mat4();

		float f = 1 / tan(fov/2);

		m.set(0,0, f / aspect);
		m.set(1,1, f);
		m.set(2,2, (far+near) / (near-far));
		m.set(3,2, (2*far*near) / (near-far));
		m.set(2,3, -1);
		m.set(3,3, 0);

		return m;
	}

	mat4 get_translation_matrix(const vec3 & v)
	{
		mat4 m = mat4();
		m.set(3,0, v.x);
		m.set(3,1, v.y);
		m.set(3,2, v.z);

		return m;
	}

	mat4 get_rotation_matrix(const vec3 & axis, const float theta)
	{
		//https://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations
		mat4 m = mat4();

		float costheta = cos(theta);
		float sintheta = sin(theta);
		
		
		m.set(0,0, costheta+axis.x*axis.x*(1-costheta));
		m.set(1,0, axis.x*axis.y*(1-costheta)-axis.z*sintheta);
		m.set(2,0, axis.x*axis.z*(1-costheta)+axis.y*sintheta);
		m.set(0,1, axis.y*axis.x*(1-costheta)+axis.z*sintheta);
		m.set(1,1, costheta+axis.y*axis.y*(1-costheta));
		m.set(2,1, axis.y*axis.z*(1-costheta)-axis.x*sintheta);
		m.set(0,2, axis.z*axis.x*(1-costheta)-axis.y*sintheta);
		m.set(1,2, axis.z*axis.y*(1-costheta)+axis.x*sintheta);
		m.set(2,2, costheta+axis.z*axis.z*(1-costheta));

		return m;
	}

	mat4 get_scale_matrix(const vec3 & v)
	{
		mat4 m = mat4();

		m.set(0,0, v.x);
		m.set(1,1, v.y);
		m.set(2,2, v.z);

		return m;
	}


	
	mat4 get_model_matrix(
		mat4 & translation_matrix,
		mat4 & rotation_matrix,
		mat4 & scale_matrix)
	{
		return translation_matrix * rotation_matrix * scale_matrix;
	}

	void print_mat4(mat4 & m)
	{
		for(int x = 0; x < 4; x ++){
			for(int y = 0; y < 4; y ++){
				printf("%f ", m.get(y, x));
			}
			printf("\n");
		}
		printf("\n");
	}

	void print_vec3(vec3 & v)
	{
		printf("%f %f %f\n\n", v.x, v.y, v.z);
	}

	void print_glmmat4(glm::mat4 & m)
	{
		for(int x = 0; x < 4; x ++){
			for(int y = 0; y < 4; y ++){
				printf("%f ", m[y][x]);
			}
			printf("\n");
		}
		printf("\n");
	}

	void print_glmvec3(glm::vec3 & v)
	{
		printf("%f %f %f\n\n", v.x, v.y, v.z);
	}

	//tested:
	//set/add
	//pointer
	//multiplication operator
	//look_at
	//perspective (and radians)
	//translation_matrix NOTE: doesn't work exactly as glm does. instead of adding, this is meant to be a one-time function call. Should store position separately and create matrices only when rendering.
	//rotation_matrix NOTE: same as translation_matrix.
	//scale_matrix NOTE: same as translation_matrix.
	void test_mat4()
	{
		//my mat4
		mat4 test = mat4();
		
		//glm mat4
		glm::mat4 glmtest = glm::mat4(1);

		test = get_scale_matrix(vec3(2,1,3));
		glmtest = glm::scale(glmtest, glm::vec3(2,1,3));
	
		
		print_mat4(test);
		print_glmmat4(glmtest);
	}

	//tested:
	//addition operator
	//subtraction operator
	//multiplication operator
	//division operator
	//dot product
	//cross product
	//normalize
	void test_vec3()
	{
		vec3 test = vec3(1,2,3);
		vec3 test2 = test;

		glm::vec3 glmtest = glm::vec3(1,2,3);
		glm::vec3 glmtest2 = glm::vec3(4,1,4);

		test = vec3(1,0,0) - vec3(0,1,0);
		glmtest = normalize(glmtest);
		
		print_vec3(test);
		print_glmvec3(glmtest);
	}
		
} //namespace Math

