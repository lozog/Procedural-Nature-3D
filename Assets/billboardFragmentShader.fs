#version 330

in vec2 texCoords;

uniform sampler2D billboard;

out vec4 fragColor;

void main() {
	fragColor = texture(billboard, texCoords);
	// fragColor = vec4(texCoords.x, 0, texCoords.y, 1);
}