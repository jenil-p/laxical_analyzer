#include "language_tables.hpp"



// -----------------------------
//         Operators
// -----------------------------
unordered_map<string, string> operators = {
    {"+", "PLUS"},
    {"-", "MINUS"},
    {"*", "MULTIPLY"},
    {"/", "DIVIDE"},
    {"%", "MODULO"},

    {"==", "EQUAL"},
    {"!=", "NOT_EQUAL"},
    {"<", "LESS"},
    {"<=", "LESS_EQUAL"},
    {">", "GREATER"},
    {">=", "GREATER_EQUAL"},

    {"&&", "LOGICAL_AND"},
    {"||", "LOGICAL_OR"},
    {"!", "LOGICAL_NOT"},

    {"=", "ASSIGN"},
    {"+=", "PLUS_ASSIGN"},
    {"-=", "MINUS_ASSIGN"},
    {"*=", "MULTIPLY_ASSIGN"},
    {"/=", "DIVIDE_ASSIGN"},

    {"->", "ARROW"},
    {"::", "SCOPE"},
};


// -----------------------------
//         Keywords
// -----------------------------
unordered_set<string> keywords = {
    "if", "else", "while", "for", "return", "break", "continue", "fn", "let", "const", "struct", "enum", "match"
};


// -----------------------------
//        Alphabets
// -----------------------------
unordered_set<char> alpha = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};


// -----------------------------
//         Digits
// -----------------------------
unordered_set<char> digits = {
    '0','1','2','3','4','5','6','7','8','9'
};

// -----------------------------
//         WHITESPACES
// -----------------------------
unordered_map<string,string> whitespace = {
    {" ","SPACE"},
    {"\t","TAB"},
    {"\n","NEWLINE"}
};


// -----------------------------
//         Delimiters
// -----------------------------
unordered_map<string, string> delimiters = {
    {"(", "LPAREN"},
    {")", "RPAREN"},
    {"{", "LBRACE"},
    {"}", "RBRACE"},
    {"[", "LBRACKET"},
    {"]", "RBRACKET"},

    {",", "COMMA"},
    {";", "SEMICOLON"},
    {":", "COLON"}};