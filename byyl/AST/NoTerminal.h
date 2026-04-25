// 非终结符定义
#pragma once

#include <string>

class ASTNode;
class NoTerminal
{
public:
	NoTerminal() = default;
	NoTerminal(const std::string &inName) : name(inName) {}

	bool operator==(const NoTerminal &other) const { return name == other.name; }
	bool operator<(const NoTerminal &other) const { return name < other.name; }

	const std::string& getFormatString() const noexcept { return name; }

	ASTNode* getAstNode() const { return astNode; }
	void setAstNode(ASTNode* node) { astNode = node; }
private:
	//产生式在进行规约的时候，会规约为一个非终结符，规约完成后会生成一个抽象语法树节点，并将该节点与非终结符关联起来
	ASTNode* astNode = nullptr; // 关联的抽象语法树节点指针
	std::string name; // 非终结符名称
};


