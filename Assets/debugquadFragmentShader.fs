#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;

void main()
{
	float depthValue = texture(screenTexture, TexCoords).r;
    color = vec4(vec3(depthValue), 1.0f);
}