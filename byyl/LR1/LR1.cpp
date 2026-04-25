// LR0.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "Production.h"
#include "ProductionSet.h"
#include "LR0Table.h"
using T = Terminal;
using NT = NoTerminal;
int main()
{
	cout << "Hello World SLR!\n";
	Production p1(NoTerminal("S'"), ProductionRight({ NoTerminal("S"), Terminal("$") }));
	Production p2(NoTerminal("S"), ProductionRight({ Terminal("x"), Terminal("x"), NoTerminal("T") }));
	Production p3(NoTerminal("T"), ProductionRight({ Terminal("y") }));


	ProductionSet ps;
	ps.add(p1);
	ps.add(p2);
	ps.add(p3);

	//计算FOLLOW集合必须在计算LR0表之前，因为LR0表的构建可能需要使用FOLLOW集合来确定规约动作的合法性。
	ps.calculate_followSet();
	ps.FOLLOW[p1.left()].add(T("$")); // 将 $ 加入 S' 的 FOLLOW 集合，表示输入结束时可以规约 S' -> S$
	LR0Table lr0Table;
	lr0Table.build_LR1_table(ps);
	lr0Table.print();

	vector<Terminal> tokens{ Terminal("x"), Terminal("x"), Terminal("x"), Terminal("x"), Terminal("x"), Terminal("y") ,Terminal("$") };
	lr0Table.validate_tokens(tokens);
	vector<Terminal> tokens2{ Terminal("x"), Terminal("x"), Terminal("y") ,Terminal("$") };
	lr0Table.validate_tokens(tokens2);
	return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件


