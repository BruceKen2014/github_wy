#pragma once

#include <iostream>
#include <string>
#include "MyStack.h"

using namespace std;
/*
* AST(Abstract Syntax Tree)抽象语法树
* 针对加法乘法的抽象语法树节点
* E -> E * E
* E -> E + E
* E -> n
*/

enum class ASTNodeType {
	Root,
	Add,
	Mul,
	Num
};

class ASTNode 
{
public:
	ASTNode() = default;
	ASTNode(ASTNodeType t) : type(t) {}
	virtual ~ASTNode() = default;
	virtual int getNumNodes() const = 0; // 纯虚函数，要求派生类实现获取节点数量的功能
	virtual int getBytes() const = 0; // 纯虚函数，要求派生类实现获取节点占用字节数的功能
	virtual void print() const = 0; // 纯虚函数，要求派生类实现打印功能
public:
	ASTNodeType type;
};

class ASTNode_Root : public ASTNode
{
public:
	int value; // 仅当 type == Num 时有效
	ASTNode_Root() : ASTNode(ASTNodeType::Root){}
	void print() const override {
		std::cout << "(";
		child->print();
		std::cout << ")";
	}
	virtual int getNumNodes() const override
	{
		return 1 + (child ? child->getNumNodes() : 0); // 当前节点加上子树的节点数量
	}
	virtual int getBytes() const override
	{
		return sizeof(ASTNode_Root) + (child ? child->getBytes() : 0); // 当前节点占用的字节数加上子树占用的字节数	
	}

	ASTNode* child = nullptr;
};


class ASTNode_Num : public ASTNode
{
public:
	int value; // 仅当 type == Num 时有效
	ASTNode_Num(int inValue) : ASTNode(ASTNodeType::Num), value(inValue) {}
	ASTNode_Num(const std::string& str) : ASTNode(ASTNodeType::Num), value(std::stoi(str)) {}
	void print() const override {
		std::cout << value;
	}
	virtual int getNumNodes() const override {
		return 1; // 数字节点本身就是一个节点
	}
	virtual int getBytes() const override {
		return sizeof(ASTNode_Num); // 返回数字节点占用的字节数
	}
};

class ASTNode_Mul : public ASTNode{
public:
	ASTNode* left = nullptr;
	ASTNode* right = nullptr;
	ASTNode_Mul(ASTNode* left, ASTNode* right) : ASTNode(ASTNodeType::Mul), left(left), right(right) {}
	~ASTNode_Mul() {
		delete left;
		delete right;
	}
	void print() const override {
		std::cout << "(";
		left->print();
		std::cout << " * ";
		right->print();
		std::cout << ")";
	}
	virtual int getNumNodes() const override 
	{
		return 1 + (left ? left->getNumNodes() : 0) + (right ? right->getNumNodes() : 0); // 当前节点加上左右子树的节点数量
	}
	virtual int getBytes() const override
	{
		return sizeof(ASTNode_Mul) + (left ? left->getBytes() : 0) + (right ? right->getBytes() : 0); // 当前节点占用的字节数加上左右子树占用的字节数
	}
	
};

class ASTNode_Add : public ASTNode
{
public:
	ASTNode* left = nullptr;
	ASTNode* right = nullptr;
	ASTNode_Add(ASTNode* left, ASTNode* right) : ASTNode(ASTNodeType::Add), left(left), right(right) {}
	~ASTNode_Add() {
		delete left;
		delete right;
	}
	void print() const override {
		std::cout << "(";
		left->print();
		std::cout << " + ";
		right->print();
		std::cout << ")";
	}
	virtual int getNumNodes() const override
	{
		return 1 + (left ? left->getNumNodes() : 0) + (right ? right->getNumNodes() : 0); // 当前节点加上左右子树的节点数量
	}
	virtual int getBytes() const override
	{
		return sizeof(ASTNode_Add) + (left ? left->getBytes() : 0) + (right ? right->getBytes() : 0); // 当前节点占用的字节数加上左右子树占用的字节数
	}
};

//for S -> E$
extern ASTNode* createASTNodeAddForEnd(const MyStack& stack);

//for E -> E + n
extern ASTNode* createASTNodeAddForProduction(const MyStack& stack);

//for E -> n
extern ASTNode* createASTNodeNumForProduction(const MyStack& stack);
extern void test_AST();