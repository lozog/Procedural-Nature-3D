#include "vertex.hpp"
#include "water.hpp"

#include <iostream>
using namespace std;

Water::Water( size_t length, size_t width )
	: m_length( length ),
	  m_width( width ),
	  numVerts(4)
{}

Water::~Water()
{}

// generates water to be rendered with GL_TRIANGLES
void Water::init( ShaderProgram& m_shader, GLuint texture, size_t waterHeight ) {
	m_height = waterHeight;
	m_texture = texture;

	Vertex* verts = new Vertex[ numVerts ];
	unsigned int* vertIndices = new unsigned int[numVerts];
	for (unsigned int i = 0; i < numVerts; i += 1 ) {
		verts[i].y = m_height;

		// all vertices face up
		verts[i].Nx = 1;
		verts[i].Ny = 0;
		verts[i].Nz = 0;
		verts[i].r  = 0.5f;
		verts[i].g  = 0.5f;
		verts[i].b  = 0.5f;
		verts[i].a  = 1.0f;

		// vertex index buffer
		vertIndices[i] = i;
	} // for
	verts[0].x = 0;
	verts[0].z = 0;
	verts[0].u = 0;
	verts[0].v = 0;

	verts[1].x = m_length;
	verts[1].z = 0;
	verts[1].u = m_length;
	verts[1].v = 0;

	verts[2].x = 0;
	verts[2].z = m_width;
	verts[2].u = 0;
	verts[2].v = m_width;

	verts[3].x = m_length;
	verts[3].z = m_width;
	verts[3].u = m_length;
	verts[3].v = m_width;


	//----------------------------------------------------------------------------------------
	/*
	 * Set up Vertex arrays, buffers, etc.
	 */

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_water_vao );
	glBindVertexArray( m_water_vao );

	// Create the water vertex buffer
	glGenBuffers( 1, &m_water_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_water_vbo );
	glBufferData( GL_ARRAY_BUFFER, numVerts*sizeof(Vertex),
		verts, GL_STATIC_DRAW );

	// Create the water index buffer
	glGenBuffers( 1, &m_water_ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_water_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, numVerts*sizeof(unsigned int),
		vertIndices, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr );

	// Specify the means of extracting the normals properly.
	GLint normAttrib = m_shader.getAttribLocation( "normal" );
	glEnableVertexAttribArray( normAttrib );
	glVertexAttribPointer( normAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*3) );

	// Specify the means of extracting the colours properly.
	GLint colAttrib = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray( colAttrib );
	glVertexAttribPointer( colAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*6) );

	// Specify the means of extracting the textures properly.
	GLint texAttrib = m_shader.getAttribLocation( "texture" );
	glEnableVertexAttribArray( texAttrib );
	glVertexAttribPointer( texAttrib, 2, GL_INT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*10) );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffers now, there's no need for us to keep any copies
	delete [] vertIndices;
	delete [] verts;

	CHECK_GL_ERRORS;
}

void Water::draw() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindVertexArray( m_water_vao );
	glDrawElements( GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );									// Restore defaults

}