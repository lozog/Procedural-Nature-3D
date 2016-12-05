#pragma once

#include <glm/glm.hpp>							// GLuint
#include "cs488-framework/GlErrorCheck.hpp"		// CHECK_GL_ERRORS
#include "cs488-framework/ShaderProgram.hpp"	// ShaderProgram

class Terrain
{
public:
	Terrain();
	~Terrain();

	size_t getBufferIndexCount();

	void create( size_t m_length, size_t m_width, unsigned int numOctaves, double redist );
	void init( ShaderProgram& m_shader, GLuint m_ground_texture, GLuint m_cliff_texture );

	void draw();

	double** getHeightMap();

	int mode, numModes;							// 0 == SimplexNoise1234, 1 == my impl
	unsigned int numOctaves;					// fractalization factor
	double redist;								// raise elevation to pow of redist
	
private:
	bool created;								// has storage been allocated?

	size_t m_length, m_width;
	size_t bufferIndexCount;

	double** m_heightmap;						// x,y,z coords of terrain vertices

	// Fields related to terrain geometry.
	GLuint m_terrain_vao; 						// Vertex Array Object
	GLuint m_terrain_vbo; 						// Vertex Buffer Object
	GLuint m_normal_vbo; 						// vertex normal Buffer Object
	GLuint m_colour_vbo; 						// vertex colour Buffer Object
	GLuint m_texture_vbo; 						// vertex texture Buffer Object
	GLuint m_terrain_ibo; 						// Index Buffer Object

	ShaderProgram m_shader;

	// terrain texture
	GLuint m_ground_texture; 					// ground texture
	GLuint m_cliff_texture; 					// steep slope texture

};
