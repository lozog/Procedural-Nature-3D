#pragma once

// from SimplexNoise1234 by Stefan Gustavson
// see http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )

const static int LATTICE_SIZE = 16;
const static int LATTICE_AREA = LATTICE_SIZE*LATTICE_SIZE;
const static int LATTICE_MASK = LATTICE_AREA - 1;

const double PI = 3.14159265f;

class Vec2D {
public:
	Vec2D(double x, double y) : x(x), y(y) {}
	Vec2D() {}
	~Vec2D() {}
	void normalize();
	double x;
	double y;
};

class Perlin {
public:
	Perlin() {}
	~Perlin() {
		delete [] grads;
	}

    static double noise( double x, double y );

private:
	static double dot( Vec2D a, Vec2D b );
    static double fade( double t );
	static int hash( int x, int y );

	static int permuteTable[512];
	static Vec2D* grads;

};