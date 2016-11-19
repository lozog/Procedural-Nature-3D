#pragma once

// from SimplexNoise1234 by Stefan Gustavson
// see http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )

class Perlin {
public:
	Perlin() {}
	~Perlin() {}
    static double noise( double x, double y );
    static double dot( int g[], double x, double y );	// fast 2D dot product
    static double fade( double t );

};