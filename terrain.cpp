#include <algorithm>

#include "terrain.hpp"

#include <iostream>
using namespace std;

#define NEWIMPL 1

Terrain::Terrain( size_t x, size_t z, size_t tileSize )
	: m_length( x ),
	  m_width( z ),
	  bufferIndexCount(0),
	  tileSize( tileSize )
{

	reset();
}

void Terrain::reset()
{
	// size_t sz = tileSize*tileSize;
}

Terrain::~Terrain()
{}

size_t Terrain::getTileSize() const
{
	return tileSize;
}

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
	// TODO: generate flat terrain with GL_TRIANGLES_STRIP
	// http://stackoverflow.com/questions/5915753/generate-a-plane-with-triangle-strips
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
			heightMapVertData[ idx+1 ] 	= 0;					// flat terrain -> y=0
			heightMapVertData[ idx+2 ] 	= z;
			#if 1
			cout << heightMapVertData[idx] << ", " << heightMapVertData[idx+1] << ", " << heightMapVertData[idx+2] << endl;
			#endif
			idx += 3;
		} // for
	} // for

	#if 0

	// Now build the index data
	// based on http://www.learnopengles.com/android-lesson-eight-an-introduction-to-index-buffer-objects-ibos/
	size_t numStrips = m_width - 1;
	size_t numDegens = 2 * (numStrips - 1);
	// size_t numDegens = 0;
	size_t vertsPerStrip = 2 * m_length;

	size_t heightMapIndexDataSZ = (vertsPerStrip * numStrips) + numDegens;
	unsigned short* heightMapIndexData = new unsigned short[ heightMapIndexDataSZ ];

	cout << numTriangles << " triangles" << endl;
	cout << heightMapVertDataSZ << " vert sz" << endl;
	cout << heightMapIndexDataSZ << " index sz" << endl;

	int offset = 0;
	 
	for (int z = 0; z < m_width - 1; z += 1) {      
		if (z > 0) {
	        // Degenerate begin: repeat first vertex
	        heightMapIndexData[offset++] = (unsigned short) (z * m_width);
	    } // if
	 
	    for (int x = 0; x < m_length; x += 1) {
	        // One part of the strip
	        heightMapIndexData[offset++] = (unsigned short) ((z * m_width) + x);
	        heightMapIndexData[offset++] = (unsigned short) (((z + 1) * m_width) + x);
	    } // for
	 
	    if (z < m_width - 2) {
	        // Degenerate end: repeat last vertex
	        heightMapIndexData[offset++] = (unsigned short) (((z + 1) * m_width) + (m_length - 1));
	    } // if
	} // for

	bufferIndexCount = heightMapIndexDataSZ;

	for (int i = 0; i < bufferIndexCount; i += 1) {
		cout << heightMapIndexData[i] << " ";
	} // for
	cout << endl;
	
	cout << bufferIndexCount << " bufferIndexCount" << endl;
	#else 

	#if 0
	size_t numVerts = 9;
	size_t heightMapVertDataSZ = 3 * numVerts;
	float* heightMapVertData = new float[ heightMapVertDataSZ ];

	size_t idx = 0;
	heightMapVertData[ idx ] = 0;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 0;
	idx += 3;
	heightMapVertData[ idx ] = 0;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 1;
	idx += 3;
	heightMapVertData[ idx ] = 0;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 2;
	idx += 3;
	heightMapVertData[ idx ] = 1;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 0;
	idx += 3;
	heightMapVertData[ idx ] = 1;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 1;
	idx += 3;
	heightMapVertData[ idx ] = 1;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 2;
	idx += 3;
	heightMapVertData[ idx ] = 2;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 0;
	idx += 3;
	heightMapVertData[ idx ] = 2;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 1;
	idx += 3;
	heightMapVertData[ idx ] = 2;
	heightMapVertData[ idx+1 ] = 0;
	heightMapVertData[ idx+2 ] = 2;
	idx += 3;
	#endif

	size_t heightMapIndexDataSZ = 14;
	unsigned short* heightMapIndexData = new unsigned short[ heightMapIndexDataSZ ];

	heightMapIndexData[0] = 0;
	heightMapIndexData[1] = 3;
	heightMapIndexData[2] = 1;
	heightMapIndexData[3] = 4;
	heightMapIndexData[4] = 2;
	heightMapIndexData[5] = 5;
	heightMapIndexData[6] = 5;
	heightMapIndexData[7] = 3;
	heightMapIndexData[8] = 3;
	heightMapIndexData[9] = 6;
	heightMapIndexData[10] = 4;
	heightMapIndexData[11] = 7;
	heightMapIndexData[12] = 5;
	heightMapIndexData[13] = 8;



	#endif
	#if 0
	for (int i = 0; i < numTriangles; i += 1 ) {
		for (int k = 0; k < 3; k += 1 ) {
			cout << heightMapIndexData[i+k] << " " << heightMapIndexData[i+k+1] << " " << heightMapIndexData[i+k+2] << " - ";
			float x = heightMapVertData[ heightMapIndexData[i+k] ];
			float y = heightMapVertData[ heightMapIndexData[i+1+k] ];
			float z = heightMapVertData[ heightMapIndexData[i+2+k] ];
			if ( i+k % 2 == 1) swap(x, y);
			// cout << x <<", " << y << ", " << z << " -- ";
		}
		cout << endl;
	}
	#endif

	#if 1

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
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, heightMapIndexDataSZ*sizeof(unsigned short),
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

	#else
	// THEIR IMPL:

	size_t sz = 3 * 2 * 2 * (tileSize+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < tileSize+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = tileSize+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = tileSize+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_terrain_vao );
	glBindVertexArray( m_terrain_vao );

	// Create the terrain vertex buffer
	glGenBuffers( 1, &m_terrain_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_terrain_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	const size_t STRIDE = 3; // number of components per generic vertex attribute (3 for x,y,z)
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, STRIDE, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
	#endif
}

void Terrain::draw( const GLuint& col_uni ) {
	#if 0
	glBindBuffer( GL_ARRAY_BUFFER, m_terrain_vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_terrain_ibo );

	glDrawElements( GL_TRIANGLE_STRIP, bufferIndexCount, GL_UNSIGNED_SHORT, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	
	#else

	// THEIR IMPL
	glBindVertexArray( m_terrain_vao );
	// glBindVertexBuffer( m_terrain_ibo );
	glUniform3f( col_uni, 1, 1, 1 );
	// glDrawArrays( GL_LINES, 0, (3+tileSize)*4 );
	// glDrawArrays( GL_TRIANGLE_STRIP, 0, 6 ); // last param is # of ACTUAL verts to draw
	glDrawElements( GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, 0 );
	// glBindVertexBuffer( 0 );											// Restore defaults
	glBindVertexArray( 0 );											// Restore defaults
	

	#endif

}