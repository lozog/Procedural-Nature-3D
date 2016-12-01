#version 330

in vec2 texCoords;

uniform sampler2D billboard;

out vec4 fragColor;

void main() {
	vec4 outColor = texture(billboard, texCoords);
	fragColor = outColor + vec4(0.1f, 0.0f, 0.0f, 0.0f);
	// fragColor = texture(billboard, texCoords);
}