#include <algorithm>

#include "terrain.hpp"

Terrain::Terrain( size_t d )
	: tileSize( d )
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

void Terrain::init( ShaderProgram& m_shader )
{
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

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_terrain_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_terrain_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void Terrain::draw( const GLuint& col_uni ) {
	glBindVertexArray( m_terrain_vao );
	glUniform3f( col_uni, 1, 1, 1 );
	glDrawArrays( GL_LINES, 0, (3+tileSize)*4 );
}