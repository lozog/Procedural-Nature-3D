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
									 vector<Rule*> rules,
									 unsigned int depth ) {
	string result = axiom;
	for ( unsigned int i = 0; i < depth; i += 1 ) {
		for ( Rule* rule : rules ) {
			ReplaceString( result, rule->LHS, rule->RHS );
		} // for
		// cout << result << endl;
	} // for
	return result;
}