#include "branch.hpp"

#include <cmath>					// sin, cos

#include <iostream>
using namespace std;

Branch::Branch( float bottomRadius,
				float topRadius,
				float length,
				glm::vec3 origin,
				unsigned int levelOfDetail )
	: bottomRadius( bottomRadius ),
	  topRadius( topRadius ),
	  length( length ),
	  origin( origin ),
	  detail( levelOfDetail )
{
	// allocate vertex array for this branch
	unsigned int vertsPerRow = 4 * pow(2, detail-1);
	unsigned int numVerts = 2 * vertsPerRow;
	verts = new Vertex[ numVerts ];

	float angle = glm::radians(90.0f / (float)detail);

	size_t idx = 0;
	// bottom row of verts
	for ( unsigned int x = 0; x < vertsPerRow; x += 1 ) {
		// do stuff
		verts[idx].x = origin.x + (bottomRadius * cos(angle*(x+1)));
		verts[idx].y = origin.y;
		verts[idx].z = origin.z + (bottomRadius * sin(angle*(x+1)));
		verts[idx].Nx = verts[idx].x;
		verts[idx].Ny = origin.y;
		verts[idx].Nz = verts[idx].z;
		verts[idx].u = x;
		verts[idx].v = 0;
		#if 0
		cout << verts[idx].x << ", ";
		cout << verts[idx].y << ", ";
		cout << verts[idx].z << endl;
		#endif
		idx += 1;
	} // for

	// top row of verts
	for ( unsigned int x = 0; x < vertsPerRow; x += 1 ) {
		// do stuff
		verts[idx].x = origin.x + (topRadius * cos(angle*(x+1)));
		verts[idx].y = origin.y + length;
		verts[idx].z = origin.z + (topRadius * sin(angle*(x+1)));
		verts[idx].Nx = verts[idx].x;
		verts[idx].Ny = origin.y;
		verts[idx].Nz = verts[idx].z;
		verts[idx].u = x;
		verts[idx].v = 10;
		#if 0
		cout << verts[idx].x << ", ";
		cout << verts[idx].y << ", ";
		cout << verts[idx].z << endl;
		#endif
		idx += 1;
	} // for

	indexBuffer = new unsigned int[numVerts + 2];

	idx = 0;
	for( unsigned int x = 0; x < vertsPerRow; x+= 1 ) {
		indexBuffer[idx] = x;
		indexBuffer[idx + 1] = x + vertsPerRow;
	#if 0
		cout << indexBuffer[idx] << " ";
		cout << indexBuffer[idx+1] << " ";
	#endif
		idx += 2;
	} // for

	// last two triangles wrap around to beginning to make closed shape
	indexBuffer[idx] = 0;
	indexBuffer[idx+1] = vertsPerRow;
	#if 0
	cout << indexBuffer[idx] << " ";
	cout << indexBuffer[idx+1] << endl;
	#endif

	Branch::numVerts = numVerts;

}

Branch::~Branch() {
	// delete [] verts;
	// delete [] indexBuffer;
}

void Branch::init( ShaderProgram& m_shader, GLuint m_texture ) {
	//----------------------------------------------------------------------------------------
	/*
	 * Set up Vertex arrays, buffers, etc.
	 */

	Branch::m_texture = m_texture;

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_vao );
	glBindVertexArray( m_vao );

	// Create the terrain vertex buffer
	glGenBuffers( 1, &m_vertex_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_vertex_vbo );
	glBufferData( GL_ARRAY_BUFFER, (numVerts)*sizeof(Vertex),
		verts, GL_STATIC_DRAW );

	// Create the terrain index buffer
	glGenBuffers( 1, &m_ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, (numVerts+2)*sizeof(unsigned int),
		indexBuffer, GL_STATIC_DRAW );

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
	delete [] verts;
	delete [] indexBuffer;

	CHECK_GL_ERRORS;
}

void Branch::draw() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindVertexArray( m_vao );
	glDrawElements( GL_TRIANGLE_STRIP, numVerts+2, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );									// Restore defaults

}