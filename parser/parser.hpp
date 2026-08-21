#pragma once
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

extern unordered_map<string, unordered_map<string, vector<string>>> parsingTable;

void buildParsingTable();
void predictiveParser();