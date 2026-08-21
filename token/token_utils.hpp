#pragma once

#include <string>
#include "token_types.hpp"

using namespace std;

bool isWhiteSpace(string str);
bool isDelimeter(string str);
bool isOperator(string str);
bool isKeyWord(string str);

bool isAlpha(char c);
bool isDigit(char c);

bool isValidIdentifier(string str,int row,int col);

bool isDecimalInteger(string s);
bool isBinaryOctalHex(string s,int row,int col);
bool isFloatingPoint(string s,int row,int col);
bool isHexDigit(char c);
bool isNumberLiteral(string s,int row,int col);

bool isDotOperatorToken(string token,int row,int col);

bool mergeTokens(vector<TRC>& tokens);