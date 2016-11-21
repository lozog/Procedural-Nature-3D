#include "perlinnoise.hpp"

#include <assert.h> 	// assert
#include <cmath>		// cos
#include <cstdlib>  	// rand
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

// 256 random numbers in the range (0, 1)
double Perlin::rand2D[256] = {
	0.0334699, 0.329964, 0.690636, 0.422487, 0.206265, 0.250128, 0.636559, 0.863622,
	0.301656, 0.0249239, 0.364993, 0.765381, 0.31786, 0.135728, 0.106745, 0.760672,
	0.0816723, 0.550956, 0.564652, 0.743271, 0.98176, 0.21885, 0.454396, 0.518654,
	0.573842, 0.555231, 0.728546, 0.628874, 0.796128, 0.583716, 0.274766, 0.829598,
	0.91368, 0.965402, 0.252085, 0.119945, 0.215531, 0.888643, 0.983568, 0.517186,
	0.913567, 0.34856, 0.282567, 0.231427, 0.484289, 0.389312, 0.992099, 0.565961,
	0.940269, 0.556751, 0.309231, 0.922029, 0.775601, 0.763627, 0.440683, 0.349442,
	0.318858, 0.169229, 0.978316, 0.114987, 0.752945, 0.253083, 0.944585, 0.666625,
	0.218485, 0.196669, 0.78657, 0.434016, 0.0853126, 0.770138, 0.951202, 0.99888,
	0.118698, 0.233769, 0.230307, 0.602987, 0.623081, 0.222406, 0.168947, 0.56335,
	0.779157, 0.478179, 0.485379, 0.554758, 0.241806, 0.926062, 0.904201, 0.560665,
	0.0952909, 0.882517, 0.675651, 0.848236, 0.1356, 0.620236, 0.514861, 0.354085,
	0.816905, 0.301431, 0.7881, 0.902218, 0.0715685, 0.739302, 0.901098, 0.190266,
	0.973071, 0.131405, 0.793253, 0.596153, 0.353811, 0.962201, 0.159503, 0.132969,
	0.440379, 0.644882, 0.687727, 0.682186, 0.570944, 0.591927, 0.24285, 0.666235,
	0.474444, 0.918502, 0.51447, 0.610044, 0.538737, 0.0293311, 0.964129, 0.355643,
	0.330762, 0.752229, 0.25786, 0.402331, 0.491532, 0.158958, 0.592597, 0.464603,
	0.290363, 0.38585, 0.0607554, 0.644175, 0.348051, 0.220258, 0.777143, 0.78843,
	0.86514, 0.46487, 0.470616, 0.436084, 0.0567978, 0.713466, 0.102318, 0.531242,
	0.631968, 0.616788, 0.141286, 0.170705, 0.646119, 0.105415, 0.526348, 0.976882,
	0.857645, 0.784208, 0.379212, 0.349176, 0.943166, 0.971809, 0.813779, 0.233529,
	0.357659, 0.874534, 0.877704, 0.70571, 0.0947925, 0.654847, 0.49414, 0.959932,
	0.119718, 0.964755, 0.396016, 0.176516, 0.678221, 0.498334, 0.707758, 0.310189,
	0.115122, 0.849044, 0.480894, 0.761242, 0.95446, 0.00724181, 0.738123, 0.812104,
	0.79145, 0.117336, 0.161281, 0.734616, 0.0891446, 0.97506, 0.968146, 0.446804,
	0.849594, 0.84585, 0.152513, 0.944386, 0.500697, 0.646653, 0.904319, 0.620415,
	0.611408, 0.300335, 0.796931, 0.28963, 0.798669, 0.504688, 0.599819, 0.913791,
	0.353733, 0.0807127, 0.675033, 0.308192, 0.0879545, 0.413157, 0.120296, 0.879404,
	0.530492, 0.281577, 0.614021, 0.619637, 0.256637, 0.582166, 0.0664406, 0.10623,
	0.428016, 0.218954, 0.0506169, 0.928713, 0.865607, 0.954936, 0.549128, 0.477015,
	0.255271, 0.346058, 0.766645, 0.0539397, 0.850747, 0.366463, 0.967731, 0.204479,
	0.447176, 0.642764, 0.512671, 0.535131, 0.0559212, 0.632968, 0.414535, 0.586414
};


