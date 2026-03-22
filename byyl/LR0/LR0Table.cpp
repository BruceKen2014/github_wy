#include "LR0Table.h"
#include <queue>
#include <stack>


void closure(LR0State& state, const ProductionSet& productionSet)
{
	while (true)
	{
		bool is_change = false;
		for (const auto& production : state.productions)
		{
			if (production.dotPos < production.right.count())
			{
				auto symbol = production.right.at(production.dotPos);
				if (symbol.is_no_terminal())
				{
					// 对于每个非终结符，找到对应的产生式，并将点位置设置为0，加入状态
					for (const auto& prod : productionSet.all())
					{
						if (prod.left() == symbol.noTerminal)
						{
							StateProduction newProd(prod, 0);
							if(state.add_production(newProd))
								is_change = true;
						}
					}
				}
			}
		}
		if (!is_change)
			break;
	}
}

LR0State GOTO(const LR0State& state, const Terminal& terminal, const ProductionSet& productionSet)
{
	LR0State nextState;
	for (const StateProduction& stateProduction : state.productions)
	{// 如果点后面还有符号，并且这个符号是 terminal 且等于输入的 terminal，那么就把这个产生式的点向右移动一位，加入 nextState
		if (stateProduction.dotPos < stateProduction.right.count())
		{
			auto symbol = stateProduction.right.at(stateProduction.dotPos);
			if (symbol.is_terminal() && symbol.terminal == terminal)
			{
				StateProduction newProd(stateProduction, stateProduction.dotPos + 1);
				nextState.add_production(newProd);
			}
		}
	}
	closure(nextState, productionSet);
	return nextState;
}
LR0State GOTO(const LR0State& state, const NoTerminal& noTerminal, const ProductionSet& productionSet)
{
	LR0State nextState;
	for (const StateProduction& stateProduction : state.productions)
	{// 如果点后面还有符号，并且这个符号是 noTerminal 且等于输入的 noTerminal，那么就把这个产生式的点向右移动一位，加入 nextState
		if (stateProduction.dotPos < stateProduction.right.count())
		{
			auto symbol = stateProduction.right.at(stateProduction.dotPos);
			if (symbol.is_no_terminal() && symbol.noTerminal == noTerminal)
			{
				StateProduction newProd(stateProduction, stateProduction.dotPos + 1);
				nextState.add_production(newProd);
			}
		}
	}
	closure(nextState, productionSet);
	return nextState;
}
//参考实现
//https://www.bilibili.com/video/BV17W41187gL/?spm_id_from=333.1391.0.0&p=59&vd_source=220188dae404f2d7dbd030327e0acc36
//视频5.3.4只介绍了LR0分析表的构建
void LR0Table::build_LR0_table(const ProductionSet& inProductionSet)
{
	productionSet = inProductionSet;
	reset();

	LR0State initialState({ productionSet.at(0), 0 }); // 假设第一个产生式是 S' -> S$
	closure(initialState, productionSet);

	add_state(initialState);

	std::queue<int> stateQueue;
	stateQueue.push(initialState.id);

	const std::set<Terminal> allTerminals = productionSet.getAllTerminals();
	const std::set<NoTerminal> allNoTerminals = productionSet.getAllNoTerminals();
	while (!stateQueue.empty())
	{
		int front_id = stateQueue.front();
		stateQueue.pop();
		for (const Terminal& terminal : allTerminals)
		{
			LR0State& currentState = get_state(front_id);
			LR0State next_state = GOTO(currentState, terminal, productionSet);
			if (contains_state(next_state))
			{
				auto id = get_state_id(next_state);
				currentState.actionTable[terminal] = id;
			}
			else if (!next_state.empty())
			{//
				currentState.actionTable[terminal] = next_state.id;
				if (!contains_state(next_state))
				{
					add_state(next_state);
					stateQueue.push(next_state.id);
				}
			}
		}
		for (const NoTerminal& noTerminal : allNoTerminals)
		{
			LR0State& currentState = get_state(front_id);
			LR0State next_state = GOTO(currentState, noTerminal, productionSet);
			if (contains_state(next_state))
			{
				auto id = get_state_id(next_state);
				currentState.gotoTable[noTerminal] = id;
			}
			else if (!next_state.empty())
			{
				currentState.gotoTable[noTerminal] = next_state.id;
				if (!contains_state(next_state))
				{
					add_state(next_state);
					stateQueue.push(next_state.id);
				}
			}
		}
	}
}

