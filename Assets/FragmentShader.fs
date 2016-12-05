#version 330

in vec3 pos;
in vec3 norm;
in vec4 col;
in vec2 tex;
in vec4 posLightspace;

uniform sampler2D groundTexture;
uniform sampler2D cliffTexture;
uniform sampler2D shadowMap;

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
	float PI = 3.14159f;

	// directional light from the Sun
	DirLight theSun;
	theSun.colour = theSunColour;
	theSun.dir = normalize(theSunDir);
	theSun.intensity = theSunIntensity;

	vec4 sunLightColour = vec4(theSun.intensity * theSun.colour, 1);
	vec3 lightDir = normalize(theSun.dir);

	// note: no attenuation of direction lights

	vec3 normal = normalize(norm);

	// diffuse light
	float diffterm = max(0.0f, dot(normal, lightDir));
	vec4 diffuse = col * sunLightColour * diffterm;

	// specular light
	vec3 h = normalize(normalize(eye) + lightDir);
	float ks = 0.2f;
	float specterm = pow(max(0.0, dot(h, normal)), 5);
	vec4 spec = specterm * sunLightColour * ks;

	// ambient light
	vec4 ambient = vec4(0.8f * globalAmbientLight, 1);

	// shadow
	// float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);
	float bias = 0.005f; // I've found that this bias is sufficient to eliminate shadow acne with minimal to no peter panning
	float shadow = shadowCalc(posLightspace, bias);
	// shadow -= 0.9f;

	// float scale = 0.0000001;
	// float scale = 1;
	vec4 texture1 = texture(groundTexture, 0.05f*tex);
	vec4 texture2 = texture(cliffTexture, 0.05f*tex);
	vec4 finaltexture = texture1;
	float angleOfGround = acos(dot(normal, vec3(0.0f, 1.0f, 0.0f)));
	angleOfGround -= 0.225f;
	if ( angleOfGround > 1.0f ) angleOfGround = 1.0f;
	else if (angleOfGround < 0.0f ) angleOfGround = 0.0f;
	finaltexture = angleOfGround*texture2 + (1.0f - angleOfGround)*texture1;
	fragColor = finaltexture*(ambient + (1.0f - shadow)*(diffuse + spec));
}
