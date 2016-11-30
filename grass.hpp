#pragma once

#include <glm/glm.hpp>							// GLuint
#include "cs488-framework/GlErrorCheck.hpp"		// CHECK_GL_ERRORS
#include "cs488-framework/ShaderProgram.hpp"	// ShaderProgram

class Grass
{
public:
	Grass() : numVerts(4) {}
	~Grass() {}

	void init( ShaderProgram& m_shader, GLuint m_texture, glm::vec3 position );

	void draw();

private:
	size_t numVerts;

	// Fields related to geometry.
	GLuint m_vao; 						// Vertex Array Object
	GLuint m_vertex_vbo; 				// Vertex Buffer Object
	// GLuint m_normal_vbo; 				// vertex normal Buffer Object
	GLuint m_texture_vbo; 				// vertex texture Buffer Object
	// GLuint m_ibo; 						// Index Buffer Object

	// texture
	GLuint m_texture; 					// water texture

};
