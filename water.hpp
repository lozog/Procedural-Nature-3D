#pragma once

#include <glm/glm.hpp>							// GLuint
#include "cs488-framework/GlErrorCheck.hpp"		// CHECK_GL_ERRORS
#include "cs488-framework/ShaderProgram.hpp"	// ShaderProgram

class Water
{
public:
	Water();
	~Water();

	void create( size_t m_length, size_t m_width );
	void init( ShaderProgram& m_shader, GLuint m_texture, size_t waterHeight );

	void draw();

private:
	bool created;								// has data storage been allocated?

	size_t m_length, m_width, m_height;
	size_t numVerts;

	// Fields related to terrain geometry.
	GLuint m_water_vao; 						// Vertex Array Object
	GLuint m_water_vbo; 						// Vertex Buffer Object
	GLuint m_normal_vbo; 						// vertex normal Buffer Object
	GLuint m_colour_vbo; 						// vertex colour Buffer Object
	GLuint m_texture_vbo; 						// vertex texture Buffer Object
	GLuint m_water_ibo; 						// Index Buffer Object

	// water texture
	GLuint m_texture; 							// water texture

};
