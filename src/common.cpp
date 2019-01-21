#include <stdlib.h>
#include <stdio.h>

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

	struct vec3 {
		float x;
		float y;
		float z;

		vec3(float value)
		{
			x = value;
			y = value;
			z = value;
		}

		vec3(float xv, float yv, float zv)
		{
			x = xv;
			y = yv;
			z = zv;
		}
	};

	struct mat4 {

		float values[16];

		mat4()
		{
			for(int i = 0 ; i < 16; i++){
					values[i] = 0;
			}

			values[0]  = 1;
			values[5]  = 1;
			values[10] = 1;
			values[15] = 1;
		}

		float get(int x, int y)
		{
			return values[y + 4 * x];
		}

		void set(int x, int y, float val)
		{
			values[y + 4 * x] = val;
		}

		float * get_ptr()
		{
			return values;
		}
	};

	void test_mat4()
	{
		mat4 test = mat4();

		test.set(3, 0, 5);

		for(int x = 0; x < 4; x ++){
			for(int y = 0; y < 4; y ++){
				printf("%f ", test.get(y, x));
			}
			printf("\n");
		}

		float * ptr = test.get_ptr();

		for(int i = 0; i < 16; i++){
			printf("%f ", ptr[i]);
		}
	}
		
} //namespace Math

