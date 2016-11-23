#version 330

in vec3 pos;
in vec3 norm;

out vec4 fragColor;

void main() {
	// if ( pos.y < 0 )
	// 	fragColor = vec4( 0, 0, norm.y, 1 );
	// else
	// 	fragColor = vec4( pos.y/5.0f, 1.0f/pos.y, 0, 1 );

	fragColor = vec4(vec3((pos.y/5.0f), 0, 0)+norm, 1);
}
