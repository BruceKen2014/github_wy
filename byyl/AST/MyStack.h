#pragma once

#include <vector>
#include <string.h>
#include "Terminal.h"
#include "NoTerminal.h"

using namespace std;

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
class MyStack
{
public:
	void push(const ArgoStackItem& item)
	{
		items.push_back(item);
	}
	void pop()
	{
		if (!items.empty())
			items.pop_back();
	}
	const ArgoStackItem& top() const
	{
		if (!items.empty())
			return items.back();
		throw std::runtime_error("stack is empty");
	}
	const ArgoStackItem& r_at(int index) const
	{
		if (index >= 0 && index < items.size())
			return items[items.size() - 1 - index];
		throw std::out_of_range("index out of range");
	}
	bool empty() const
	{
		return items.empty();
	}
	int size() const
	{
		return items.size();
	}
	const ArgoStackItem& at(int index) const
	{
		if (index >= 0 && index < items.size())
			return items[index];
		throw std::out_of_range("index out of range");
	}
public:

	std::vector<ArgoStackItem> items;
};

extern void print_stack(MyStack stateStack);