#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform mat4 lightProj;
uniform mat4 lightView;

in vec3 position;
in vec3 normal;
in vec4 colour;
in vec2 texture;

out vec3 pos;
out vec3 norm;
out vec4 col;
out vec2 tex;
out vec4 posLightspace;

void main() {
	// regular ol' vertex calculations
	gl_Position = P * V * M * vec4(position, 1.0);
	pos = vec3(M * vec4(position, 1.0f));
	// pos = position;
	norm = mat3(transpose(inverse(M))) * normal;
	col = colour;
	tex = texture;

	// shadow calculations
	posLightspace = lightProj * lightView *  M * vec4( position, 1.0f);
}
