#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
in vec3 position;
in vec3 normal;
in vec2 texture;

out vec3 pos;
out vec3 norm;
out vec2 tex;

void main() {
	gl_Position = P * V * M * vec4(position, 1.0);
	pos = position;
	norm = normal;
	tex = texture;
}
