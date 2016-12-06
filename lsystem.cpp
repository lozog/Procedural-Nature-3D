#include "lsystem.hpp"

#include <iostream>
using namespace std;

// helper function
// modifies subject so all occurrences of search are changed to replace
// taken from http://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
void ReplaceString(string& subject, const string& search,
                          const string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    } // while
}

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