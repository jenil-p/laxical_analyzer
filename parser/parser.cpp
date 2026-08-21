#include "parser.hpp"
#include "../token/token_types.hpp"
#include "../firstFollow/firstFollow.hpp"
#include <stack>
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>  
unordered_map<string, unordered_map<string, vector<string>>> parsingTable;

// build parsing table
void buildParsingTable()
{
    for (auto row : grammar)
    {
        string prod = row.first;
        for (auto rule : row.second)
        {
            bool containsEps = false;
            set<string> firstOfRule = findFirst(rule[0]);
            for (auto termi : firstOfRule)
            {
                if (termi == "eps")
                {
                    containsEps = true;
                    continue;
                }
                parsingTable[prod][termi] = rule;
            }
            if (containsEps == true)
            {
                for (auto folo : FOLLOW[prod])
                {
                    parsingTable[prod][folo] = rule;
                }
            }
        }
    }
}


void predictiveParser()
{
    buildParsingTable();

    // printing parsing table ...
    // cout << "\n-----------------------------------------\n";
    // cout << "\nPARSING TABLE :\n" << endl;
    // for(auto& it: parsingTable){
    //     string nonT = it.first;
    //     for(auto& T: it.second){
    //         cout << "ParsingTable" << "[" << nonT << "]" << "[" << T.first << "]" << " = ";
    //         for(auto& prod: T.second){
    //            cout << prod << " ";
    //         }
    //         cout << endl;
    //     }
    // }

    stack<string> st;
    st.push("$");
    st.push("Stmt");

    int ptr = 0;

    cout << left << setw(50) << "Stack"        
         << left << setw(50) << "Input"        
         << "Action" << endl;                   
    cout << string(150, '-') << endl;         

    while (!st.empty())
    {
        string top = st.top();
        string input = tokenStream[ptr];

        stack<string> temp = st;                
        // this vector thing and reverse and these things are for printing the current state of stack, input , actions...             
        vector<string> stkElems; 
        while (!temp.empty())                    
        {
            stkElems.push_back(temp.top());     
            temp.pop();                          
        }
        reverse(stkElems.begin(), stkElems.end()); 
        string stackStr = "";
        for (auto &s : stkElems){                
            stackStr += s + " "; 
        }               
        
        string inputStr = "";                    
        for (int i = ptr; i < tokenStream.size(); i++){ 
            inputStr += tokenStream[i] + " ";
        }

        string action;                           

        if (top == input)
        {
            action = "Match " + input;          
            st.pop();
            ptr++;
        }
        else if (parsingTable[top].count(input))
        {
            vector<string> rule = parsingTable[top][input];
            action = top + " -> ";              
            for (auto &s : rule) {               
                action += s + " ";  
            }            
            
            st.pop();
            if (rule[0] != "eps")
            {
                for (int i = rule.size() - 1; i >= 0; i--){
                    st.push(rule[i]);
                }
            }
        }
        else
        {
            cout << "\nSyntax Error at token '" << input 
                 << "' near position " << (ptr + 1); 
            if (ptr > 0){
                cout << " after '" << tokenStream[ptr - 1] << "'";
            }
            cout << endl;
            return;
        }

        cout << left << setw(50) << stackStr       
             << left << setw(50) << inputStr       
             << action << endl;                     
    }

    cout << "\nParsing Successful\n";
}