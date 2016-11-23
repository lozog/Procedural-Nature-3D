#pragma once

#include <glm/glm.hpp>							// GLuint
#include "cs488-framework/GlErrorCheck.hpp"		// CHECK_GL_ERRORS
#include "cs488-framework/ShaderProgram.hpp"	// ShaderProgram

class Terrain
{
public:
	Terrain( size_t m_length, size_t m_width, unsigned int numOctaves );
	~Terrain();

	void reset();

	GLuint getVAO();
	GLuint getVBO();
	size_t getBufferIndexCount();

	void init( ShaderProgram& m_shader );

	void draw( const GLuint& col_uni );

	int mode, numModes;
	unsigned int numOctaves;
	
private:
	size_t m_length, m_width;
	size_t bufferIndexCount;

	// Fields related to terrain geometry.
	GLuint m_terrain_vao; 						// Vertex Array Object
	GLuint m_terrain_vbo; 						// Vertex Buffer Object
	GLuint m_normal_vbo; 						// vertex normal Buffer Object
	GLuint m_terrain_ibo; 						// Index Buffer Object

};
