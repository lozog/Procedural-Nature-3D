#pragma once

#include <string>
#include <vector>

struct Rule {
public:
	Rule( std::string LHS, std::string RHS ) : LHS(LHS), RHS(RHS) {}
	std::string LHS;
	std::string RHS;
};

typedef std::vector<Rule*> Rules;

class LSystem {
public:
	LSystem() {}
	~LSystem() {}

	static std::string generateExpr( std::string axiom,
									 Rules rules,
									 unsigned int depth );
};