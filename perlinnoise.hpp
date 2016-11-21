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

class Vec2D {
public:
	Vec2D(double x, double y) : x(x), y(y) {}
	Vec2D() {}
	~Vec2D() {}
	double x;
	double y;
};

class Perlin {
public:
	Perlin() {
		// generateGrads();
	}
	~Perlin() {
		delete [] grads;
	}
    static double noise( double x, double y );
    static double dot( Vec2D a, Vec2D b );
    static double fade( double t );
	static double smoothStep( double t ); // general "fade" function

    static double simpleNoise( double x, double y );
    // static void generateGrads();

    static double terrain( int x, int y, int w, int h, double& maxVal );
// private:
	static int permuteTable[512];
	static double rand2D[LATTICE_AREA];
	static Vec2D* grads;

};