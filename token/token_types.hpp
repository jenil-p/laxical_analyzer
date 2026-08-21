#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

using namespace std;

using TRC = pair<string, pair<int, int>>; // token -> {row , col}
using tokenTable = pair<pair<string, string>, pair<int, int>>; // {{lexeme, token type}, {row, col}}

extern vector<TRC> tokens;
extern vector<string> docComments;

extern vector<tokenTable> validTokens;
extern vector<string> tokenStream;
extern unordered_map<string, int> symboleTable;