#include <fstream>
#include <iostream>

#include "lexer.hpp"
#include "../token/token_types.hpp"
#include "../token/token_utils.hpp"
#include "../tables/language_tables.hpp"

using namespace std;

void generateTokensFromFile(string filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error opening file\n";
        return;
    }

    string line;
    int lineNo = 1; // treak line no.

    bool insideDocComment = false;
    bool insideComment = false;
    int commentLayer = 0;
    string token = "";
    while (getline(file, line))
    {
        int tokenStartCol = -1;
        bool insideString = false; // state --> normal or inside_string.
        bool stringHasError = false;
        for (int i = 0; i < line.size(); i++) // iterate through each character in line
        {
            string s = "";
            s += line[i]; // convert char into string

            if (insideString) // if inside string then add all in one token rather then seperate tokens
            {
                if (s == "\\") // check if / occurs then next must be n,t,/,",xNN if not then invalid escape seq..
                {
                    if (i + 1 < line.size())
                    {
                        if (line[i + 1] != 'n' && line[i + 1] != 't' && line[i + 1] != '\\' && line[i + 1] != '\"')
                        {
                            if (line[i + 1] == 'x' && i + 2 < line.size() && isHexDigit(line[i + 2]) && i + 3 < line.size() && isHexDigit(line[i + 3]))
                            {
                                // valid escape seq.
                                s += "x";
                                s += line[i + 2];
                                s += line[i + 3];
                                i = i + 3;
                            }
                            else
                            {
                                stringHasError = true;
                                cout << "Invalid escape sequence at line " << lineNo << endl;
                            }
                        }
                        else
                        {
                            s += line[i + 1];
                            i++;
                        }
                    }
                }
                else if (s == "\"") // if inside string and " occurs without \ then it must be end of string
                {
                    insideString = false;
                    if (stringHasError)
                    {
                        s = "";
                        token.clear();
                    }
                }
                if (token.empty())
                {
                    tokenStartCol = i;
                }
                token += s;
            }
            else if (insideComment)
            {
                if (s == "*" && i + 1 < line.size() && line[i + 1] == '/')
                {
                    commentLayer--;
                    if (commentLayer <= 0)
                    {
                        insideComment = false;
                    }
                    i = i + 1;
                }
                else if (s == "/" && i + 1 < line.size() && line[i + 1] == '*')
                {
                    commentLayer++;
                }
            }
            else if (insideDocComment)
            {
                if (s == "/" && i + 1 < line.size() && line[i + 1] == '*')
                {
                    insideComment = true;
                    commentLayer++;
                    s = "";
                }
                else if (s == "*" && i + 1 < line.size() && line[i + 1] == '/')
                {
                    insideDocComment = false;
                    token += "*/";
                    docComments.push_back(token);
                    s = "";
                    i = i + 1;
                    token = "";
                }
                if (token.empty())
                {
                    tokenStartCol = i;
                }
                token += s;
            }
            else if (s == "/" && i + 1 < line.size() && line[i + 1] == '*')
            { // check wheather mutiline comment is opening?
                if (i + 2 < line.size() && line[i + 2] == '*')
                {
                    insideDocComment = true;
                    if (token.empty())
                    {
                        tokenStartCol = i;
                    }
                    token += s;
                }
                else
                {
                    insideComment = true;
                    commentLayer++;
                }
            }
            else if (s == "/" && i + 1 < line.size() && line[i + 1] == '/')
            { // single line doc comment.
                if (i + 2 < line.size() && line[i + 2] == '/')
                {
                    docComments.emplace_back(line.begin() + i, line.end());
                }
                break;
            }

            else if (s == "\"") // if " occrurs and not in inside string then it should be start of string
            {
                insideString = true;
                if (token.empty())
                {
                    tokenStartCol = i;
                }
                token += s;
            }
            else if (isWhiteSpace(s)) // if in the normal state and space occurs then seperate the token
            {
                if (token.size() > 0)
                {
                    tokens.push_back({token, {lineNo, tokenStartCol}});
                    token = "";
                }
            }
            else if (isDelimeter(s)) // if in the normal state and delimeter occurs then seperate the token
            {
                if (token.size() > 0)
                {
                    tokens.push_back({token, {lineNo, tokenStartCol}});
                    token = "";
                }
                tokens.push_back({s, {lineNo, i}});
            }
            else if (isOperator(s)) // if in the normal state and operator occurs then seperate the token
            {
                if (token.size() > 0)
                {
                    tokens.push_back({token, {lineNo, tokenStartCol}});
                    token = "";
                }
                tokens.push_back({s, {lineNo, i}});
            }
            else // if nothing then add that character to token
            {
                if (token.empty())
                {
                    tokenStartCol = i;
                }
                token += s;
            }
        } // end of for

        if (insideString)
        { // unterminated string
            cout << "Unterminated string at " << lineNo << "\n";
            token.clear();
        }

        if (!token.empty() && !insideDocComment)
        {
            tokens.push_back({token, {lineNo, tokenStartCol}});
            token.clear();
        }

        lineNo++;
    } // end of while
    file.close();
    if (insideDocComment)
    { // unterminated doc comment
        cout << "Unterminated DocComment " << lineNo << "\n";
        token.clear();
    }
    if (insideComment)
    { // unterminated comment
        cout << "Unterminated Comment " << lineNo << "\n";
        token.clear();
    }
    return;
}


