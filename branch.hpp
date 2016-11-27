#pragma once

#include <glm/glm.hpp>							// glm::vec3
#include "cs488-framework/GlErrorCheck.hpp"		// CHECK_GL_ERRORS
#include "cs488-framework/ShaderProgram.hpp"	// ShaderProgram

#include "vertex.hpp"

class Branch
{
public:
	Branch( float bottomRadius,
			float topRadius,
			float length,
			glm::vec3 origin,
			unsigned int levelOfDetail );
	~Branch();

	void init( ShaderProgram& m_shader, GLuint m_texture );
	void draw();

private:
	float bottomRadius, topRadius, length;
	unsigned int detail;
	glm::vec3 origin;

	GLuint m_texture;

	// Fields related to geometry.
	GLuint m_vao; 								// Vertex Array Object
	GLuint m_vertex_vbo; 						// Vertex Buffer Object
	GLuint m_normal_vbo; 						// vertex normal Buffer Object
	GLuint m_texture_vbo; 						// vertex texture Buffer Object
	GLuint m_ibo; 								// Index Buffer Object

	unsigned int numVerts;
	Vertex* verts;
	unsigned int* indexBuffer;
};
