#include <iostream>
#include "lexer/lexer.hpp"
#include "token/token_utils.hpp"
#include "token/token_types.hpp"
#include "firstFollow/firstFollow.hpp"
#include "parser/parser.hpp"

using namespace std;

int main()
{
    // -------------------------- Laxical analyzer ---------------------------------- //
    generateTokensFromFile("test.mlpp");

    mergeTokens(tokens);

    validateTokens();

    cout << "\n-----------------------------------------\n";
    cout << "Token Stream :\n";
    
    for(auto t : tokenStream)
    cout << t << " ";
    
    cout << "\n-----------------------------------------\n";
    cout << endl;
    
    
    // -------------------------- Syntax analyzer ---------------------------------- //
    cout << endl;
    cout << "Parsing Started...." << endl;
    
    loadGrammarFromFile("grammar.txt");
    
    computeFirst();
    computeFollowSets();
    
    cout << "\n-----------------------------------------\n";
    cout << "\nFIRST SET OF ALL NON-TERMINALS:\n" << endl;
    for(auto it : FIRST){
        auto st = it.second;
        cout << it.first << " --> ";
        for(auto f: st){
            cout << f << " ";
        }
        cout << endl;
    }
    
    cout << "\n-----------------------------------------\n";
    cout << "\nFOLLOW SET OF ALL NON-TERMINALS:\n" << endl;
    for(auto it : FOLLOW){
        auto st = it.second;
        cout << it.first << " --> ";
        for(auto f: st){
            cout << f << " ";
        }
        cout << endl;
    }
    cout << "\n\n";
    

    predictiveParser();
    return 0;
}