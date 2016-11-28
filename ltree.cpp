#include "ltree.hpp"

#include <cmath>					// sin, cos
#include <glm/gtx/io.hpp>
#include <stack>

#include <iostream>
using namespace std;

struct BranchInfo {
	glm::vec3 heading;
	glm::vec3 left;
	glm::vec3 up;
	glm::vec3 origin;
	float radius, nextRadius, branchLength;
};

LTree::LTree() {}

LTree::~LTree() {}
/*
Ru(a) =  cos a   sin a     0
		-sin a   cos a     0
		   0       0       1

Rl(a) =  cos a     0    -sin a
		   0       1       0
		 sin a     0     cos a

Rh(a) =    1       0       0
		   0     cos a  -sin a
		   0     sin a   cos a
*/
// remember to transpose, default glm::mat constructors are column-first
glm::mat3 LTree::Ru( float a ) {
	glm::mat3 result = glm::transpose(glm::mat3( 		cos(a), sin(a), 0.0f,
												 -1.0f* sin(a), cos(a), 0.0f,
												 		0.0f, 	0.0f, 	1.0f
						));
	return result;
}
glm::mat3 LTree::Rl( float a ) {
	glm::mat3 result = glm::transpose(glm::mat3( cos(a), 0.0f, -1.0f* sin(a),
												 0.0f, 	 1.0f, 		  0.0f,
												 sin(a), 0.0f, 		  cos(a)
						));
	return result;
}
glm::mat3 LTree::Rh( float a ) {
	glm::mat3 result = glm::transpose(glm::mat3( 1.0f, 0.0f, 		  0.0f,
												 0.0f, cos(a), -1.0f* sin(a),
												 0.0f, sin(a), 		  cos(a)
						));
	return result;
}
void LTree::init( glm::vec3 heading, glm::vec3 down, glm::vec3 origin, std::string expr,
				 float r1, /*float r2,*/ float a0, /*float a1, float a2,*/ float wr,
				 ShaderProgram& m_shader, GLuint m_texture ) {
	LTree::origin = origin;
	LTree::expr = expr;

	a0 = glm::radians(a0);

	glm::vec3 left 	= glm::normalize(glm::cross(-1.0f * down, heading));
	glm::vec3 up 	= glm::cross(heading, left);

	unsigned int levelOfDetail = 2;
	float radiusOfStump = 1.0f;
	float initialBranchLength = 5.0f;

	float radius = radiusOfStump;
	float nextRadius = r1 * radius;
	float branchLength = initialBranchLength;

	stack<BranchInfo*> branchInfoStack;

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

				#if 0
				cout << radius << " " << nextRadius << endl;
				cout << origin << endl;
				#endif

				origin += (branchLength * up);			// for now, grows up

				radius = nextRadius;
				nextRadius *= r1;
				branchLength *= wr;
			} break; // F
			case '+': {
				// cout << "old: " << heading << left << up << endl;
				glm::mat3 newHLU = glm::mat3(heading, left, up) * Ru(a0);
				heading = newHLU[0];
				left 	= newHLU[1];
				up		= newHLU[2];
				// cout << "new: " << heading << left << up << endl;
			} break; // +
			case '-': {
				// cout << "old: " << heading << left << up << endl;
				glm::mat3 newHLU = glm::mat3(heading, left, up) * Ru(-a0);
				heading = newHLU[0];
				left 	= newHLU[1];
				up		= newHLU[2];
				// cout << "new: " << heading << left << up << endl;
			} break; // -
			case '&': {
				// cout << "old: " << heading << left << up << endl;
				glm::mat3 newHLU = glm::mat3(heading, left, up) * Rl(a0);
				heading = newHLU[0];
				left 	= newHLU[1];
				up		= newHLU[2];
				// cout << "new: " << heading << left << up << endl;
			} break; // &
			case '^': {
				// cout << "old: " << heading << left << up << endl;
				glm::mat3 newHLU = glm::mat3(heading, left, up) * Rl(-a0);
				heading = newHLU[0];
				left 	= newHLU[1];
				up		= newHLU[2];
				// cout << "new: " << heading << left << up << endl;
			} break; // ^
			case '[': {
				BranchInfo* branchInfo = new BranchInfo();
				branchInfo->heading 		= heading;
				branchInfo->left 		= left;
				branchInfo->up 			= up;
				branchInfo->origin 		= origin;
				branchInfo->radius 		= radius;
				branchInfo->nextRadius 	= nextRadius;
				branchInfo->branchLength = branchLength;
				branchInfoStack.push(branchInfo);
				// do stuff
			} break; // [
			case ']': {
				// cout << "old: " << heading << left << up << endl;
				BranchInfo* branchInfo = branchInfoStack.top();
				heading 		= branchInfo->heading;
				left 			= branchInfo->left;
				up 				= branchInfo->up;
				origin 			= branchInfo->origin;
				radius 			= branchInfo->radius;
				nextRadius 		= branchInfo->nextRadius;
				branchLength 	= branchInfo->branchLength;
				delete branchInfo;
				branchInfoStack.pop();
				// cout << "new: " << heading << left << up << endl;

			} break; // ]
			// TODO: if the next two cases are swapped, there is a compile error. WHY?
			case '/': {
				// cout << "old: " << heading << left << up << endl;
				glm::mat3 newHLU = glm::mat3(heading, left, up) * Rh(-a0);
				heading = newHLU[0];
				left 	= newHLU[1];
				up		= newHLU[2];
				// cout << "new: " << heading << left << up << endl;
			} break; // /
			case '\\': {
				// cout << "old: " << heading << left << up << endl;
				glm::mat3 newHLU = glm::mat3(heading, left, up) * Rh(a0);
				heading = newHLU[0];
				left 	= newHLU[1];
				up		= newHLU[2];
				// cout << "new: " << heading << left << up << endl;
			} break; // \
			
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
