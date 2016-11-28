#include "ltree.hpp"

#include <cmath>					// sin, cos
#include <glm/gtx/io.hpp>

#include <iostream>
using namespace std;

LTree::LTree() {}

LTree::~LTree() {}

void LTree::init( glm::vec3 heading, glm::vec3 down, glm::vec3 origin, std::string expr,
				 float r1, /*float r2,*/ float a0, /*float a1, float a2,*/ float wr,
				 ShaderProgram& m_shader, GLuint m_texture ) {
	LTree::origin = origin;
	LTree::expr = expr;

	glm::vec3 left 	= glm::normalize(glm::cross(-1.0f * down, heading));
	glm::vec3 up 	= glm::cross(heading, left);

	unsigned int levelOfDetail = 2;
	float radiusOfStump = 3.0f;
	float initialBranchLength = 5.0f;

	float radius = radiusOfStump;
	float nextRadius = r1 * radius;
	float branchLength = initialBranchLength;

	for( char& symbol : expr ) {
		
		switch( symbol ) {
			case 'F': {
				BranchNode* child = 
							new BranchNode( radius,
											nextRadius,
											branchLength,
											origin,
											levelOfDetail,
											heading,
											left);
				child->init( m_shader, m_texture );
				branches.push_back(child);

				cout << radius << " " << nextRadius << endl;
				cout << origin << endl;

				origin += (branchLength * up);			// for now, grows up

				radius = nextRadius;
				nextRadius *= r1;
				branchLength *= wr;
			} break; // F
			case '+': {
				// do stuff
			} break; // +
			default:
				cout << "skipping unrecognized symbol: " << symbol << endl;
		} // switch
	} // for
}

void LTree::draw() {
	for( BranchNode* child : branches ) {
		// cout << "drawing child" << endl;
		child->draw();
		// cout << "child drawn" << endl;
	} // for
}
