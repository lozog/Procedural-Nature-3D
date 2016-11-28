#include "branchgeometry.hpp"

#include <cmath>					// sin, cos

#include <iostream>
using namespace std;

BranchGeometry::BranchGeometry( float bottomRadius,
				float topRadius,
				float length,
				glm::vec3 origin,
				unsigned int levelOfDetail,
				glm::vec3 heading,
				glm::vec3 left )
	: bottomRadius( bottomRadius ),
	  topRadius( topRadius ),
	  length( length ),
	  origin( origin ),
	  detail( levelOfDetail )
{
	BranchGeometry::heading = glm::normalize(heading);
	BranchGeometry::left = glm::normalize(left);
	up = glm::normalize(glm::cross(heading, left));
}

BranchGeometry::~BranchGeometry() {
	// delete [] verts;
	// delete [] indexBuffer;
}

void BranchGeometry::init( ShaderProgram& m_shader, GLuint m_texture ) {
	// allocate vertex array for this branch
	unsigned int vertsPerRow = 4 * pow(2, detail-1);
	unsigned int numVerts = 2 * vertsPerRow;
	verts = new Vertex[ numVerts ];

	float angle = glm::radians(90.0f / (float)detail);
	size_t idx = 0;
	// bottom row of verts
	for ( unsigned int x = 0; x < vertsPerRow; x += 1 ) {
		#if 0
		verts[idx].x = origin.x + (bottomRadius * cos(x*angle));
		verts[idx].y = origin.y;
		verts[idx].z = origin.z + (bottomRadius * sin(x*angle));
		#else
		verts[idx].x = origin.x + (heading.x * bottomRadius * cos(x*angle))
								+ (left.x    * bottomRadius * sin(x*angle));
		verts[idx].y = origin.y + (heading.y * bottomRadius * cos(x*angle))
								+ (left.y    * bottomRadius * sin(x*angle));
		verts[idx].z = origin.z + (heading.z * bottomRadius * cos(x*angle))
								+ (left.z    * bottomRadius * sin(x*angle));
		#endif
		// probably don't need to do this here, the fragment shader normalizes the normals
		glm::vec3 normal = glm::normalize(glm::vec3(verts[idx].x, verts[idx].y, verts[idx].z));
		verts[idx].Nx = normal.x;
		verts[idx].Ny = normal.y;
		verts[idx].Nz = normal.z;
		verts[idx].u = x;
		verts[idx].v = 0;
		#if 0
		cout << verts[idx].x << ", ";
		cout << verts[idx].y << ", ";
		cout << verts[idx].z << endl;
		cout << origin.x + (bottomRadius * cos(x*angle)) << ", ";
		cout << origin.y << ", ";
		cout << origin.z + (bottomRadius * sin(x*angle)) << endl;
		cout << endl;
		#endif
		idx += 1;
	} // for

	// top row of verts
	for ( unsigned int x = 0; x < vertsPerRow; x += 1 ) {
		#if 0
		verts[idx].x = origin.x + (topRadius * cos(x*angle));
		verts[idx].y = origin.y + length;
		verts[idx].z = origin.z + (topRadius * sin(x*angle));
		#else
		glm::vec3 topOrigin = origin + length*up;
		verts[idx].x = topOrigin.x + (heading.x * topRadius * cos(x*angle))
								   + (left.x    * topRadius * sin(x*angle));
		verts[idx].y = topOrigin.y + (heading.y * topRadius * cos(x*angle))
								   + (left.y    * topRadius * sin(x*angle));
		verts[idx].z = topOrigin.z + (heading.z * topRadius * cos(x*angle))
								   + (left.z    * topRadius * sin(x*angle));
		#endif
		glm::vec3 normal = glm::normalize(glm::vec3(verts[idx].x, verts[idx].y, verts[idx].z));
		verts[idx].Nx = normal.x;
		verts[idx].Ny = normal.y;
		verts[idx].Nz = normal.z;
		verts[idx].u = x;
		verts[idx].v = 5;
		#if 0
		cout << verts[idx].Nx << ", ";
		cout << verts[idx].Ny << ", ";
		cout << verts[idx].Nz << endl;
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

	BranchGeometry::numVerts = numVerts;

	//----------------------------------------------------------------------------------------
	/*
	 * Set up Vertex arrays, buffers, etc.
	 */

	BranchGeometry::m_texture = m_texture;

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

void BranchGeometry::draw() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindVertexArray( m_vao );
	glDrawElements( GL_TRIANGLE_STRIP, numVerts+2, GL_UNSIGNED_INT, 0 );
	glBindVertexArray( 0 );									// Restore defaults

}