#version 330

in vec3 pos;
in vec3 norm;

out vec4 fragColor;

struct DirLight {
	vec3 colour;
	vec3 dir;
	float intensity;
};

void main() {
	// map colour based on height
	// if ( pos.y < 0 )
	// 	fragColor = vec4( 0, 0, norm.y, 1 );
	// else
	// 	fragColor = vec4( pos.y/5.0f, 1.0f/pos.y, 0, 1 );

	// map colour based on height & normals
	// fragColor = vec4(vec3((pos.y/5.0f), 0, 0)+norm, 1);
	vec4 terrainCol = vec4(vec3((pos.y/5.0f), 0, 0)+norm, 1);

	DirLight theSun;
	theSun.colour = vec3(1, 0, 0);
	theSun.dir = vec3(1, 1, 0);
	theSun.intensity = 0.5f;

	// directional light from the Sun
	float kd = max(0.0, dot(norm, -theSun.dir));
	fragColor = terrainCol * vec4( theSun.colour*(theSun.intensity + kd), 1);
}
