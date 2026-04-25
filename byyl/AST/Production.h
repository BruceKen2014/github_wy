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
	Production(int index,const NoTerminal& in_no_terminal, const ProductionRight& in_right)
		:index(index), no_terminal(in_no_terminal), right(in_right), is_nullable(false) {
	}
    bool operator==(const Production &other) const
	{
		if (!(no_terminal == other.no_terminal))
			return false;
		return right == other.right;
	}

    virtual void print() const;
	virtual std::string getFormatString() const;
	NoTerminal left() const;

	int right_size() const;

    //将其他产生式的first集合并入当前产生式的first集合
	void mergeFirstSet(const Production& other) const;
public:
	//一个函数指针，返回值是ASTNode*，参数是Production*
	//在该产生式进行规约时，会调用这个函数来生成一个抽象语法树节点，规约后的非终结符可以与这个节点关联起来
	//TODO 把这个节点的生成加入到规约过程中
	//参考视频 https://www.bilibili.com/video/BV17W41187gL?spm_id_from=333.788.videopod.episodes&vd_source=220188dae404f2d7dbd030327e0acc36&p=74
	ASTNode* (*generateASTNodeFunc)(const class MyStack& stack) = nullptr;
public:
	int index = 0;
    NoTerminal no_terminal; // 产生式左部非终结符
	ProductionRight right; // 产生式右部

    //注意，firstSet属于产生式，而不是非终结符。因为同一个非终结符可能有多个产生式，每个产生式的firstSet可能不同。
	mutable TerminalsSet firstSet; // 产生式的 First 集合

	// 记录该产生式的右部是否能推导出空串（在 calculate_all_production_firstSet 中设置）
	bool is_nullable;
};

