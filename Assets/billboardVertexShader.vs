#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 grass_position;
uniform vec3 cameraUp;
uniform vec3 cameraRight;
in vec3 position;
in vec4 colour;
in vec2 texture;

out vec4 col;
out vec2 texCoords;

void main() {
	vec2 size = vec2(4.0f, 2.0f);
	col = colour;
	texCoords = texture;
	vec3 vertPosition = grass_position
					+ -cameraRight *  (position.x * size.x)
					+ cameraUp	 *  (position.y * size.y);
	gl_Position = P * V * M * vec4(vertPosition, 1.0);
}
