#version 330

in vec3 pos;
in vec3 norm;
in vec4 col;
in vec2 tex;
in vec4 posLightspace;

uniform sampler2D theTexture;
uniform sampler2D shadowMap;
uniform samplerCube skybox;

uniform mat4 view;

out vec4 fragColor;

struct DirLight {
	vec3 colour;
	vec3 dir;
	float intensity;
};

uniform vec3 theSunColour;
uniform vec3 theSunDir;
uniform float theSunIntensity;

uniform vec3 globalAmbientLight;
uniform vec3 eye;

// returns 1.0 if fragment is in shadow, 0.0 otherwise
float shadowCalc(vec4 posLightspace, float bias) {
	vec3 projCoords = posLightspace.xyz / posLightspace.w; 				// perspective divide
	projCoords = projCoords * 0.5f + 0.5f;								// transform range to [0,1]
	float closestDepth = texture(shadowMap, projCoords.xy).r;			// get closest depth value from light's pov
	float currentDepth = projCoords.z;									// get depth of current fragment from light's pov
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;	// check whether current fragment is in shadow
	return shadow;
}

void main() {
	// directional light from the Sun
	DirLight theSun;
	theSun.colour = theSunColour;
	theSun.dir = normalize(theSunDir);
	theSun.intensity = theSunIntensity;

	vec4 sunLightColour = vec4(theSun.intensity * theSun.colour, 1);
	vec3 lightDir = normalize(theSun.dir);

	// note: no attenuation of direction lights

	// diffuse light
	vec3 normal = normalize(norm);
	float diffterm = max(0.0f, dot(normal, lightDir));
	vec4 diffuse = col * sunLightColour * diffterm;

	// specular light
	vec3 h = normalize(normalize(eye) + lightDir);
	float ks = 0.9f;
	float specterm = pow(max(0.0, dot(h, normal)), 2);
	vec4 spec = specterm * sunLightColour * ks;

	// ambient light
	vec4 ambient = vec4(0.8f * globalAmbientLight, 1);

	// shadow
	// float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);
	float bias = 0.005f; // I've found that this bias is sufficient to eliminate shadow acne with minimal to no peter panning
	float shadow = shadowCalc(posLightspace, bias);
	// shadow -= 0.9f;

	// skybox reflection
	vec3 I = normalize(eye - pos);
	vec3 R = reflect(I, normal);						// reflections on wrong side!
	vec3 R2 = reflect(R, vec3(0.0f, 0.0f, 1.0f));		// reflect on Z axis to correct

	fragColor = (0.2f * texture(theTexture, 0.05f*tex) + 0.8f * texture(skybox, R2))
				* (ambient + (1.0f - shadow)*(diffuse + spec));
}
