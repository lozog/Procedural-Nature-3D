#version 330

in vec3 pos;
in vec3 norm;
in vec2 tex;

uniform sampler2D theTexture;

out vec4 fragColor;

struct DirLight {
	vec3 colour;
	vec3 dir;
	float intensity;
};

// I didn't feel like figuring out how to pass a struct uniform
uniform vec3 theSunColour;
uniform vec3 theSunDir;
uniform float theSunIntensity;

void main() {
	// map colour based on height
	// if ( pos.y < 0 )
	// 	fragColor = vec4( 0, 0, norm.y, 1 );
	// else
	// 	fragColor = vec4( pos.y/5.0f, 1.0f/pos.y, 0, 1 );

	// map colour based on height & normals
	// fragColor = vec4(vec3((pos.y/5.0f), 0, 0)+norm, 1);
	vec4 terrainCol = vec4(vec3((pos.y/5.0f), 0, 0)+norm, 1);

	// directional light from the Sun
	DirLight theSun;
	theSun.colour = theSunColour;
	theSun.dir = theSunDir;
	theSun.intensity = theSunIntensity;

	float kd = max(0.0, dot(norm, -theSun.dir));
	vec4 temp = terrainCol * vec4( theSun.colour*(theSun.intensity + kd), 1);
	// fragColor = terrainCol * vec4( theSun.colour*(theSun.intensity + kd), 1);
	fragColor = texture(theTexture, tex) * vec4( theSun.colour*(theSun.intensity + kd), 1);
	// fragColor = texture(theTexture, tex) + 0.001f*temp;
}
