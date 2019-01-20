#version 330 core

in vec2 texCoord;

uniform sampler2D  aTexture;

out vec4 fragColor;

void main() {
	fragColor = texture(aTexture, texCoord);
}
