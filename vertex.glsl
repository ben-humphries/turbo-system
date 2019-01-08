#version 330 core

in vec3 position;
in vec2 _texCoord;

uniform mat4 transform; // model/view/proj

out vec2 texCoord;
out float distance_from_zero;

void main() {

	 distance_from_zero = 1 - position.y - position.x;
	 texCoord = _texCoord;
	 gl_Position = transform * vec4(position, 1);
}
