#include "vertex.hpp"
#include "billboard.hpp"

#include <iostream>
using namespace std;

void Billboard::init( ShaderProgram& m_shader, GLuint texture ) {
	m_texture = texture;

	// vertices for a square quad
	Vertex* verts = new Vertex[numVerts];

	verts[0].x = -0.5f;
	verts[0].y = -0.5f;
	verts[0].z =  0.0f;
	verts[0].u =  1;
	verts[0].v =  1;

	verts[1].x =  0.5f;
	verts[1].y = -0.5f;
	verts[1].z =  0.0f;
	verts[1].u =  0;
	verts[1].v =  1;

	verts[2].x = -0.5f;
	verts[2].y =  0.5f;
	verts[2].z =  0.0f;
	verts[2].u =  1;
	verts[2].v =  0;

	verts[3].x =  0.5f;
	verts[3].y =  0.5f;
	verts[3].z =  0.0f;
	verts[3].u =  0;
	verts[3].v =  0;
	
	unsigned int* vertIndices = new unsigned int[numVerts];
	for ( unsigned int i = 0; i < numVerts; i += 1 ) {
		vertIndices[i] = i;
	} // for


	//----------------------------------------------------------------------------------------
	/*
	 * Set up Vertex arrays, buffers, etc.
	 */

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_vao );
	glBindVertexArray( m_vao );

	// Create the vertex buffer
	glGenBuffers( 1, &m_vertex_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_vertex_vbo );
	glBufferData( GL_ARRAY_BUFFER, numVerts*sizeof(Vertex),
		verts, GL_STATIC_DRAW );

	// Create the water index buffer
	glGenBuffers( 1, &m_ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, numVerts*sizeof(unsigned int),
		vertIndices, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr );

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
	delete [] vertIndices;
	delete [] verts;

	CHECK_GL_ERRORS;
}

void Billboard::draw() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindVertexArray( m_vao );
	glDrawElements( GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );									// Restore defaults

}