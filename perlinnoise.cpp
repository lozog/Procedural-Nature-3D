#include "perlinnoise.hpp"

double Perlin::noise( double x, double y ) {

	// P = (x, y)

	// find unit grid cell containing P
	int x0 = FASTFLOOR(x);
	int y0 = FASTFLOOR(y);
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	// get gradients at each corner
	// TODO: gradients
	int grad00 = grad[x0][y0];
	int grad01 = grad[x0][y1];
	int grad10 = grad[x1][y0];
	int grad11 = grad[x1][y1];

	// calculate fade curves for x and y
	double u = x - x0;
	double v = y - y0;

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

	return nxy;
}

double Perlin::dot( int g[], double x, double y ) {
	return g[0]*x + g[1]*y;
}


double Perlin::fade( double t ) {
	return t*t*t*(t*(t*6 - 15) + 10);
}