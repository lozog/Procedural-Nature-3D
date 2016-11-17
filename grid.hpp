#pragma once
#include "cs488-framework/OpenGLImport.hpp"

class Grid
{
public:
	Grid( size_t dim );
	~Grid();

	void reset();

	size_t getDim() const;

	int getHeight( int x, int y ) const;
	int getColour( int x, int y ) const;

	void setHeight( int x, int y, int h );
	void setColour( int x, int y, int c );

	void incHeight( int x, int y );
	void decHeight( int x, int y );

	GLuint* getBarVAO( int x, int y );
	GLuint* getBarVBO( int x, int y );

	void copyBar(int x1, int y1, int x2, int y2);
	
private:
	size_t m_dim;
	int *m_heights;
	int *m_cols;
	GLuint *m_bar_vaos;
	GLuint *m_bar_vbos;
};
