#pragma once

#include <glm/glm.hpp>							// GLuint
#include "cs488-framework/GlErrorCheck.hpp"		// CHECK_GL_ERRORS
#include "cs488-framework/ShaderProgram.hpp"	// ShaderProgram

class Skybox
{
public:
	Skybox();
	~Skybox();

	void init( ShaderProgram& m_shader, GLuint m_texture );

	void draw();

private:

	// Fields related to terrain geometry.
	GLuint m_skybox_vao; 						// Vertex Array Object
	GLuint m_skybox_vbo; 						// Vertex Buffer Object
	// GLuint m_normal_vbo; 						// vertex normal Buffer Object
	// GLuint m_texture_vbo; 						// vertex texture Buffer Object
	// GLuint m_skybox_ibo; 						// Index Buffer Object

	// skybox texture
	GLuint m_texture; 							// skybox texture

};
