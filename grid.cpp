#include <algorithm>

#include "grid.hpp"

Grid::Grid( size_t d )
	: m_dim( d )
{
	m_heights = new int[ d * d ];
	m_cols = new int[ d * d ];

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
