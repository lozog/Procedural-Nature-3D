#include <algorithm>

#include "terrain.hpp"
// #include "grad.hpp"

#include <iostream>
using namespace std;

Terrain::Terrain( size_t x, size_t z )
	: m_length( x ),
	  m_width( z ),
	  bufferIndexCount(0)
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

void Terrain::init( ShaderProgram& m_shader )
{
	double maxVal = 0.0f;
	double heightMap[m_length][m_width];
	for (int x = 0; x < m_length; x += 1) {
		for (int z = 0; z < m_width; z += 1) {
			heightMap[x][z] = Perlin::terrain(x, z, 512, 512, maxVal); // TODO: don't hardcode
		} // for
	} // for

	cout << maxVal << endl;
	for (int x = 0; x < m_length; x += 1) {
		for (int z = 0; z < m_width; z += 1) {
			// cout << " " << heightMap[x][z] << endl;
			heightMap[x][z] /= (0.05f*maxVal);
			// cout << heightMap[x][z] << endl;
		} // for
	} // for

	// generates a flat terrain to be rendered with GL_TRIANGLES_STRIP
	// will need to use an index buffer object and use degenerate triangles to store.
	// see http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/

	// for an x by z grid:
	size_t numVerts 	= (m_length) * (m_width);
	size_t numTriangles = 2 * ((m_length-1)*(m_width-1));

	size_t heightMapVertDataSZ = 3 * numVerts;					// x, y, and z for each vertex
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

	// Now build the index data
	// based on http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
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

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_terrain_vao );
	glBindVertexArray( m_terrain_vao );

	// Create the terrain vertex buffer
	glGenBuffers( 1, &m_terrain_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_terrain_vbo );
	glBufferData( GL_ARRAY_BUFFER, heightMapVertDataSZ*sizeof(float),
		heightMapVertData, GL_STATIC_DRAW );

	// Create the terrain index buffer
	glGenBuffers( 1, &m_terrain_ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_terrain_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, heightMapIndexDataSZ*sizeof(unsigned int),
		heightMapIndexData, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	// const size_t STRIDE = 3; // number of components per generic vertex attribute (3 for x,y,z)
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] heightMapVertData;
	delete [] heightMapIndexData;

	CHECK_GL_ERRORS;
}

void Terrain::draw( const GLuint& col_uni ) {

	glBindVertexArray( m_terrain_vao );
	glDrawElements( GL_TRIANGLE_STRIP, bufferIndexCount, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );									// Restore defaults

}