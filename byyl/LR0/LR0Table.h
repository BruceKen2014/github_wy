
#pragma once

#include "Terminal.h"
#include "NoTerminal.h"
#include "Production.h"
#include <set>
#include "ProductionSet.h"

class StateProduction :public Production
{
public:
	int dotPos; // 点的位置，范围是0到right.count()，表示点在右部的哪个位置
};

class LR0State
{
public:
	LR0State();
	~LR0State();
public:
	std::set<StateProduction> productions;
};

class ACTION
{
public:
	ACTION(const Terminal& inTerminal, const LR0State& inState) : terminal(inTerminal), state(inState) {};
	~ACTION();

public:
	Terminal terminal;
	const LR0State& state;
};

class GOTO
{
public:
	GOTO(const NoTerminal& inNoTerminal, const LR0State& inState) : noTerminal(inNoTerminal), state(inState) {};
	~GOTO();

public:
	NoTerminal noTerminal;
	const LR0State& state;
};

class LR0Table
{
public:
	LR0Table() = default;
	~LR0Table() = default;

    // 内联函数实现
    inline void setStates(const std::vector<LR0State>& states) {
        this->states = states;
    }

    inline void setActionTable(const std::map<std::pair<int, Terminal>, ACTION>& actionTable) {
        this->actionTable = actionTable;
    }

    inline void setGotoTable(const std::map<std::pair<int, NoTerminal>, GOTO>& gotoTable) {
        this->gotoTable = gotoTable;
    }

    inline const std::vector<LR0State>& getStates() const {
        return states;
    }

    inline const std::map<std::pair<int, Terminal>, ACTION>& getActionTable() const {
        return actionTable;
    }

    inline const std::map<std::pair<int, NoTerminal>, GOTO>& getGotoTable() const {
        return gotoTable;
    }

private:
	std::vector<LR0State> states;
	std::map<std::pair<int, Terminal>, ACTION> actionTable;
	std::map<std::pair<int, NoTerminal>, GOTO> gotoTable;
};

extern void build_LR0_table(const ProductionSet& productionset, LR0Table& lr0Table);