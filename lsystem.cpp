#include "lsystem.hpp"

#include <iostream>
using namespace std;

string LSystem::generateExpr( string axiom,
									 Rules rules,
									 unsigned int depth ) {

	// parallel rewriting of production rules
	string result = axiom;
	for ( unsigned int i = 0; i < depth; i += 1 ) {

		string newResult = "";
		size_t ruleLength = result.length();
		string newRulePieces[ruleLength];

		for ( unsigned int j = 0; j < ruleLength; j += 1 ) {

			string rulePiece = "";
			rulePiece += result.at(j);		// implicit string cast
			newRulePieces[j] = rulePiece;
			for ( Rule* rule : rules ) {

				if( rulePiece == rule->LHS ) {
					newRulePieces[j] = rule->RHS;
					break;
				} // if

			} // for
			newResult += newRulePieces[j];
		} // for
		result = newResult;
	} // for

	return result;
}