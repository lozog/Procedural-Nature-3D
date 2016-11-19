#include "perlinnoise.hpp"

#include <iostream>
using namespace std;

double Perlin::grads[8][2] = {  {1,0},
								{cos45,sin45},
								{0,1},
								{-cos45,sin45},
								{-1,0},
								{-cos45,-sin45},
								{0,-1},
								{cos45,-sin45} };

double Perlin::terrain( int x, int y, int w, int h ) {

	// map coords of x,y point on terrain to the noise grid

	double gridX = (double)x / (double)w;
	double gridY = (double)y / (double)h;
	gridX *= PERLIN_GRID_SIZE;
	gridY *= PERLIN_GRID_SIZE;

	cout << gridX << " " << gridY << endl;

	return noise( gridX, gridY );
	// return 0.0f;
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

	// get gradients at each corner
	// TODO: gradients must be pseudo-randomly chosen - hash fn?
	double* grad00 = Perlin::grads[1];
	double* grad01 = Perlin::grads[3];
	double* grad10 = Perlin::grads[5];
	double* grad11 = Perlin::grads[7];

	#if 1
	// calculate fade curves for x and y
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

	cout << nxy << endl;
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