#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

extern unordered_map<string,string> operators;
extern unordered_set<string> keywords;

extern unordered_set<char> alpha;
extern unordered_set<char> digits;

extern unordered_map<string,string> delimiters;
extern unordered_map<string,string> whitespace;