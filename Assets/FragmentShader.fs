#version 330

uniform vec3 colour;

out vec4 fragColor;

void main() {
	fragColor = vec4( colour, 1 );
}
