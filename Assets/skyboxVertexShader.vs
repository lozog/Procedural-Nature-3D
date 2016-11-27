#version 330

uniform mat4 P;
uniform mat4 V;
// uniform mat4 M;
in vec3 position;

out vec3 texCoords;

void main() {
	texCoords = position;
	// gl_Position = P * V * M * vec4(position, 1.0);
	gl_Position = P * V * vec4(position, 1.0);
	// gl_Position = V * vec4(position, 1.0);
	// gl_Position = vec4(position, 1.0);
}
