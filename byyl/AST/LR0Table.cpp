#include "LR0Table.h"
#include "MyStack.h"
#include "AST.h"
#include <queue>
#include <stack>

using namespace std;

constexpr bool debug_print_closure = false;

void closure(LR1State& state, const ProductionSet& productionSet)
{
	if (debug_print_closure)
	{
		cout << "start closure for state id: " << state.id << endl;
		state.print();
	}
	while (true)
	{
		bool is_change = false;
		for (int i = 0; i < state.productions.size(); ++i)
		{
			const auto& production = state.productions[i];
			if (production.dotPos < production.production.right.count())
			{
				if(debug_print_closure)
				{
					cout << "check production: " << production.getFormatString() << endl;
				}
				auto symbol = production.production.right.at(production.dotPos);
				if (symbol.is_no_terminal())
				{
					// 对于每个非终结符，找到对应的产生式，并将点位置设置为0，加入状态
					for (const auto& prod : productionSet.all())
					{
						if (prod.left() == symbol.noTerminal)
						{
							LR1StateProduction newProd(prod, 0);
							newProd.generate_lookahead(state.productions[i], state.productions[i].dotPos + 1, productionSet);
							if (state.add_production(newProd))
							{
								if (debug_print_closure)
								{
									cout << "add production: " << newProd.getFormatString() << " to state id: " << state.id << endl;
								}
								is_change = true;
							}
						}
					}
				}
			}
		}
		if (!is_change)
			break;
	}
	if (debug_print_closure)
	{
		cout << "end closure for state id: " << state.id << endl;
		state.print();
	}
}

