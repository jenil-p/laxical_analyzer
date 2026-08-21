#include "token_utils.hpp"
#include "../tables/language_tables.hpp"
#include "../token/token_types.hpp"
#include <iostream>

using namespace std;

bool isWhiteSpace(string str)
{
    return whitespace.find(str) != whitespace.end();
}

bool isDelimeter(string str)
{
    return delimiters.find(str) != delimiters.end();
}

bool isOperator(string str)
{
    return operators.find(str) != operators.end();
}

bool isKeyWord(string str)
{
    return keywords.find(str) != keywords.end();
}

// -----------------------------
//         ALPHABATES CHECK
// -----------------------------
bool isAlpha(char c)
{
    return alpha.find(c) != alpha.end();
}

// -----------------------------
//         DIGITS CHECK
// -----------------------------
bool isDigit(char c)
{
    return digits.find(c) != digits.end();
}

// -----------------------------
//         Valid identifier check
// -----------------------------
bool isValidIdentifier(string str, int row, int col)
{
    if (str.size() > 64)
    {
        cout << "Lexical Error: too long identifier " << str << " at " << row << ":" << col << endl;
        return false; // too long identifier.
    }
    if (isAlpha(str[0]) || str[0] == '_')
    { // checking first char.
        for (int i = 1; i < str.size(); i++)
        {
            if (!isAlpha(str[i]) && !isDigit(str[i]) && str[i] != '_')
            {
                cout << "Lexical Error: Invalid symbol in Indentifier" << str[i] << " at " << row << ":" << col + i << endl;
                return false;
            }
        }
        return true;
    }
    else
    {
        cout << "Lexical Error: Invalid start of Identifier " << str[0] << " at " << row << ":" << col << endl;
        return false;
    }
}

// -----------------------------
//         Decimal int check
// -----------------------------
bool isDecimalInteger(string s)
{
    if (s.empty())
        return false;

    for (char c : s)
    {
        if (!isDigit(c))
            return false;
    }
    return true;
}

// -----------------------------
//         BINARY CHECK
// -----------------------------
bool isBinaryOctalHex(string s, int row, int col)
{
    if (s.size() < 3 || s[0] != '0')
        return false;

    char base = s[1];

    if (base == 'b')
    { // binary
        for (int i = 2; i < s.size(); i++)
        {
            if (s[i] != '0' && s[i] != '1')
            {
                cout << "Lexical Error: Invalid binary numeric literal " << s << " at " << row << ":" << col << endl;
                return false;
            }
        }
        return true;
    }

    if (base == 'o')
    { // octal
        for (int i = 2; i < s.size(); i++)
        {
            if (s[i] < '0' || s[i] > '7')
            {
                cout << "Lexical Error: Invalid octal literal " << s << " at " << row << ":" << col << endl;
                return false;
            }
        }
        return true;
    }

    if (base == 'x')
    { // hex
        for (int i = 2; i < s.size(); i++)
        {
            char c = s[i];
            if (!isDigit(c) && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
            {
                cout << "Lexical Error: Invalid hex literal " << s << " at " << row << ":" << col << endl;
                return false;
            }
        }
        return true;
    }

    if (isAlpha(base))
    {
        cout << "Lexical Error: Invalid numeric literal " << s << " at " << row << ":" << col << endl;
    }

    return false;
}

// -----------------------------
//         FLOATING CHECK
// -----------------------------
bool isFloatingPoint(string s, int row, int col)
{
    bool hasDigit = false;
    bool hasDot = false;
    bool hasExp = false;

    for (int i = 0; i < s.size(); i++)
    {
        char c = s[i];

        if (isDigit(c))
        {
            hasDigit = true;
        }
        else if (c == '.' && !hasDot && !hasExp)
        {
            hasDot = true;
        }
        else if ((c == 'e' || c == 'E') && hasDigit && !hasExp)
        {
            hasExp = true;
            hasDigit = false; // digits should be ythere after exponential.
        }
        else if ((c == '+' || c == '-') &&
                 (s[i - 1] == 'e' || s[i - 1] == 'E'))
        {
            continue;
        }
        else
        {
            cout << "Lexical Error: Invalid float number" << s << " at " << row << ":" << col << endl;
            return false;
        }
    }

    return hasDigit && (hasDot || hasExp);
}

// -----------------------------
//         HEX charecter CHECK
// -----------------------------
bool isHexDigit(char c)
{
    return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

// -----------------------------
//         NUMBER CHECK
// -----------------------------
bool isNumberLiteral(string s, int row, int col)
{
    // Binary / Octal / Hex
    if (s.size() > 1 && s[0] == '0' &&
        (s[1] == 'b' || s[1] == 'o' || s[1] == 'x'))
    {
        return isBinaryOctalHex(s, row, col);
    }

    // Floating point (contains '.' or exponent)
    if (s.find('.') != string::npos ||
        s.find('e') != string::npos ||
        s.find('E') != string::npos)
    {
        return isFloatingPoint(s, row, col);
    }

    // Decimal integer
    if (isDecimalInteger(s))
        return true;

    // Otherwise invalid number
    cout << "Lexical Error: Invalid numeric literal "
         << s << " at " << row << ":" << col << endl;
    return false;
}

// ----------------------------
//        DOT AS OPERATOR
// ----------------------------
bool isDotOperatorToken(string token, int row, int col)
{
    int dotCount = 0;
    for (char c : token)
        if (c == '.')
            dotCount++;

    if (dotCount != 1)
        return false;

    int pos = token.find('.');
    string left = token.substr(0, pos);
    string right = token.substr(pos + 1);

    if (left.empty() || right.empty())
    {
        cout << "Lexical Error: Invalid use of '.' at "
             << row << ":" << col << endl;
        return false;
    }

    if (isValidIdentifier(left, row, col) &&
        isValidIdentifier(right, row, col + pos + 1))
    {
        return true;
    }

    cout << "Lexical Error: Invalid member access "
         << token << " at " << row << ":" << col << endl;
    return false;
}

// -----------------------------
//        merge the tokens
// -----------------------------
bool mergeTokens(vector<TRC> &tokens)
{
    vector<TRC> merged;

    for (int i = 0; i < tokens.size(); i++)
    {
        if (isOperator(tokens[i].first))
        {
            string op = tokens[i].first;

            string bestMatch = ""; // NEW: stores longest valid operator
            int bestIndex = i;     // NEW: tracks last index of valid match

            string temp = op;

            // NEW: try extending operator step-by-step (maximal munch)
            for (int j = i; j < tokens.size(); j++)
            {
                if (!isOperator(tokens[j].first))
                    break;

                if (j == i)
                    temp = tokens[j].first;
                else
                    temp += tokens[j].first;

                if (isOperator(temp)) // NEW: update only if valid operator
                {
                    bestMatch = temp; // NEW: store longest valid operator so far
                    bestIndex = j;    // NEW: remember how far we matched
                }
            }

            if (bestMatch != "")
            {
                merged.push_back({bestMatch,
                                  {tokens[i].second.first, tokens[i].second.second}});

                i = bestIndex; // NEW: jump ahead (consume all matched tokens)
            }
            else
            {
                cout << "Lexical Error: Invalid operator "
                     << tokens[i].first << " "
                     << tokens[i].second.first << ":"
                     << tokens[i].second.second << endl;
            }
        }
        else
        {
            merged.push_back(tokens[i]);
        }
    }

    tokens = merged;
    return true;
}