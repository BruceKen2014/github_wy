
#pragma once

#include <vector>
#include <set>
#include <map>
#include <string>
#include "Production.h"

// 管理产生式集合
class ProductionSet
{
public:
    ProductionSet() = default;

    // 添加产生式
    void add(const Production &p);

    // 返回产生式数量
    size_t size() const noexcept;

    // 通过索引访问（带范围检查）
    Production &at(size_t idx);
    const Production &at(size_t idx) const;

    // 获取所有产生式的只读引用
    const std::vector<Production> &all() const noexcept;

    // 根据索引删除产生式（可能抛出 out_of_range）
    void removeAt(size_t idx);

    // 清空集合
    void clear() noexcept;

    //打印所有产生式
    void print() const;

    //判断非终结符是否为NULLABLE集合
    bool is_nullable(const NoTerminal& terminal) const  {    return NULLABLE.contains(terminal);}

    //计算NULLABLE集合
    void calculate_nullable();

    //https://www.bilibili.com/video/BV17W41187gL?spm_id_from=333.788.videopod.episodes&vd_source=220188dae404f2d7dbd030327e0acc36&p=52
	//仅计算非终结符的FIRST集
	void calculate_firstSet();

    //计算非终结符的FOLLOW集合
    void calculate_followSet();

	//计算所有产生式的FIRST集合
    void calculate_all_production_firstSet();

	//获取非终结符的FIRST集合大小
    int get_firstSet_size(const NoTerminal& noTerminal) const
    {
        auto it = FIRST.find(noTerminal);
        if (it != FIRST.end())
            return it->second.size();
		return 0;
    }



    void print_all_noTerminals() const;

	//打印所有非终结符的FIRST集合
    void print_all_noTerminals_firstSet() const;

	//打印所有产生式的FIRST集合
	void print_all_production_firstSet() const;
    void print_all_terminals() const;
    void print_nullable() const;
	void print_all_followSet() const;

    // 打印 LL(1) 分析表及冲突信息
    void print_LL1_table() const;
    void print_LL1_conflicts() const;

    // 构建 LL(1) 分析表（会计算 FIRST 和 FOLLOW），允许冲突并记录到 LL1Conflicts
    void build_LL1_table();

    // 使用已构建的 LL(1) 分析表判断给定的终结符序列能否被文法接受
    // tokens: 输入的终结符序列（不需要包含结束符），返回 true 表示接受
    bool validate_tokens(const std::vector<Terminal>& tokens, bool verbose = false);

    // 获取 LL1 表的只读引用
    const std::map<NoTerminal, std::map<Terminal, std::vector<size_t>>>& getLL1Table() const { return LL1Table; }

    // 获取构建过程中发现的冲突列表
    const std::vector<std::pair<NoTerminal, Terminal>>& getLL1Conflicts() const { return LL1Conflicts; }

	//获取所有终结符集合
	std::set<Terminal> getAllTerminals() const;
	//获取所有非终结符集合
	std::set<NoTerminal> getAllNoTerminals() const;

	// 判断给定的终结符是否在指定非终结符的 FOLLOW 集合中
    bool is_in_follow_set(const NoTerminal& noTerminal, const Terminal& terminal) const
    {
        auto it = FOLLOW.find(noTerminal);
        if (it != FOLLOW.end())
            return it->second.contains(terminal);
        return false;
	}

public:
    std::vector<Production> productions;

    //可能推导为空的非终结符集合
    std::set<NoTerminal>  NULLABLE;

	//每个非终结符的FOLLOW集合
    std::map<NoTerminal, TerminalsSet> FOLLOW;
	//每个非终结符的FIRST集合
	std::map<NoTerminal, TerminalsSet> FIRST;

    // LL(1) 分析表: 对于每个非终结符和终结符，记录可能的产生式索引集合（允许冲突）
    // 表类型：LL1Table[非终结符][终结符] = {产生式索引...}
    std::map<NoTerminal, std::map<Terminal, std::vector<size_t>>> LL1Table;

    // 在构建过程中记录发生冲突的表格坐标（非终结符, 终结符）
    std::vector<std::pair<NoTerminal, Terminal>> LL1Conflicts;
};