LR1State GOTO(const LR1State& state, const Terminal& terminal, const ProductionSet& productionSet)
{
	LR1State nextState;
	//if(terminal.is_end())
		//任何状态遇见结束符都不转移，直接返回空状态
		//return nextState;
	for (const LR1StateProduction& stateProduction : state.productions)
	{// 如果点后面还有符号，并且这个符号是 terminal 且等于输入的 terminal，那么就把这个产生式的点向右移动一位，加入 nextState
		if (stateProduction.dotPos < stateProduction.production.right.count())
		{
			auto symbol = stateProduction.production.right.at(stateProduction.dotPos);
			if (symbol.is_terminal() && symbol.terminal == terminal)
			{
				LR1StateProduction newProd(stateProduction.production, stateProduction.lookahead, stateProduction.dotPos + 1);
				cout << "GOTO: from state id: " << state.id << " meet terminal: " << terminal.getFormatString() 
						<< ", add production: " << newProd.getFormatString() << " to nextState " << nextState.id << endl;
				nextState.add_production(newProd);
			}
		}
	}
	closure(nextState, productionSet);
	return nextState;
}
LR1State GOTO(const LR1State& state, const NoTerminal& noTerminal, const ProductionSet& productionSet)
{
	LR1State nextState;
	for (const LR1StateProduction& stateProduction : state.productions)
	{// 如果点后面还有符号，并且这个符号是 noTerminal 且等于输入的 noTerminal，那么就把这个产生式的点向右移动一位，加入 nextState
		if (stateProduction.dotPos < stateProduction.production.right.count())
		{
			auto symbol = stateProduction.production.right.at(stateProduction.dotPos);
			if (symbol.is_no_terminal() && symbol.noTerminal == noTerminal)
			{
				LR1StateProduction newProd(stateProduction.production, stateProduction.lookahead, stateProduction.dotPos + 1);
				nextState.add_production(newProd);
			}
		}
	}
	closure(nextState, productionSet);
	return nextState;
}
void LR0Table::build_LR1_table(const ProductionSet& inProductionSet)
{
	/*
	LR1与LR0的区别在于，在LR0的基础上，LR1在构建action表时，对于规约动作，还需要判断当前输入的terminal是否在状态产生式的前看符号里，如果在里面，那么才认为这个terminal可以被规约掉，否则就认为这个terminal是一个错误输入。
	这意味着我们在构建ACTION表的时候，只将属于状态产生式的前看符号集合的终结符加入到规约动作中，而不是将所有终结符都加入到规约动作中，从而减少了LR0分析表中的冲突，使得更多的文法能够被LR1分析器接受。
	*/
	productionSet = inProductionSet;
	reset();

	LR1State initialState({ productionSet.at(0), 0 }); // 假设第一个产生式是 S' -> S$
	initialState.productions[0].lookahead.add(Terminal::END); // S' -> S$ 这个产生式的前看符号是结束符

	closure(initialState, productionSet); //计算产生式闭包，加入状态集合

	add_state(initialState);

	std::queue<int> stateQueue;
	stateQueue.push(initialState.id);

	const std::set<Terminal> allTerminals = productionSet.getAllTerminals();
	const std::set<NoTerminal> allNoTerminals = productionSet.getAllNoTerminals();
	while (!stateQueue.empty())
	{
		int current_id = stateQueue.front();
		stateQueue.pop();
		for (const Terminal& terminal : allTerminals)
		{
			LR1State& currentState = get_state(current_id);

			std::vector<int> reduce_production_ids = currentState.get_reduce_production_ids();
			//计算可以真正规约的产生式ID
			//只有在lookahead集合里的terminal时，才认为这个terminal是可以被规约掉的
			std::vector<int> target_reduce_ids;
			for (int id : reduce_production_ids)
			{
				const auto& prod = currentState.productions[id];
				if (prod.lookahead.contains(terminal))
					target_reduce_ids.push_back(prod.production.index);
			}


			bool can_shift = currentState.can_shift(terminal);

			if (target_reduce_ids.size() > 0)
			{//可以真正规约
				if (can_shift)
				{
					//如果既可以真正规约又可以移进，那么就认为发生了冲突，记录冲突信息
					cout << "conflict happens, state id: " << currentState.id << " meet terminal: " << terminal.getFormatString() << endl;
				}
				else
				{
					//能规约，但是不能移进，那么就认为这个terminal是可以被规约掉的，
					currentState.actionTable[terminal] = { ActionType::Reduce, target_reduce_ids };
				}
			}
			else
			{//不能真正规约，那么就需要判断是否能移进，如果能移进，那么就认为是一个有效的转移状态，如果不能移进，那么就认为这个terminal是一个错误输入，记录错误状态
				if (can_shift)
				{
					//如果能移进，那么就认为是一个有效的转移状态
					//说明当前状态可以吃掉这个terminal，从而转移到下一个状态
					LR1State next_state = GOTO(currentState, terminal, productionSet);
					if (contains_state(next_state))
					{//如果已经包含这个状态了，那么就直接在action表里记录这个转移状态的id
						auto id = get_state_id(next_state);
						currentState.actionTable[terminal] = { ActionType::Shift,id };
					}
					else
					{//如果不包含这个状态，那么就把这个状态加入状态集合，并且在action表里记录这个转移状态的id
						currentState.actionTable[terminal] = { ActionType::Shift,next_state.id };
						add_state(next_state);
						stateQueue.push(next_state.id);
					}
				}
				else
				{
					//如果不能移进，也不能规约，那么就认为这个terminal是一个错误输入，记录错误状态
					currentState.actionTable[terminal] = { ActionType::Error };
				}
			}
		}
		for (const NoTerminal& noTerminal : allNoTerminals)
		{
			LR1State& currentState = get_state(current_id);
			LR1State next_state = GOTO(currentState, noTerminal, productionSet);
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

constexpr bool debug_print_astnode = false;
//参考：https://www.bilibili.com/video/BV17W41187gL?spm_id_from=333.788.videopod.episodes&vd_source=220188dae404f2d7dbd030327e0acc36&p=58
ASTNode* LR0Table::validate_tokens(const std::vector<Terminal>& tokens, const ProductionSet& inProductionSet)
{
	MyStack stateStack;
	stateStack.push(ArgoStackItem(states[0].id)); // 初始状态ID为0

	int tokenIndex = 0;
	Terminal last_token;
	while (true)
	{
		const LR1State& currentState = get_state(stateStack.top().stateId);
		if (last_token.is_end())
		{//尝试规约 
			auto actionData = currentState.actionTable.find(last_token);

			if (actionData != currentState.actionTable.end() && actionData->second.type == ActionType::Reduce)
			{//是个规约状态
				const auto& reduce_production_index = actionData->second.target;
				const auto& prod = inProductionSet.productions[reduce_production_index[0]];
				cout << "meet " << last_token.getFormatString() << " reduce by production index: " << prod.index << endl;

				NoTerminal noTerminal = prod.left();
				ASTNode* node = prod.generateASTNodeFunc(stateStack);
				if(debug_print_astnode)
					node->print();

				noTerminal.setAstNode(node);


				int popCount = prod.right.count() * 2; // 每个符号对应一个状态和一个符号，所以乘以2
				while (popCount-- > 0)
					stateStack.pop();
				print_stack(stateStack);
				if (stateStack.size() == 1)
				{
					cout << "only left start state, accept !" << endl;
					return node;
				}
				const LR1State& topState = get_state(stateStack.top().stateId);
				auto gotoIt = topState.gotoTable.find(prod.left());
				if (gotoIt != topState.gotoTable.end())
				{
					stateStack.push(ArgoStackItem(prod.left()));
					stateStack.push(ArgoStackItem(gotoIt->second));
					print_stack(stateStack);
				}
			}
			else
			{//无法规约，说明token结束了，但还没规约完成，判断为失败
				cout << " already meet end $ , but can not reduce" << endl;
				return nullptr;
			}
		}

		//读取token
		if (tokenIndex >= tokens.size())
		{
			cout << "still need token, but accept no token, failed" << endl;
			break;
		}
		const auto& token = tokens[tokenIndex];
		auto actionData = currentState.actionTable.find(token);
		if (actionData != currentState.actionTable.end() && actionData->second.type == ActionType::Shift)
		{//认为是找到了转移状态
			last_token = token;
			stateStack.push(ArgoStackItem(token));
			stateStack.push(ArgoStackItem(actionData->second.target[0]));
			print_stack(stateStack);
			tokenIndex++;
		}
		else if (actionData != currentState.actionTable.end() && actionData->second.type == ActionType::Reduce)
		{//是个规约状态
			const auto& reduce_production_index = actionData->second.target;
			const auto& prod = inProductionSet.productions[reduce_production_index[0]];
			cout << "meet " << token.getFormatString() << " reduce by production index: " << reduce_production_index[0] << endl;
			NoTerminal noTerminal = prod.left();
			ASTNode* node = prod.generateASTNodeFunc(stateStack);
			if (debug_print_astnode)
				node->print();

			noTerminal.setAstNode(node);

			int popCount = prod.right.count() * 2; // 每个符号对应一个状态和一个符号，所以乘以2
			while (popCount-- > 0)
				stateStack.pop();
			print_stack(stateStack);
			const LR1State& topState = get_state(stateStack.top().stateId);
			auto gotoIt = topState.gotoTable.find(prod.left());
			if (gotoIt != topState.gotoTable.end())
			{
				stateStack.push(ArgoStackItem(noTerminal));
				stateStack.push(ArgoStackItem(gotoIt->second));
				print_stack(stateStack);
			}
		}
		else
		{//没有吃掉这个token，不接受
			cout << " meet " << token.getFormatString() << " failed " << endl;
			cout << "actualy need";
			currentState.print_need_id();
			return nullptr;
		}

	}
	if (tokenIndex == tokens.size())
	{//尝试规约 
	}
	cout << "unkown error" << endl;
	return nullptr;
}