void validateTokens()
{
    for (auto it : tokens)
    {
        string token = it.first;
        int row = it.second.first;
        int col = it.second.second;
        if (token[0] == '"')
        {
            validTokens.push_back({{token, "String"}, {row, col}});
            continue;
        }
        if (isKeyWord(token))
        {
            validTokens.push_back({{token, "Keywords"}, {row, col}});
        }
        else if (isDelimeter(token))
        {
            validTokens.push_back({{token, "Delimiter"}, {row, col}});
            string parserToken = delimiters[token];

            // Only push tokens used in grammar
            if (parserToken == "LPAREN" ||
                parserToken == "RPAREN" ||
                parserToken == "SEMICOLON")
            {
                tokenStream.push_back(parserToken);
            }
        }
        else if (isOperator(token))
        {
            validTokens.push_back({{token, "Operator"}, {row, col}});

            string parserToken = operators[token];

            // Only operators used by grammar
            if (parserToken == "PLUS" ||
                parserToken == "MINUS" ||
                parserToken == "MULTIPLY" ||
                parserToken == "DIVIDE" ||
                parserToken == "ASSIGN")
            {
                tokenStream.push_back(parserToken);
            }
        }
        else if (token[0] == '.' && isFloatingPoint(token, row, col))
        {
            validTokens.push_back({{token, "Numeric Literal"}, {row, col}});
        }
        else if (isDigit(token[0]))
        {
            if (isNumberLiteral(token, row, col))
            {
                validTokens.push_back({{token, "Numeric Literal"}, {row, col}});
                tokenStream.push_back("NUM");
            }
        }

        else if (token.find('.') != string::npos)
        {
            if (isDotOperatorToken(token, row, col))
            {
                string left = token.substr(0, token.find('.'));

                validTokens.push_back({{left, "Identifier"}, {row, col}});
                validTokens.push_back({{".", "Operator"}, {row, col + left.size()}});
                validTokens.push_back({{token.substr(token.find('.') + 1), "Identifier"}, {row, col + left.size() + 1}});
            }
        }

        else if (isValidIdentifier(token, row, col))
        {
            validTokens.push_back({{token, "Identifier"}, {row, col}});
            tokenStream.push_back("ID");
            if (symboleTable.find(token) == symboleTable.end())
            {
                symboleTable[token] = row;
            }
        }
        else
        {
            cout << "Lexical Error: Invalid token " << token << " at " << row << ":" << col << endl;
        }
    }
    tokenStream.push_back("$");


    cout << "Symbole Table : \n";
    cout << "lexeme rowNumber\n";
    for(auto& it: symboleTable){
        cout << it.first << " " << it.second << endl;
    }
}

