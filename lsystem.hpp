#pragma once

#include <string>
#include <vector>

class Rule {
public:
	Rule( std::string LHS, std::string RHS ) : LHS(LHS), RHS(RHS) {}
	~Rule() {}
	std::string LHS;
	std::string RHS;
};

class LSystem {
public:
	LSystem() {}
	~LSystem() {}

	static std::string generateExpr( std::string axiom,
									 std::vector<Rule*> rules,
									 unsigned int depth );
};