#include <algorithm>

#include "terrain.hpp"
#include "simplexnoise.hpp"
#include "perlinnoise.hpp"

#include <iostream>
using namespace std;

struct Vertex {
	float x, y, z;			// coordinates of this vertex
	float Nx, Ny, Nz;		// normal at this vertex
	int u, v;				// texture coordinates
};

Terrain::Terrain( size_t length, size_t width, unsigned int numOctaves, double redist )
	: m_length( length ),
	  m_width( width ),
	  bufferIndexCount(0),
	  numOctaves(numOctaves),
	  mode(1),
	  numModes(2),
	  redist(redist)
{
	reset();
}

void Terrain::reset()
{
}

Terrain::~Terrain()
{}

GLuint Terrain::getVAO() {
	return m_terrain_vao;
}

GLuint Terrain::getVBO() {
	return m_terrain_vbo;
}

size_t Terrain::getBufferIndexCount() {
	return bufferIndexCount;
}

// generates a flat terrain to be rendered with GL_TRIANGLES_STRIP
void Terrain::init( ShaderProgram& m_shader, GLuint ground_texture ) {
	m_ground_texture = ground_texture;
	//----------------------------------------------------------------------------------------
	/*
	 * use noise to generate terrain
	 */
	double heightMap[m_length][m_width];

	double maxVal = 0.0f; // for now, unused

	for (int x = 0; x < m_length; x += 1) {
		for (int z = 0; z < m_width; z += 1) {

			// scaling factor makes terrain more interesting
			double scale = 0.5f;

			double gridX = ((double)x / ((double)m_length * scale)) - 0.5f;
			double gridY = ((double)z / ((double)m_width * scale)) - 0.5f;

			double freq = 1.0f;
			double amp = 1.0f;
			double fractalSum = 0;

			for ( unsigned int i = 0; i < numOctaves; i += 1 ) {
				if (mode == 0 ) {
					fractalSum += 3.0f * (1.0f + (SimplexNoise1234::noise(freq*gridX, freq*gridY) * amp));
				} else {
					fractalSum += 3.0f * (1.1f + (Perlin::noise(freq*gridX, freq*gridY) * amp));
				}
				amp *= 0.5f;
				freq *= 2.0f;
			} // for
			// TODO: prevent these from ever being negative!
			if (fractalSum < 0.0f) cout << fractalSum << endl;
			if ( fractalSum > maxVal ) maxVal = fractalSum;
			heightMap[x][z] = pow(fractalSum, redist);

		} // for
	} // for

	#if 0
	// resources say I should have this, but I find results are better when omitted
	cout << maxVal << endl;
	for (int x = 0; x < m_length; x += 1) {
		for (int z = 0; z < m_width; z += 1) {
			// cout << " " << heightMap[x][z] << endl;
			heightMap[x][z] /= maxVal;
			// cout << heightMap[x][z] << endl;
		} // for
	} // for
	#endif

	//----------------------------------------------------------------------------------------
	/*
	 * Generate vertices of the terrain
	 */

	// for an x by z grid:
	size_t numVerts 	= (m_length) * (m_width);
	size_t numTriangles = 2 * ((m_length-1)*(m_width-1));

	size_t heightMapVertDataSZ = 3 * numVerts;					// x, y, and z for each vertex
	// float heightMapVertData[ heightMapVertDataSZ ];
	float* heightMapVertData = new float[ heightMapVertDataSZ ];

	size_t idx = 0;
	for (int x = 0; x < m_length; x += 1) {
		for (int z = 0; z < m_width; z += 1) {
			// size_t idx = 3*(z*(m_length-1)) + x;
			heightMapVertData[ idx ] 	= x;
			// heightMapVertData[ idx+1 ] 	= SimplexNoise1234::noise(x, z);
			heightMapVertData[ idx+1 ] 	= heightMap[x][z];
			// heightMapVertData[ idx+1 ] 	= 0.01f*Perlin::simpleNoise(x, z);
			// heightMapVertData[ idx+1 ] 	= (((x+z )% 2 == 0) ? 3 : 0);
			heightMapVertData[ idx+2 ] 	= z;
			#if 1
			// cout << heightMapVertData[idx] << ", " << heightMapVertData[idx+1] << ", " << heightMapVertData[idx+2] << endl;
			#endif
			idx += 3;
		} // for
	} // for

	//----------------------------------------------------------------------------------------
	/*
	 * Calculate normals at each vertex of terrain
	 */

	// one normal per vertex -> same size as vertex
	// float normalMap[ heightMapVertDataSZ ];
	float* normalMap = new float[ heightMapVertDataSZ ];

	idx = 0;
	for (int x = 0; x < m_length; x += 1) {
		for (int z = 0; z < m_width; z += 1) {
			float sx = heightMap[x < m_length-1 ? x+1 : x][z] 
					  - heightMap[x > 0 ? x-1 : x][z];
			if ( x == 0 || x == m_length-1)
				sx *= 2.0f;

			float sz = heightMap[x][z < m_width-1 ? z+1 : z]
					  - heightMap[x][z > 0 ? z-1 : z];
			if ( z == 0 || z == m_width-1)
				sz *= 2.0f;

			/*if ( x == 18 && z == 39 ) {
				cout << (z < m_width-1 ? z+1 : z) << endl;
				cout << (z > 0 ? z-1 : z) << endl;
				cout << heightMap[x][z < m_width-1 ? z+1 : z] << " - " << heightMap[x][z > 0 ? z-1 : z] << endl;
				cout << sz << endl;
			}*/
			// cout << x << ", " << z << ": " << sx << ", " << sz << endl;
			glm::vec3 norm = glm::normalize(glm::vec3(-sx, 2.0f, sz));
			normalMap[idx] = norm.x;
			normalMap[idx+1] = norm.y;
			normalMap[idx+2] = norm.z;
			idx += 3;
		} // for
	} // for

	//----------------------------------------------------------------------------------------
	/*
	 * Calculate terrain (u, v) coordinates for each vertex
	 */

	int terrainMap[m_length * m_width * 2];

	idx = 0;
	for (int x = 0; x < m_length; x += 1) {
		for (int z = 0; z < m_width; z += 1) {
			terrainMap[idx] = x % 1024;
			terrainMap[idx+1] = z % 1024;
			// if ( idx < 8 ) cout << terrainMap[idx] << ", " << terrainMap[idx+1] << endl;
			idx += 2;
		} // for
	} // for

	//----------------------------------------------------------------------------------------
	/*
	 * Collect vertex info into Vertex structs
	 */

	idx = 0;
	size_t idx2 = 0;
	size_t vertexDataSZ = sizeof(Vertex) * numVerts;
	Vertex* verts = new Vertex[ vertexDataSZ ];
	for (int i = 0; i < numVerts; i += 1) {
		verts[i].x = heightMapVertData[idx    ];
		verts[i].y = heightMapVertData[idx + 1];
		verts[i].z = heightMapVertData[idx + 2];
		verts[i].Nx = normalMap[idx];
		verts[i].Ny = normalMap[idx + 1];
		verts[i].Nz = normalMap[idx + 2];
		verts[i].u = terrainMap[idx2];
		verts[i].v = terrainMap[idx2 + 1];
		// if ( i < 4 ) cout << verts[i].u << ", " << verts[i].v << endl;
		idx += 3;
		idx2 += 2;
	} // for

	//----------------------------------------------------------------------------------------
	/*
	 * Now build the vertex index data using an index buffer object and degenerate triangles
	 * based on http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
	 */
	size_t numStrips = m_width - 1;
	size_t numDegens = 2 * (numStrips - 1);
	size_t vertsPerStrip = 2 * m_length;

	size_t heightMapIndexDataSZ = (vertsPerStrip * numStrips) + numDegens;
	unsigned int* heightMapIndexData = new unsigned int[ heightMapIndexDataSZ ];

	#if 0
	cout << numTriangles << " triangles" << endl;
	cout << heightMapVertDataSZ << " vert sz" << endl;
	cout << heightMapIndexDataSZ << " index sz" << endl;
	#endif

	int offset = 0;
	 
	for (int z = 0; z < m_width - 1; z += 1) {      
		if (z > 0) {
	        // Degenerate begin: repeat first vertex
	        heightMapIndexData[offset++] = (unsigned int) (z * m_width);
	    } // if
	 
	    for (int x = 0; x < m_length; x += 1) {
	        // One part of the strip
	        heightMapIndexData[offset++] = (unsigned int) ((z * m_width) + x);
	        heightMapIndexData[offset++] = (unsigned int) (((z + 1) * m_width) + x);
	    } // for
	 
	    if (z < m_width - 2) {
	        // Degenerate end: repeat last vertex
	        heightMapIndexData[offset++] = (unsigned int) (((z + 1) * m_width) + (m_length - 1));
	    } // if
	} // for

	bufferIndexCount = heightMapIndexDataSZ;

	#if 0
	for (int i = 0; i < heightMapVertDataSZ; i += 1) {
		cout << heightMapVertData[i] << " ";
	} // for
	cout << endl;
	for (int i = 0; i < bufferIndexCount; i += 1) {
		cout << heightMapIndexData[i] << " ";
	} // for
	cout << endl;
	#endif

	//----------------------------------------------------------------------------------------
	/*
	 * Set up Vertex arrays, buffers, etc.
	 */

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_terrain_vao );
	glBindVertexArray( m_terrain_vao );

	// Create the terrain vertex buffer
	glGenBuffers( 1, &m_terrain_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_terrain_vbo );
	glBufferData( GL_ARRAY_BUFFER, vertexDataSZ*sizeof(Vertex),
		verts, GL_STATIC_DRAW );

	// Create the terrain index buffer
	glGenBuffers( 1, &m_terrain_ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_terrain_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, heightMapIndexDataSZ*sizeof(unsigned int),
		heightMapIndexData, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr );

	// Specify the means of extracting the normals properly.
	GLint normAttrib = m_shader.getAttribLocation( "normal" );
	glEnableVertexAttribArray( normAttrib );
	glVertexAttribPointer( normAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*3) );

	// Specify the means of extracting the textures properly.
	GLint texAttrib = m_shader.getAttribLocation( "texture" );
	glEnableVertexAttribArray( texAttrib );
	glVertexAttribPointer( texAttrib, 2, GL_INT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*6) );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffers now, there's no need for us to keep any copies
	delete [] normalMap;
	delete [] verts;
	delete [] heightMapIndexData;
	delete [] heightMapVertData;

	CHECK_GL_ERRORS;
}

void Terrain::draw() {

	glBindTexture(GL_TEXTURE_2D, m_ground_texture);
	glBindVertexArray( m_terrain_vao );
	glDrawElements( GL_TRIANGLE_STRIP, bufferIndexCount, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );									// Restore defaults

}