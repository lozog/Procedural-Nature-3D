#version 330

in vec3 texCoords;

uniform samplerCube skybox;

out vec4 fragColor;

void main() {
	fragColor = texture(skybox, texCoords);
}