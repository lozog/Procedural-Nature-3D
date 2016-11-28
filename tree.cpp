#include "tree.hpp"

#include <cmath>					// sin, cos

#include <iostream>
using namespace std;

Tree::Tree() {}

Tree::~Tree() {}

void Tree::init( glm::mat3 HLU, glm::vec3 origin, string expr,
				 float r1, /*float r2,*/ float a0, /*float a1, float a2,*/ float wr ) {
	Tree::origin = origin;
	Tree::expr = expr;

	rootSegment = new Branch()
}

Branch* Tree::getRoot() {
	return rootSegment;
}
