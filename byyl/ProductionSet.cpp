#include "ProductionSet.h"
#include <algorithm>
#include <stdexcept>

// 添加产生式
void ProductionSet::add(const Production &p)
{
    if (std::find(productions.begin(), productions.end(), p) != productions.end())
    {
        std::cout << "Warning: Attempted to add duplicate production: " << p.getFormatString() << std::endl;
    }
    productions.push_back(p);
}

std::set<Terminal> ProductionSet::getAllTerminals() const
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
    return terminals;
}

std::set<NoTerminal> ProductionSet::getAllNoTerminals() const
{
    std::set<NoTerminal> noTerminals;
    for (const auto& p : productions)
        noTerminals.insert(p.left());
    return noTerminals;
}

// 返回产生式数量
size_t ProductionSet::size() const noexcept { return productions.size(); }

// 通过索引访问（带范围检查）
Production &ProductionSet::at(size_t idx)
{
    if (idx >= productions.size())
        throw std::out_of_range("ProductionSet::at index out of range");
    return productions.at(idx);
}

const Production &ProductionSet::at(size_t idx) const
{
    if (idx >= productions.size())
        throw std::out_of_range("ProductionSet::at index out of range");
    return productions.at(idx);
}

// 获取所有产生式的只读引用
const std::vector<Production> &ProductionSet::all() const noexcept { return productions; }

// 根据索引删除产生式（可能抛出 out_of_range）
void ProductionSet::removeAt(size_t idx)
{
    if (idx >= productions.size())
        throw std::out_of_range("ProductionSet::removeAt index out of range");
    productions.erase(productions.begin() + static_cast<std::ptrdiff_t>(idx));
}

// 清空集合
void ProductionSet::clear() noexcept { productions.clear(); }

//NULLABLE集合定义
//定义 https://www.bilibili.com/video/BV17W41187gL?spm_id_from=333.788.videopod.episodes&vd_source=220188dae404f2d7dbd030327e0acc36&p=50
//9分32秒
void ProductionSet::calculate_nullable()
{
    NULLABLE.clear();
    bool nullable_is_changing = true;

    while (nullable_is_changing)
    {
        auto size = NULLABLE.size();
        for (const Production& p : productions)
        {
            bool add = true;
            for (const ProductionRightItem &rightitem : p.right.items)
            {
                if (rightitem.getType() == ProductionRightItemType::terminal)
                {
                    //遇到终结符，说明这个产生式不可能推导出空串，直接跳过
					add = false;
                    break;
                }
				//遇到非终结符，如果这个非终结符不在NULLABLE集合中，说明这个产生式不可能推导出空串，直接跳过
                if (not is_nullable(rightitem.noTerminal))
                {
                    add = false;
                    break;
                }
            }
            if(add)
                NULLABLE.insert(p.left());
        }
		nullable_is_changing = NULLABLE.size() != size;
    }
}

void ProductionSet::calculate_firstSet()
{
    // 清空所有产生式的 FIRST 集合
    for (auto &p : productions)
        p.firstSet.clear();

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto &p : productions)
        {
            size_t before = p.firstSet.size();

            // 遍历右部，从左到右收集 FIRST
            for (const auto &item : p.right.items)
            {
                if (item.getType() == ProductionRightItemType::terminal)
                {
                    // 终结符直接加入 FIRST
                    p.firstSet.add(item.terminal);
                    break;
                }

                // 非终结符：把该非终结符所有产生式的 FIRST 并入
                for (const auto &q : productions)
                {
                    if (q.left() == item.noTerminal)
                    {
						p.mergeFirstSet(q);
                    }
                }

                // 如果该非终结符不可空，则停止向后看
                if (!is_nullable(item.noTerminal))
                    break;
                // 否则继续看下一个符号
            }

            if (p.firstSet.size() != before)
                changed = true;
        }
    }

    FIRST.clear();
    
    for (const auto& p : productions)
    {
		auto ite = FIRST.find(p.left());    
        if (ite != FIRST.end())
        {
			ite->second.merge(p.firstSet);
        }
        else
        {
            FIRST[p.left()] = p.firstSet;
        }
    }
}
