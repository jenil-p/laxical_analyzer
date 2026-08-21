#pragma once
#include <map>
#include <set>
#include <vector>
#include <string>

using namespace std;

extern map<string, vector<vector<string>>> grammar;
extern set<string> terminals;
extern set<string> nonTerminals;

extern map<string, set<string>> FIRST;
extern map<string, set<string>> FOLLOW;

extern string startSymbol;

void loadGrammarFromFile(string path);
void computeFirst();
void computeFollowSets();

set<string> findFirst(string X); // for making parse table...