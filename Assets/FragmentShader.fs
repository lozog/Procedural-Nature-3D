#version 330

in vec3 pos;
in vec3 norm;
in vec4 col;
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

uniform vec3 globalAmbientLight;
uniform vec3 eye;

void main() {
	// map colour based on height
	// if ( pos.y < 0 )
	// 	fragColor = vec4( 0, 0, norm.y, 1 );
	// else
	// 	fragColor = vec4( pos.y/5.0f, 1.0f/pos.y, 0, 1 );

	// map colour based on height & normals
	// fragColor = vec4(vec3((pos.y/5.0f), 0, 0)+norm, 1);
	// vec3 terrainCol = vec3((pos.y/5.0f), 0, 0) + norm;

	// directional light from the Sun
	DirLight theSun;
	theSun.colour = theSunColour;
	theSun.dir = normalize(theSunDir);
	theSun.intensity = theSunIntensity;

	// float kd = max(0.0, dot(norm, -theSun.dir));
	// vec4 temp = terrainCol * vec4( theSun.colour*(theSun.intensity + kd), 1);
	// fragColor = terrainCol * vec4( theSun.colour*(theSun.intensity + kd), 1);
	// fragColor = texture(theTexture, 0.05f*tex) * vec4( theSun.colour*(theSun.intensity + kd), 1);
	// fragColor = texture(theTexture, tex) + 0.001f*temp;

	vec4 sunlight = vec4(theSun.intensity * theSun.colour, 1);
	vec3 lightDir = normalize(theSun.dir - pos);

	// diffuse light
	vec3 normal = normalize(norm);
	float diffterm = max(0.0f, dot(normal, lightDir));
	// no attenuation of direction lights
	float kd = 0.8f; // hard-code kd of object for now
	vec4 diffuse = sunlight * diffterm + (0.000001f*col);

	// specular light
	vec3 h = normalize(normalize(eye) + lightDir);
	float ks = 0.5f;
	float specterm = pow(max(0.0, dot(h, normal)), 3);
	vec4 spec = specterm * sunlight * ks;


	// ambient light
	vec4 ambient = vec4(0.8f * globalAmbientLight, 1);

	// float scale = 0.0000001;
	float scale = 1;
	fragColor = texture(theTexture, 0.05f*tex)*(scale*ambient + diffuse + scale*spec);

}