enum class StackItemType
{
	State,
	Terminal,
	NoTerminal
};
struct ArgoStackItem
{
	int stateId;
	Terminal terminal;
	NoTerminal noTerminal;
	StackItemType type;
	ArgoStackItem(int inStateId) : stateId(inStateId), type(StackItemType::State) {}
	ArgoStackItem(const Terminal& inTerminal) : terminal(inTerminal), type(StackItemType::Terminal) {}
	ArgoStackItem(const NoTerminal& inNoTerminal) : noTerminal(inNoTerminal), type(StackItemType::NoTerminal) {}
};
void print_stack(std::stack<ArgoStackItem> stateStack)
{
	std::vector<string> items;
	while (!stateStack.empty())
	{
		const auto item = stateStack.top();
		stateStack.pop();
		switch (item.type)
		{
		case StackItemType::State:
			items.push_back("State(" + to_string(item.stateId) + ")");
			break;
		case StackItemType::Terminal:
			items.push_back("Terminal(" + item.terminal.getFormatString() + ")");
			break;
		case StackItemType::NoTerminal:
			items.push_back("NoTerminal(" + item.noTerminal.getFormatString() + ")");
			break;
		}
	}
	std::reverse(items.begin(), items.end());
	for (const auto& str : items)
	{
		cout << str << " ";
	}
	cout << endl;
}
//参考：https://www.bilibili.com/video/BV17W41187gL?spm_id_from=333.788.videopod.episodes&vd_source=220188dae404f2d7dbd030327e0acc36&p=58
bool LR0Table::validate_tokens(const std::vector<Terminal>& tokens)
{
	std::stack<ArgoStackItem> stateStack;
	stateStack.push(ArgoStackItem(states[0].id)); // 初始状态ID为0

	int tokenIndex = 0;
	while (true)
	{
		const LR0State& currentState = get_state(stateStack.top().stateId);
		if (currentState.actionTable.empty())
		{//是个规约状态，不需要读取token
			bool reduced = false;
			for (const auto& prod : currentState.productions)
			{
				if (prod.dotPos == prod.right.count())
				{//点在右部末尾，认为是规约
					int popCount = prod.right.count() * 2; // 每个符号对应一个状态和一个符号，所以乘以2
					while (popCount-- > 0)
						stateStack.pop();
					print_stack(stateStack);
					if (prod.left() == productionSet.at(0).left())
					{
						//如果是按照第一个产生式的左部规约的，则认为已完全接受
						cout << "accept" << endl;
						return true;
					}
					const LR0State& topState = get_state(stateStack.top().stateId);
					auto gotoIt = topState.gotoTable.find(prod.left());
					if (gotoIt != topState.gotoTable.end())
					{
						stateStack.push(ArgoStackItem(prod.left()));
						stateStack.push(ArgoStackItem(gotoIt->second));
						reduced = true;
						print_stack(stateStack);
						break;
					}
				}
			}
			if (!reduced)
			{
				cout << "failed" << endl;
				return false; // 无法规约，输入无效
			}
		}
		else
		{//是个读取状态，还需要读取token
			if (tokenIndex >= tokens.size())
			{
				cout << "still need token, but accept no token, failed" << endl;
				return false;
			}
			const auto& token = tokens[tokenIndex++];
			auto nextStateID = currentState.actionTable.find(token);
			if (nextStateID != currentState.actionTable.end())
			{//认为是找到了转移状态
				stateStack.push(ArgoStackItem(token));
				stateStack.push(ArgoStackItem(nextStateID->second));
				print_stack(stateStack);
			}
			else
			{//没有吃掉这个token，不接受
				cout << " meet " << token.getFormatString() << " failed " << endl;
				cout << "actualy need";
				currentState.print_need_id();	
				print_stack(stateStack);
				return false;
			}
		}
	}
	cout << "unkown error" << endl;
	return false;
}
