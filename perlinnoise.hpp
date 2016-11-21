#pragma once

// from SimplexNoise1234 by Stefan Gustavson
// see http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )

const static double sin45 = 0.70710678118f;
const static double cos45 = sin45;

const static int LATTICE_SIZE = 16;
const static int LATTICE_AREA = LATTICE_SIZE*LATTICE_SIZE;
const static int LATTICE_MASK = LATTICE_AREA - 1;

const double PI = 3.14159265f;

class Perlin {
public:
	Perlin() {
		// generateRand2D();
	}
	~Perlin() {}
    static double noise( double x, double y );
    static double dot( double g[], double x, double y );
    static double fade( double t );
	static double smoothStep( double t ); // general "fade" function

    static double simpleNoise( double x, double y );
    // static void generateRand2D();

    static double terrain( int x, int y, int w, int h );
// private:
	// gradients are 8 evenly distributed directions around the unit circle
	static double grads[8][2];
	static int permuteTable[512];
	static double rand2D[LATTICE_AREA];

};