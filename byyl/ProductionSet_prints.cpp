#include "ProductionSet.h"
#include <iostream>
#include <iomanip>
#include <sstream>

void ProductionSet::print() const
{
    int i = 0;
    for (const auto& p : productions)
    {
        cout << i++ << ":" << p.getFormatString() << endl;
    }
}

void ProductionSet::print_all_noTerminals() const
{
    std::set<NoTerminal> noTerminals;
    for (const auto& p : productions)
        noTerminals.insert(p.left());
    std::cout << "Non-terminals: { ";
    for (const auto& nt : noTerminals)
        std::cout << nt.getFormatString() << " ";
    std::cout << "}" << std::endl;
}

void ProductionSet::print_all_noTerminals_firstSet() const
{
    for (const auto& [key, value] : FIRST)
    {
        std::cout << "FIRST(" << key.getFormatString() << ") = " << value.getFormatString() << std::endl;
    }
}

void ProductionSet::print_all_production_firstSet() const
{
    for (const auto& p : productions)
    {
        std::cout << "production " << p.getFormatString() << " FIRST(" << p.firstSet.getFormatString() << ")" << std::endl;
    }
}

void ProductionSet::print_all_terminals() const
{
    std::set<Terminal> terminals;
    for (const auto& p : productions)
    {
        for (const auto& item : p.right.items)
        {
            if (item.getType() == ProductionRightItemType::terminal)
                terminals.insert(item.terminal);
        }
    }
    std::cout << "Terminals: { ";
    for (const auto& t : terminals)
        std::cout << t.getFormatString() << " ";
    std::cout << "}" << std::endl;
}

void ProductionSet::print_nullable() const
{
    std::cout << "NULLABLE: { ";
    for (const auto& nt : NULLABLE)
        std::cout << nt.getFormatString() << " ";
    std::cout << "}" << std::endl;
}

void ProductionSet::print_all_followSet() const
{
    for (const auto& [key, value] : FOLLOW)
    {
       std::cout << "FOLLOW(" << key.getFormatString() << ") = " << value.getFormatString() << std::endl;
	}
}

//完全由AI生成，未经人工修改
void ProductionSet::print_LL1_table() const
{
    // 打印为二维表：行 = 非终结符，列 = 终结符，单元格为产生式索引列表
    // 收集所有列（终结符）和行（非终结符）
    std::set<Terminal> terminals;
    std::set<NoTerminal> nonTerminals;
    // 基于 LL1Table 收集已有列
    for (const auto &rowPair : LL1Table)
    {
        nonTerminals.insert(rowPair.first);
        for (const auto &colPair : rowPair.second)
            terminals.insert(colPair.first);
    }
    // 还要确保包含文法中所有终结符（即使某些行没有映射），以便表格列完整
    for (const auto &t : getAllTerminals())
        terminals.insert(t);
    // 确保包含所有非终结符（即使某些非终结符行为空）
    for (const auto &p : productions)
        nonTerminals.insert(p.left());
    // 不自动加入结束符 '$' 列，表格只显示文法中声明的终结符

    // 准备有序向量
    std::vector<NoTerminal> rows(nonTerminals.begin(), nonTerminals.end());
    std::vector<Terminal> cols(terminals.begin(), terminals.end());

    // 计算每列宽度
    std::vector<size_t> colWidth(cols.size());
    size_t firstColWidth = 0;
    for (const auto &r : rows)
        firstColWidth = std::max(firstColWidth, r.getFormatString().size());
    firstColWidth = std::max(firstColWidth, std::string("Non-Terminal").size());

    for (size_t j = 0; j < cols.size(); ++j)
    {
        colWidth[j] = cols[j].getFormatString().size();
    }

    // cell string lengths may be larger
    for (size_t i = 0; i < rows.size(); ++i)
    {
        const NoTerminal &A = rows[i];
        auto itRow = LL1Table.find(A);
        if (itRow == LL1Table.end()) continue;
        for (size_t j = 0; j < cols.size(); ++j)
        {
            const Terminal &t = cols[j];
            auto itCell = itRow->second.find(t);
            if (itCell != itRow->second.end())
            {
                // make comma separated indices string
                std::ostringstream oss;
                for (size_t k = 0; k < itCell->second.size(); ++k)
                {
                    if (k) oss << ",";
                    oss << itCell->second[k];
                }
                colWidth[j] = std::max(colWidth[j], oss.str().size());
            }
        }
    }

    // 打印表头
    std::cout << std::setw(static_cast<int>(firstColWidth) + 2) << " ";
    for (size_t j = 0; j < cols.size(); ++j)
    {
        std::cout << std::setw(static_cast<int>(colWidth[j]) + 2) << cols[j].getFormatString();
    }
    std::cout << std::endl;

    // 打印分隔线
    std::cout << std::string(firstColWidth + 2, '-') ;
    for (size_t j = 0; j < cols.size(); ++j)
        std::cout << std::string(colWidth[j] + 2, '-');
    std::cout << std::endl;

    // 打印每一行
    for (const auto &A : rows)
    {
        std::cout << std::setw(static_cast<int>(firstColWidth) + 2) << A.getFormatString();
        auto itRow = LL1Table.find(A);
        for (size_t j = 0; j < cols.size(); ++j)
        {
            std::string cell;
            if (itRow != LL1Table.end())
            {
                auto itCell = itRow->second.find(cols[j]);
                if (itCell != itRow->second.end())
                {
                    std::ostringstream oss;
                    for (size_t k = 0; k < itCell->second.size(); ++k)
                    {
                        if (k) oss << ",";
                        oss << itCell->second[k];
                    }
                    cell = oss.str();
                }
            }
            std::cout << std::setw(static_cast<int>(colWidth[j]) + 2) << cell;
        }
        std::cout << std::endl;
    }
}

void ProductionSet::print_LL1_conflicts() const
{
    if (LL1Conflicts.empty())
    {
        std::cout << "No LL(1) conflicts." << std::endl;
        return;
    }
    std::cout << "LL(1) Conflicts:\n";
    for (const auto & [A, t] : LL1Conflicts)
    {
        const auto &vec = LL1Table.at(A).at(t);
        std::cout << "Conflict at [" << A.getFormatString() << ", " << t.getFormatString() << "]: ";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i) std::cout << ", ";
            std::cout << vec[i];
        }
        std::cout << std::endl;
    }
}
