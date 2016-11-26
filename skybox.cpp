#include "skybox.hpp"

#include <iostream>
using namespace std;

Skybox::Skybox() {}

Skybox::~Skybox() {}

// generates skybox to be rendered with GL_TRIANGLES
void Skybox::init( ShaderProgram& m_shader, GLuint texture ) {
	m_texture = texture;

	// vertex data from http://learnopengl.com/code_viewer.php?code=advanced/cubemaps_skybox_data
	GLfloat skyboxVertices[] = {
	    // Positions          
	    -1.0f,  1.0f, -1.0f,
	    -1.0f, -1.0f, -1.0f,
	     1.0f, -1.0f, -1.0f,
	     1.0f, -1.0f, -1.0f,
	     1.0f,  1.0f, -1.0f,
	    -1.0f,  1.0f, -1.0f,

	    -1.0f, -1.0f,  1.0f,
	    -1.0f, -1.0f, -1.0f,
	    -1.0f,  1.0f, -1.0f,
	    -1.0f,  1.0f, -1.0f,
	    -1.0f,  1.0f,  1.0f,
	    -1.0f, -1.0f,  1.0f,

	     1.0f, -1.0f, -1.0f,
	     1.0f, -1.0f,  1.0f,
	     1.0f,  1.0f,  1.0f,
	     1.0f,  1.0f,  1.0f,
	     1.0f,  1.0f, -1.0f,
	     1.0f, -1.0f, -1.0f,

	    -1.0f, -1.0f,  1.0f,
	    -1.0f,  1.0f,  1.0f,
	     1.0f,  1.0f,  1.0f,
	     1.0f,  1.0f,  1.0f,
	     1.0f, -1.0f,  1.0f,
	    -1.0f, -1.0f,  1.0f,

	    -1.0f,  1.0f, -1.0f,
	     1.0f,  1.0f, -1.0f,
	     1.0f,  1.0f,  1.0f,
	     1.0f,  1.0f,  1.0f,
	    -1.0f,  1.0f,  1.0f,
	    -1.0f,  1.0f, -1.0f,

	    -1.0f, -1.0f, -1.0f,
	    -1.0f, -1.0f,  1.0f,
	     1.0f, -1.0f, -1.0f,
	     1.0f, -1.0f, -1.0f,
	    -1.0f, -1.0f,  1.0f,
	     1.0f, -1.0f,  1.0f
	};


	//----------------------------------------------------------------------------------------
	/*
	 * Set up Vertex arrays, buffers, etc.
	 */

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_skybox_vao );
	glBindVertexArray( m_skybox_vao );

	// Create the skybox vertex buffer
	glGenBuffers( 1, &m_skybox_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_skybox_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(skyboxVertices),
		&skyboxVertices, GL_STATIC_DRAW );

	// Create the skybox index buffer
	/*glGenBuffers( 1, &m_skybox_ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_skybox_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, numVerts*sizeof(unsigned int),
		vertIndices, GL_STATIC_DRAW );*/

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr );

	/*// Specify the means of extracting the normals properly.
	GLint normAttrib = m_shader.getAttribLocation( "normal" );
	glEnableVertexAttribArray( normAttrib );
	glVertexAttribPointer( normAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*3) );

	// Specify the means of extracting the textures properly.
	GLint texAttrib = m_shader.getAttribLocation( "texture" );
	glEnableVertexAttribArray( texAttrib );
	glVertexAttribPointer( texAttrib, 2, GL_INT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*6) );*/

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	// glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	CHECK_GL_ERRORS;
}

void Skybox::draw() {

	glDepthMask(GL_FALSE);
	// skyboxShader.Use();
	// ... Set view and projection matrix
	glBindVertexArray(m_skybox_vao);
	// glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}