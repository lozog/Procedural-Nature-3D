#version 330

in vec4 col;
in vec2 texCoords;

uniform sampler2D billboard;
uniform sampler2D screendoor;

out vec4 fragColor;

void main() {
	vec4 screen = texture(screendoor, texCoords);
	if (screen.a < 0.6f) {
		// alpha test -> discards pixels with alpha less than 0.6
		discard;
	}
	if (screen.r < 0.5f) {
		// screen door effect -> alpha test against the screendoor texture
		discard;
	}
	vec4 outColor = texture(billboard, texCoords);
	fragColor = (0.65f*outColor) + (0.35f*col)/* - vec4(0.1f, 0.1f, 0.1f, 0.0f)*/;
	// fragColor = texture(billboard, texCoords);
}