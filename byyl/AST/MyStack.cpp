
#include <iostream>

using namespace std;
#include "MyStack.h"

void print_stack(MyStack stateStack)
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