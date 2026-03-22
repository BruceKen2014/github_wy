#pragma once

#include "StateProduction.h"

using namespace std;

class LR0State
{
public:
    inline static int ID = 0;
    LR0State() { id = ID++; };
    LR0State(const StateProduction& inStateProduction) :productions{ inStateProduction } {
        id = ID++;
    };
    bool add_production(const StateProduction& production)
    {
        // 如果生产已经存在于状态中，则不添加
        if (std::find(productions.begin(), productions.end(), production) != productions.end())
            return false;
        productions.push_back(production);
        return true;
    }
    bool empty() const { return productions.empty(); }
    bool operator == (const LR0State& other) const
    {
        if (productions.size() != other.productions.size())
            return false;
        return std::equal(productions.begin(), productions.end(), other.productions.begin());
    }
    string getFormatString() const
    {
        string result = "State " + to_string(id) + ":\n";
        for (const auto& prod : productions)
        {
            result += "  " + prod.getFormatString() + "\n";
        }
        return result;
    }
    void print_need_id() const
    {
        string result;
        for (const auto& prod : productions)
        {
			const ProductionRightItem& item = prod.right.at(prod.dotPos);
            result += "  " + item.getFormatString();
        }
        std::cout << result << endl;
	}
    void print() const
    {
        std::cout << getFormatString();
    }

public:
    int id = 0; // 状态ID，可以根据需要设置为唯一值
    std::vector<StateProduction> productions;
    map<Terminal, int> actionTable;
    map<NoTerminal, int> gotoTable;
};