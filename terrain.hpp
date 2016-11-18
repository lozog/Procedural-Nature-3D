#pragma once

#include <glm/glm.hpp>							// GLuint
#include "cs488-framework/GlErrorCheck.hpp"		// CHECK_GL_ERRORS
#include "cs488-framework/ShaderProgram.hpp"	// ShaderProgram

class Terrain
{
public:
	Terrain( size_t tileSize );
	~Terrain();

	void reset();

	size_t getTileSize() const;

	GLuint getVAO();
	GLuint getVBO();

	void init( ShaderProgram& m_shader );

	void draw( const GLuint& col_uni );
	
private:
	size_t tileSize;
	const size_t numVertices = 128;

	// Fields related to terrain geometry.
	GLuint m_terrain_vao; 						// Vertex Array Object
	GLuint m_terrain_vbo; 						// Vertex Buffer Object
};
