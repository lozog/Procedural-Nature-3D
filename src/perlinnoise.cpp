#include "perlinnoise.hpp"

#include <assert.h> 	// assert
#include <cmath>		// sqrt
#include <iostream>
using namespace std;

template<typename T = double>
inline T lerp(const T& lo, const T& hi, const T& t) {
	return lo * (1 - t) + hi * t;
}

// hash lookup table, from Ken Perlin's original implementation
int Perlin::permuteTable[512] = {
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
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
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

void Vec2D::normalize() {
	double length = sqrt( (x*x) + (y*y) );
	x /= length;
	y /= length;
}

// factory function for gradients
// which are distributed around the unit circle
static Vec2D* generateGrads() {
	const unsigned int NUMGRADS = 8; // 8 because 2D noise
	Vec2D* grads = new Vec2D[NUMGRADS];

	grads[0] = Vec2D(0, 1);
	grads[1] = Vec2D(0, -1);
	grads[2] = Vec2D(1, 0);
	grads[3] = Vec2D(-1, 0);
	grads[4] = Vec2D(1, 1);
	grads[5] = Vec2D(1, -1);
	grads[6] = Vec2D(-1, 1);
	grads[7] = Vec2D(-1, -1);

	for (unsigned int i = 0; i < NUMGRADS; i += 1) {
		grads[i].normalize();
	} // for

	return grads;
}

Vec2D* Perlin::grads = generateGrads();

int Perlin::hash( int x, int y ) {
	return permuteTable[permuteTable[x] + y] & 7; // mod 7 because there are 8 gradients
}

double Perlin::noise( double x, double y ) {
	int xi = FASTFLOOR(x);
	int yi = FASTFLOOR(y);
	// cout << x << ", " << xi << endl;

	// We can use the & operator as a mod function since we chose the lattice size
	// to be a power of two.
	int xMin = xi & LATTICE_MASK;
	int yMin = yi & LATTICE_MASK;
	int xMax = (xMin + 1) & LATTICE_MASK;
	int yMax = (yMin + 1) & LATTICE_MASK;

	// sanity check
	assert(xMin < LATTICE_AREA);
	assert(yMin < LATTICE_AREA);

	// indices of gradients at four corners
	int g00 = hash(xMin, yMin);
	int g10 = hash(xMax, yMin);
	int g01 = hash(xMin, yMax);
	int g11 = hash(xMax, yMax);

	// calculate parameters for lerp
	double tx = x - xi;
	double ty = y - yi;
	// cout << tx << " " << ty << endl;

	// take dot products to find values to lerp
	double n00 = dot(grads[g00], Vec2D(tx	 , ty    ));
	double n10 = dot(grads[g10], Vec2D(tx - 1, ty    ));
	double n01 = dot(grads[g01], Vec2D(tx	 , ty - 1));
	double n11 = dot(grads[g11], Vec2D(tx - 1, ty - 1));

	// remap lerp parameters using smoothstep function
	double u = fade( tx );
	double v = fade( ty );

	// lerp along x axis
	double nx0 = lerp( n00, n10, u );
	double nx1 = lerp( n01, n11, u );

	// increasing scaling factor yields a larger range of heights
	double scale = 10.0f;

	// lerp along remaining (y) axis
	return scale * lerp( nx0, nx1, v );
}

double Perlin::dot( Vec2D a, Vec2D b ) {
	return a.x*b.x + a.y*b.y;
}

// fade function used by Perlin's improved algorithm
double Perlin::fade( double t ) {
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}