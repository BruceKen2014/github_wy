

#include "AST.h"
#include "NoTerminal.h"
#include "Terminal.h"
#include "Production.h"

ASTNode* createASTNodeAddForEnd(const MyStack& stack)
{
	ASTNode_Root* node = new ASTNode_Root(); // 根节点的值可以根据需要设置，这里暂时设置为0

	//3E 2状态 1$ 0状态
	const ArgoStackItem& firstItem = stack.r_at(3);
	if (firstItem.type == StackItemType::NoTerminal)
		node->child = firstItem.noTerminal.getAstNode(); //获取第一个非终结符关联的AST节点
	return node;
}

//加法产生式 E -> E + n
ASTNode* createASTNodeAddForProduction(const MyStack& stack)
{
	//第二个必须是终结符+
	const ArgoStackItem& secondItem = stack.r_at(3);
	if (secondItem.type != StackItemType::Terminal || secondItem.terminal.name != "+")
	{
		cout << "createASTNodeAddForProduction: second item is not a terminal '+'." << endl;
		return nullptr;
	}


	//5E 4状态 3+ 2状态 1n 状态0
	const ArgoStackItem& firstItem = stack.r_at(5);
	const ArgoStackItem& thirdItem = stack.r_at(1);

	ASTNode* left = nullptr;
	if (firstItem.type == StackItemType::NoTerminal)
		left = firstItem.noTerminal.getAstNode(); //获取第一个非终结符关联的AST节点
	ASTNode* right = nullptr;
	if (thirdItem.type == StackItemType::NoTerminal)
		right = thirdItem.noTerminal.getAstNode(); //获取第三个非终结符关联的AST节点
	else if(thirdItem.type == StackItemType::Terminal)
		right = new ASTNode_Num(thirdItem.terminal.value); 
	ASTNode* ret = new ASTNode_Add(left, right);

	return ret;
}


ASTNode* createASTNodeNumForProduction(const MyStack& stack)
{
	const ArgoStackItem& firstItem = stack.r_at(1);
	//暂时用0来表示数字节点的值，实际使用时可以根据需要从栈中获取数字的值并转换为整数
	ASTNode* node = new ASTNode_Num(firstItem.terminal.value); //获取第一个终结符关联的AST节点 
	return node;
}

void test_AST()
{
	ASTNode* node = new ASTNode_Add(
		new ASTNode_Num(1),
		new ASTNode_Mul(
			new ASTNode_Num(2),
			new ASTNode_Num(3)
		)
	);
	node->print(); // 输出: (1 + (2 * 3))
	std::cout << std::endl;
	delete node;
}
