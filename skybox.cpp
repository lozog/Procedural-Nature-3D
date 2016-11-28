#include "skybox.hpp"

#include <iostream>
using namespace std;

Skybox::Skybox() {}

Skybox::~Skybox() {}

// generates skybox to be rendered with GL_TRIANGLES
void Skybox::init( ShaderProgram& m_shader, GLuint texture ) {
	m_texture = texture;

	float r = 5.0f;
	// vertex data from http://learnopengl.com/code_viewer.php?code=advanced/cubemaps_skybox_data
	GLfloat skyboxVertices[] = {
	    // Positions          
	    -r,  r, -r,
	    -r, -r, -r,
	     r, -r, -r,
	     r, -r, -r,
	     r,  r, -r,
	    -r,  r, -r,

	    -r, -r,  r,
	    -r, -r, -r,
	    -r,  r, -r,
	    -r,  r, -r,
	    -r,  r,  r,
	    -r, -r,  r,

	     r, -r, -r,
	     r, -r,  r,
	     r,  r,  r,
	     r,  r,  r,
	     r,  r, -r,
	     r, -r, -r,

	    -r, -r,  r,
	    -r,  r,  r,
	     r,  r,  r,
	     r,  r,  r,
	     r, -r,  r,
	    -r, -r,  r,

	    -r,  r, -r,
	     r,  r, -r,
	     r,  r,  r,
	     r,  r,  r,
	    -r,  r,  r,
	    -r,  r, -r,

	    -r, -r, -r,
	    -r, -r,  r,
	     r, -r, -r,
	     r, -r, -r,
	    -r, -r,  r,
	     r, -r,  r
	};


	//----------------------------------------------------------------------------------------
	/*
	 * Set up Vertex array & buffers
	 */

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_skybox_vao );
	glBindVertexArray( m_skybox_vao );

	// Create the skybox vertex buffer
	glGenBuffers( 1, &m_skybox_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_skybox_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(skyboxVertices),
		&skyboxVertices, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	// glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	CHECK_GL_ERRORS;
}

void Skybox::draw() {

	glDepthMask(GL_FALSE);
		glBindVertexArray(m_skybox_vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}