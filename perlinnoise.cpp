#include "perlinnoise.hpp"

#include <assert.h> // assert
#include <cmath> // fmod
#include <iostream>
using namespace std;

template<typename T = double>
inline T lerp(const T& lo, const T& hi, const T& t) {
	return lo * (1 - t) + hi * t;
}

double Perlin::grads[8][2] = {  {1,0},
								{cos45,sin45},
								{0,1},
								{-cos45,sin45},
								{-1,0},
								{-cos45,-sin45},
								{0,-1},
								{cos45,-sin45} };

// hash lookup table, from Ken Perlin's original implementation
int Perlin::perms[256] = {
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

double Perlin::rand2D[LATTICE_AREA] = {
	0.36f, 0.54f, 0.12f, 0.93f, 0.42f, 0.73f, 0.23f, 0.84f
};

double Perlin::terrain( int x, int y, int w, int h ) {

	// map coords of x,y point on terrain to the noise grid

	double gridX = (double)x / (double)w;
	double gridY = (double)y / (double)h;
	gridX *= 10;
	gridY *= 10;

	cout << gridX << " " << gridY << endl;

	return simpleNoise( gridX, gridY );
	// return noise( gridX, gridY );
}

double Perlin::smoothStep( double t ) {
	assert(t >= 0 && t <= 1 );
	return (1 - cos( t * PI )) * 0.5;
}

double Perlin::simpleNoise( double x, double y ) {

	int xi = FASTFLOOR(x);
	int yi = FASTFLOOR(y);
	int xMin = xi % LATTICE_SIZE;
	int yMin = yi % LATTICE_SIZE;
	int xMax = (xMin + 1) % LATTICE_SIZE;
	int yMax = (yMin + 1) % LATTICE_SIZE;

	double u = x - xi;
	double v = y - yi;
	// cout << x << ", " << xi << endl;

	assert(xMin < LATTICE_SIZE);
	assert(yMin < LATTICE_SIZE);
	// cout << fmod(rand1D[xMin], LATTICE_SIZE) << endl;

	// u = smoothStep( u );
	// v = smoothStep( v );

	double c00 = rand2D[xMin];
	double c01 = rand2D[xMax];
	double c10 = rand2D[yMin];
	double c11 = rand2D[yMax];

	double nx0 = lerp( c00, c10, u );
	double nx1 = lerp( c01, c11, u );

	return lerp( nx0, nx1, v );
}

double Perlin::noise( double x, double y ) {

	// P = (x, y)

	// find unit grid cell containing P
	int x0 = FASTFLOOR(x);
	int y0 = FASTFLOOR(y);
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	// cout << x << ", " << y << endl;
	// cout << x0 << ", " << y0 << endl;
	// cout << x1 << ", " << y1 << endl;

	// get gradients at each corner

	// hash function
	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	// TODO: gradients must be pseudo-randomly chosen - hash fn?
	double* grad00 = Perlin::grads[1];
	double* grad01 = Perlin::grads[3];
	double* grad10 = Perlin::grads[5];
	double* grad11 = Perlin::grads[7];

	#if 1
	/*
	|---x0----x-------x1--
	|   |-----| <- this interval is u
	used for lerp
	*/
	double u = x - x0;
	double v = y - y0;

	// cout << u << ", " << v << endl;

	// calculate noise contributions from the 4 corners
	double n00 = dot(grad00, u, v);
	double n01 = dot(grad01, u, v-1);
	double n10 = dot(grad10, u-1, v);
	double n11 = dot(grad11, u-1, v-1);

	// interpolate noise contributions for x and y
	double fu = fade(u);
	double fv = fade(v);

	double nx0 = n00*(1-fu) + n10*fu;
	double nx1 = n01*(1-fu) + n11*fu;
	double nxy = nx0*(1-fv) + nx1*fv;

	// cout << nxy << endl;
	return nxy;
	#else
	return 0.0f;
	#endif
}

double Perlin::dot( double g[], double x, double y ) {
	return g[0]*x + g[1]*y;
}


double Perlin::fade( double t ) {
	// 6*t^5 - 15*t^4 + 10*t^3
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}