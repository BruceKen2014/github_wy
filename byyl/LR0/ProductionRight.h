#pragma once

#include <vector>
#include <string>
#include "Terminal.h"
#include "NoTerminal.h"
#include <algorithm>


//产生式的右部元素可以是一个终结符或者一个非终结符
enum class ProductionRightItemType
{
	terminal,
	noTerminal,
};
class ProductionRightItem
{
public:
  ProductionRightItem(const Terminal& inTerminal) :terminal(inTerminal), type(ProductionRightItemType::terminal) {};
	ProductionRightItem(const NoTerminal& inNoTerminal) :noTerminal(inNoTerminal), type(ProductionRightItemType::noTerminal) {};

	ProductionRightItemType getType() const noexcept { return type; }
	bool is_terminal() const noexcept { return type == ProductionRightItemType::terminal; }
	bool is_no_terminal() const noexcept { return type == ProductionRightItemType::noTerminal; }
	bool operator==(const ProductionRightItem& other) const;
 // keep operator< declaration if needed elsewhere
 bool operator<(const ProductionRightItem& other) const;
	const std::string& getFormatString() const;

public:
	Terminal terminal;
	NoTerminal noTerminal;
	ProductionRightItemType type;
};

// 产生式右部，由多个 ProductionRightItem 组成
class ProductionRight
{
public:
	ProductionRight() = default;
    ProductionRight(std::vector<ProductionRightItem> in_items) :items(in_items) {};
    bool operator==(const ProductionRight& other) const;
	bool operator<(const ProductionRight& other) const
	{
		if (items.size() != other.items.size()) return items.size() < other.items.size();
		return std::lexicographical_compare(items.begin(), items.end(), other.items.begin(), other.items.end());
	}
	//右部的item数量
	int count()const { return items.size(); }
    const std::string getFormatString() const;

public:
	//for iterate
	auto begin(){ return items.begin();}
	auto end()  { return items.end();}

	//for iterate
	auto cbegin() const { return items.cbegin(); }
	auto cend() const { return items.cend(); }
public:
	std::vector<ProductionRightItem> items;
};