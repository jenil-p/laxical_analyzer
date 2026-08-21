#include "firstFollow.hpp"
#include <bits/stdc++.h>
using namespace std;

map<string, vector<vector<string>>> grammar;
set<string> terminals;
set<string> nonTerminals;

map<string, set<string>> FIRST;
map<string, set<string>> FOLLOW;

string startSymbol;

set<string> findFirst(string X)
{
    if (terminals.count(X))
    {
        return {X};
    }

    if (FIRST.count(X))
    {
        return FIRST[X];
    }

    set<string> result;

    for (auto production : grammar[X])
    {
        bool allEps = true;

        for (auto symbol : production)
        {
            set<string> temp = findFirst(symbol);

            for (auto t : temp)
            {
                if (t != "eps")
                {
                    result.insert(t);
                }
            }

            if (!temp.count("eps"))
            {
                allEps = false;
                break;
            }
        }

        if (allEps)
        {
            result.insert("eps");
        }
    }

    return FIRST[X] = result;
}

void computeFollowSets()
{
    FOLLOW[startSymbol].insert("$");

    bool changed = true;

    while (changed)
    {
        changed = false;

        for (auto &rule : grammar)
        {
            string A = rule.first;

            for (auto &prod : rule.second)
            {
                for (int i = 0; i < prod.size(); i++)
                {
                    string B = prod[i];

                    if (!nonTerminals.count(B))
                    {
                        continue;
                    }

                    if (i + 1 < prod.size())
                    {
                        string next = prod[i + 1];
                        set<string> f = findFirst(next);

                        for (auto t : f)
                        {
                            if (t != "eps")
                            {
                                changed |= FOLLOW[B].insert(t).second;
                            }
                        }

                        if (f.count("eps"))
                        {
                            for (auto t : FOLLOW[A])
                            {
                                changed |= FOLLOW[B].insert(t).second;
                            }
                        }
                    }
                    else
                    {
                        for (auto t : FOLLOW[A])
                        {
                            changed |= FOLLOW[B].insert(t).second;
                        }
                    }
                }
            }
        }
    }
}

void loadGrammarFromFile(string path)
{
    ifstream fin(path);

    int n; // number of grammer rules that are given ...
    fin >> n;

    fin >> startSymbol; // starting symbole for computeing FOLLOW set

    string dummy;
    getline(fin, dummy);

    for (int i = 0; i < n; i++)
    {
        string line;
        getline(fin, line);

        stringstream ss(line);

        string lhs, arrow;
        ss >> lhs >> arrow;

        nonTerminals.insert(lhs);

        vector<string> prod;
        string sym;

        while (ss >> sym)
        {
            if (sym == "|")
            {
                grammar[lhs].push_back(prod);
                prod.clear();
            }
            else
            {
                prod.push_back(sym);
            }
        }

        grammar[lhs].push_back(prod);
    }

    for (auto &rule : grammar)
    {
        for (auto &prod : rule.second)
        {
            for (auto &sym : prod)
            {
                if (!nonTerminals.count(sym))
                {
                    terminals.insert(sym);
                }
            }
        }
    }
}

void computeFirst()
{
    for (auto nt : nonTerminals)
    {
        findFirst(nt);
    }
}