// factory function for gradients
static Vec2D* generateGrads() {
	Vec2D* grads = new Vec2D[LATTICE_AREA];
	for (int i = 0; i < LATTICE_AREA; i += 1 ) {
		// grads[i] = Vec2D(cos(2.0f * PI * i / LATTICE_AREA),
						 // sin(2.0f * PI * i / LATTICE_AREA));
		grads[i].x = cos(2.0f * PI * i / LATTICE_AREA);
		grads[i].y = sin(2.0f * PI * i / LATTICE_AREA);
	} // for
	return grads;
}

Vec2D* Perlin::grads = generateGrads();

double Perlin::terrain( int x, int y, int w, int h ) {

	// map coords of x,y point on terrain to the noise grid
	double gridX = (double)x / (double)w;
	double gridY = (double)y / (double)h;
	gridX *= 5.0f;
	gridY *= 5.0f;

	// cout << gridX << " " << gridY << endl;

	// calculate sum of octaves of noise
	double noiseSum = 0;
	unsigned int numLayers = 3;				// # of octaves

	// going to be a bit verbose for the sake of clarity
	float amp  = 1.0f;
	float freq = 1.0f;
	for ( unsigned int i = 0; i < numLayers; i += 1 ) {
		// double res = simpleNoise((gridX * freq) / amp, (gridY * freq) / amp);
		double res = simpleNoise((gridX * freq), (gridY * freq)) / amp;
		// cout << "res: " << res << endl;
		noiseSum += res;

		amp *= 0.5f;
		freq *= 2;
	} // for
		cout << "total: " << noiseSum << endl;

	return noiseSum;
	// return simpleNoise( gridX, gridY );
	// return noise( gridX, gridY );
}

double Perlin::simpleNoise( double x, double y ) {
	int xi = FASTFLOOR(x);
	int yi = FASTFLOOR(y);

	// We can use the & operator as a mod function since we chose the lattice size
	// to be a power of two.
	int xMin = xi & LATTICE_MASK;
	int yMin = yi & LATTICE_MASK;
	int xMax = (xMin + 1) & LATTICE_MASK;
	int yMax = (yMin + 1) & LATTICE_MASK;

	// calculate parameters for lerp
	double u = x - xi;
	double v = y - yi;
	// cout << x << ", " << xi << endl;

	// sanity check
	assert(xMin < LATTICE_AREA);
	assert(yMin < LATTICE_AREA);

	// remap lerp parameters using smoothstep function
	u = fade( u );
	v = fade( v );

	#if 0

	// generate direction bectors from (x,y) to 4 corners
	Vec2D p00 = Vec2D(xMin, yMin);
	Vec2D p10 = Vec2D(xMin, yMax);
	Vec2D p01 = Vec2D(xMin, yMin);
	Vec2D p11 = Vec2D(xMax, yMax);


	// get the values for each of the 4 corners of the cell
	Vec2D c00 = grads[permuteTable[permuteTable[xMin] + yMin]];
	Vec2D c01 = grads[permuteTable[permuteTable[xMin] + yMax]];
	Vec2D c10 = grads[permuteTable[permuteTable[xMax] + yMin]];
	Vec2D c11 = grads[permuteTable[permuteTable[xMax] + yMax]];

	// lerp along x axis first
	double nx0 = lerp( dot(c00, p00), dot(c10, p10), u );
	double nx1 = lerp( dot(c01, p01), dot(c11, p11), u );

	#else 

	// get the values for each of the 4 corners of the cell
	double c00 = rand2D[permuteTable[permuteTable[xMin] + yMin]];
	double c01 = rand2D[permuteTable[permuteTable[xMin] + yMax]];
	double c10 = rand2D[permuteTable[permuteTable[xMax] + yMin]];
	double c11 = rand2D[permuteTable[permuteTable[xMax] + yMax]];

	// lerp along x axis first
	double nx0 = lerp( c00, c10, u );
	double nx1 = lerp( c01, c11, u );

	#endif

	// lerp along remaining (y) axis
	return lerp( nx0, nx1, v );
}

double Perlin::dot( Vec2D a, Vec2D b ) {
	return a.x*b.x + a.y*b.y;
}


double Perlin::fade( double t ) {
	// 6*t^5 - 15*t^4 + 10*t^3
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}