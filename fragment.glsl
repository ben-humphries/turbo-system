#version 330 core

in vec2 texCoord;
in float distance_from_zero;

out vec4 fragColor;

void main() {
	fragColor = vec4(0.5, 0.5, 1.0, 1.0) * distance_from_zero;
}
