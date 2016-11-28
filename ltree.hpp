#pragma once

#include <string>
#include <vector>

#include "branchnode.hpp"

/***********************
Procedural generation of trees using L-systems
Based on Chapter 1 of The Algorithmic Beauty of Plants

H, L, and U are the heading, left, and up vectors associated with the turtle.
L = V x H and U = H x L, where V is the direction opposite gravity.
All three are normalized, as they are direction vectors.

Symbols:
F   	move forward a step, drawing the branch
f       move forward a step, without drawing the branch (TODO: will I need this?)
+		turn left by angle a using matrix Ru(a)
-		turn right by angle a using matrix Ru(-a)
&		pitch down by angle a using matrix Rl(a)
^		pitch up by angle a using matrix Rl(-a)
\		roll left by angle a using matrix Rl(a)
/		roll right by angle a using matrix Rl(-a)
[		push current state of the turtle onto a stack
]		pop a state from the stack anf make it the current state of the turtle

Matrices:

Ru(a) =  cos a   sin a     0
		-sin a   cos a     0
		   0       0       1

Rl(a) =  cos a     0    -sin a
		   0       1       0
		 sin a     0     cos a

Rh(a) =    1       0       0
		   0     cos a  -sin a
		   0     sin a   cos a

These parameters are also used:

r1		contraction ratio for trunk
r2  	contraction ratio for branches
a0		branching angle from the trunk
a1		branching angle for lateral axes (?)
a2		divergence angle (this is a from above)
wr		width decrease rate

For simplicity, only use r1, a0, and wr

************************/

class LTree {
public:
	LTree();
	~LTree();

	// a0 is angle in degrees
	void init( 	glm::vec3 heading, glm::vec3 down, glm::vec3 origin, std::string expr,
				float r1, /*float r2,*/ float a0, /*float a1, float a2,*/ float wr,
				ShaderProgram& m_shader, GLuint m_texture );
	void draw();
private:
	// angle in radians
	glm::mat3 Ru( float angle );
	glm::mat3 Rl( float angle );
	glm::mat3 Rh( float angle );

	glm::vec3 origin;
	std::string expr;					// L-system expression
	std::vector<BranchNode*> branches;
};