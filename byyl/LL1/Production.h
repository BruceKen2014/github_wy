#pragma once
// 产生式定义

#include <vector>
#include <iostream>
#include "NoTerminal.h"
#include "ProductionRight.h"
#include "TerminalsSet.h"
using namespace std;

class Production
{
public:
	Production(const NoTerminal &in_no_terminal, const ProductionRight& in_right)
        :no_terminal(in_no_terminal), right(in_right), is_nullable(false) {}
    bool operator==(const Production &other) const
	{
		if (!(no_terminal == other.no_terminal))
			return false;
		return right == other.right;
	}

    void print() const;
	std::string getFormatString() const;
	NoTerminal left() const;

	int right_size() const;

    //将其他产生式的first集合并入当前产生式的first集合
	void mergeFirstSet(const Production& other) const;
public:
    NoTerminal no_terminal; // 产生式左部非终结符
	ProductionRight right; // 产生式右部

    //注意，firstSet属于产生式，而不是非终结符。因为同一个非终结符可能有多个产生式，每个产生式的firstSet可能不同。
	mutable TerminalsSet firstSet; // 产生式的 First 集合

	// 记录该产生式的右部是否能推导出空串（在 calculate_all_production_firstSet 中设置）
	bool is_nullable;
};

