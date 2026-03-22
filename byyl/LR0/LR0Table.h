
#pragma once

#include "Terminal.h"
#include "NoTerminal.h"
#include "StateProduction.h"
#include <set>
#include "ProductionSet.h"
#include "LR0State.h"
using namespace std;


class LR0Table
{
public:
	LR0Table() = default;
	~LR0Table() = default;

    void reset()
    {
        states.clear();
    }

    bool add_state(const LR0State& state)
    { 	
        if (contains_state(state))
            return false;
	    states.push_back(state);
        return true;
	}

    const LR0State& get_state(int id) const
    {
        for (const auto& state : states)
        {
            if (state.id == id)
                return state;
        }
	}
    LR0State& get_state(int id)
    {
        for (auto& state : states)
        {
            if (state.id == id)
                return state;
        }
    }

    bool contains_state(const LR0State& state) const
	{
		return std::find(states.begin(), states.end(), state) != states.end();
	}
    int get_state_id(const LR0State& inState) const
    {
        for (const auto& state : states)
        {
            if (state == inState)
                return state.id;
        }
        cout << "error happens" << endl;
        return -1;
    }
    string getFormatString() const
    {
        string result;
        for (const auto& state : states)
        {
            result += state.getFormatString() + "\n";
        }
        return result;
	}
    void print()
    {
		cout << getFormatString();
    }

    void build_LR0_table(const ProductionSet& productionSet);

    bool validate_tokens(const std::vector<Terminal>& tokens);

public:
    ProductionSet productionSet;
	vector<LR0State> states;
};
