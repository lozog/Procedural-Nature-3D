#include "branchnode.hpp"

#include <cmath>					// sin, cos

#include <iostream>
using namespace std;

BranchNode::BranchNode() {}

BranchNode::~BranchNode() {}

void BranchNode::init( glm::mat3 HLU, glm::vec3 origin, std::string expr,
				 float r1, /*float r2,*/ float a0, /*float a1, float a2,*/ float wr ) {
	BranchNode::origin = origin;
	BranchNode::expr = expr;

	// rootSegment = new Branch()
}

BranchGeometry* BranchNode::getRoot() {
	return rootSegment;
}
