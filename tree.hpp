#pragma once

#include <string>

#include "branch.hpp"

/***********************
From The Algorithmic Beauty of Plants, Chapter 1 (section 1.5, Modeling in Three Dimensions)

Symbols:
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

Also need:

r1		contraction ratio for trunk
r2  	contraction ratio for branches
a0		branching angle from the trunk
a1		branching angle for lateral axes (?)
a2		divergence angle (this is a from above)
wr		width decrease rate

For simplicity, only use r1, a0, and wr

************************/

class Tree {
public:
	Tree();
	~Tree();

	void init( glm::mat3 HLU, glm::vec3 origin, string expr,
		float r1, /*float r2,*/ float a0, /*float a1, float a2,*/ float wr );
	Branch* getRoot();
private:
	Branch* rootSegment;
	glm::vec3 origin;
	string expr;					// L-system expression
};