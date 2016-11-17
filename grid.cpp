#include <algorithm>
// #include <iostream>

#include "cs488-framework/OpenGLImport.hpp"

#include "grid.hpp"

Grid::Grid( size_t d )
	: m_dim( d )
{
	m_heights = new int[ d * d ];
	m_cols = new int[ d * d ];
	m_bar_vaos = new GLuint[ d * d ];
	m_bar_vbos = new GLuint[ d * d ];

	reset();
}

void Grid::reset()
{
	size_t sz = m_dim*m_dim;
	std::fill( m_heights, m_heights + sz, 0 );
	std::fill( m_cols, m_cols + sz, 0 );
}

Grid::~Grid()
{
	delete [] m_heights;
	delete [] m_cols;
}

size_t Grid::getDim() const
{
	return m_dim;
}

int Grid::getHeight( int x, int y ) const
{
	return m_heights[ y * m_dim + x ];
}

int Grid::getColour( int x, int y ) const
{
	return m_cols[ y * m_dim + x ];
}

void Grid::setHeight( int x, int y, int h )
{
	m_heights[ y * m_dim + x ] = h;
}

void Grid::setColour( int x, int y, int c )
{
	m_cols[ y * m_dim + x ] = c;
}

void Grid::incHeight( int x, int y )
{	
	int height = m_heights[ y * m_dim + x ];
	if (height < m_dim-1) {
		setHeight(x, y, height+1);
	}
}

void Grid::decHeight( int x, int y )
{
	int height = m_heights[ y * m_dim + x ];
	if (height > 0) {
		setHeight(x, y, height-1);
	}
}

GLuint* Grid::getBarVAO( int x, int y ) {
	return &m_bar_vaos[ y * m_dim + x ];
}

GLuint* Grid::getBarVBO( int x, int y ) {
	return &m_bar_vbos[ y * m_dim + x ];
}

void Grid::copyBar(int x1, int y1, int x2, int y2) {
	// std::cout << "changing height " << m_heights[ y2 * m_dim + x2 ] << " to " << m_heights[ y1 * m_dim + x1 ] << std::endl;
	this->setHeight(x2, y2, m_heights[ y1 * m_dim + x1 ]);
	this->setColour(x2, y2, m_cols[ y1 * m_dim + x1 ]);
}
