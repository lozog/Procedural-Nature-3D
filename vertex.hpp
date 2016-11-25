#pragma once

struct Vertex {
	float x, y, z;			// coordinates of this vertex
	float Nx, Ny, Nz;		// normal at this vertex
	int u, v;				// texture coordinates
};