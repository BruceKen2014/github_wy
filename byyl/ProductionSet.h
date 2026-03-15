
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

	//计算FIRST集，注意FIRST集属于产生式，而不是非终结符。因为同一个非终结符可能有多个产生式，每个产生式的FIRST集可能不同。
	//同时，这里也会保存一份每个非终结符的FIRST集合。
	void calculate_firstSet();

	//计算所有产生式的FOLLOW集合
	//void calculate_followSet();

    void print_all_noTerminals() const;

    void print_all_firstSet() const;
    void print_all_terminals() const;
    void print_nullable() const;

	//获取所有终结符集合
	std::set<Terminal> getAllTerminals() const;
	//获取所有非终结符集合
	std::set<NoTerminal> getAllNoTerminals() const;
private:
    std::vector<Production> productions;

    //可能推导为空的非终结符集合
    std::set<NoTerminal>  NULLABLE;

	//每个非终结符的FOLLOW集合
    std::map<NoTerminal, TerminalsSet> FOLLOW;
	//每个非终结符的FIRST集合
	std::map<NoTerminal, TerminalsSet> FIRST;
};


