#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 grass_position;
uniform vec3 cameraUp;
uniform vec3 cameraRight;
in vec3 position;
in vec2 texture;

out vec2 texCoords;

void main() {
	vec2 size = vec2(2.0f, 1.0f);
	texCoords = texture;
	// vec3 camRightWorldSpace = vec3(V[0][0], V[1][0], V[2][0]);
	// vec3 camUpWorldSpace = vec3(V[0][1], V[1][1], V[2][1]);
	vec3 vertPosition = grass_position
					+ -cameraRight *  (position.x * size.x)
					+ cameraUp	 *  (position.y * size.y);
	gl_Position = P * V * M * vec4(vertPosition, 1.0);
	// gl_Position = vec4(vertPosition, 1.0);
}
