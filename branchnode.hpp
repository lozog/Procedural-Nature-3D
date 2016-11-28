#pragma once

#include <glm/glm.hpp>							// glm::vec3
#include "cs488-framework/GlErrorCheck.hpp"		// CHECK_GL_ERRORS
#include "cs488-framework/ShaderProgram.hpp"	// ShaderProgram

#include "vertex.hpp"

class BranchNode
{
public:
	BranchNode( float bottomRadius,				// girth of branch at first (connecting) end
			float topRadius,				// girth of branch at second (end) end
			float length,					// length of branch
			glm::vec3 origin,				// origin point of branch
			unsigned int levelOfDetail,
			glm::vec3 heading,
			glm::vec3 left );
	~BranchNode();

	void init( ShaderProgram& m_shader, GLuint m_texture );
	void draw();

private:
	float bottomRadius, topRadius, length;
	unsigned int detail;
	glm::vec3 origin;

	glm::vec3 heading, left, up;

	GLuint m_texture;

	// Fields related to geometry.
	GLuint m_vao; 							// Vertex Array Object
	GLuint m_vertex_vbo; 					// Vertex Buffer Object
	GLuint m_normal_vbo; 					// vertex normal Buffer Object
	GLuint m_texture_vbo; 					// vertex texture Buffer Object
	GLuint m_ibo; 							// Index Buffer Object

	unsigned int numVerts;
	Vertex* verts;
	unsigned int* indexBuffer;
};
