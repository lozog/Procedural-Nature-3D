#pragma once

// from SimplexNoise1234 by Stefan Gustavson
// see http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )

const static double sin45 = 0.70710678118f;
const static double cos45 = sin45;

const static int PERLIN_GRID_SIZE = 16;

class Perlin {
public:
	Perlin() {}
	~Perlin() {}
    static double noise( double x, double y );
    static double dot( double g[], double x, double y );
    static double fade( double t );

    static double terrain( int x, int y, int w, int h );
// private:
	// gradients are 8 evenly distributed directions around the unit circle
	static double grads[8][2];
	static int perms[PERLIN_GRID_SIZE*PERLIN_GRID_SIZE];

};