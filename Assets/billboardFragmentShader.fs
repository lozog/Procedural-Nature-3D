#version 330

in vec2 texCoords;

uniform sampler2D billboard;

out vec4 fragColor;

void main() {
	vec4 outColor = texture(billboard, texCoords);

	if (outColor.a <= 0.1f)
		discard;

	fragColor = outColor;
	// fragColor = vec4(texCoords.x, 0, texCoords.y, 1);
}