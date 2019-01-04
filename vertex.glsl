#version 330 core

in vec3 position;

uniform mat4 transform; // model/view/proj

void main() {
	gl_Position = transform * vec4(position, 1);
}
