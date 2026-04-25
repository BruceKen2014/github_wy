#include "ProductionSet.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

// 添加产生式
void ProductionSet::add(const Production &p)
{
    if (std::find(productions.begin(), productions.end(), p) != productions.end())
    {
        std::cout << "Warning: Attempted to add duplicate production: " << p.getFormatString() << std::endl;
        return;
    }

    productions.push_back(p);
}

void print_stack(const std::vector<ProductionRightItem>& stack)
{
	for (const auto& item : stack)
    {
        std::cout << item.getFormatString() << " ";
    }
    cout << endl;
}

// 使用 LL(1) 表验证 token 序列是否被文法接受
bool ProductionSet::validate_tokens(const std::vector<Terminal>& tokens, bool verbose)
{
    // 需要 LL1Table 已构建
    if (LL1Table.empty() || tokens.empty())
    {
        if (verbose) std::cout << "LL1 table is empty. Call build_LL1_table() first.\n";
        return false;
    }

    // 假设第一个产生式的左部为开始符号
    if (productions.empty()) return false;
    NoTerminal start = productions.front().left();

    // 使用一个栈模拟分析过程，栈元素可以是终结符或非终结符（用 ProductionRightItem 来承载）
    std::vector<ProductionRightItem> stack;

    // 初始：把开始符号放入栈（作为非终结符）
    stack.push_back(ProductionRightItem(start));

    // 输入指针
    size_t ip = 0;

    // 把输入 tokens 后面加上一个特殊结束符，使用空串表示? 暂不使用特殊终结符，直接当作结束
    while (!stack.empty())
    {
        ProductionRightItem top = stack.back();
        stack.pop_back();

        if (top.is_terminal())
        {
            // 匹配终结符
            if (ip >= tokens.size())
            {
                if (verbose) std::cout << "unexpected end of input, expected " << top.terminal.getFormatString() << "\n";
                return false;
            }
            if (!(tokens[ip] == top.terminal))
            {
                if (verbose) std::cout << "token mismatch: expected " << top.terminal.getFormatString() << " but got " << tokens[ip].getFormatString() << "\n";
                return false;
            }
            ++ip;
            if (ip == tokens.size())
            {
                // 异常情况，输入已完全匹配，但栈内容还不是空，后续栈内容必须能推导出 epsilon（即空串），否则不接受
				// 这里我们假设没有显式 epsilon 产生式，因此栈中剩余元素必须全部是可空非终结符才能接受
				for (int i = 0; i < stack.size(); ++i)
                {
                    if (stack[i].is_terminal())
                    {
                        if (verbose) std::cout << "input fully consumed but stack still has terminal " << stack[i].terminal.getFormatString() << "\n";
                        return false;
                    }
                    NoTerminal nt = stack[i].noTerminal;
                    if (!is_nullable(nt))
                    {
                        if (verbose) std::cout << "input fully consumed but stack still has non-nullable nonterminal " << nt.getFormatString() << "\n";
                        return false;
                    }
                }
                return true;
            }
            print_stack(stack);
        }
        else
        {
            // 非终结符：查表，找到产生式索引列表
            NoTerminal A = top.noTerminal;
            Terminal lookahead = tokens[ip];

            auto itRow = LL1Table.find(A);
            if (itRow == LL1Table.end())
            {
                if (verbose) std::cout << "no row for nonterminal " << A.getFormatString() << "\n";
                return false;
            }
            auto itCell = itRow->second.find(lookahead);
            if (itCell == itRow->second.end())
            {
                // 可能存在 epsilon 产生式映射到 FOLLOW(A) 而 lookahead 为 $, 但我们没有显式 $ 列
                if (verbose) std::cout << "no entry for [" << A.getFormatString() << ", " << lookahead.getFormatString() << "]\n";
                return false;
            }

            // 取第一个产生式（若有冲突，这里选择第一个）。可以根据需求改为尝试所有产生式。
            size_t prodIndex = itCell->second.front();
            const Production &prod = productions[prodIndex];

            // 将产生式右部元素逆序推入栈
            for (auto it = prod.right.items.rbegin(); it != prod.right.items.rend(); ++it)
            {
                // 如果是 epsilon（当前没有显式 epsilon 表示），我们假定空右部对应 items 为空，因此无需处理
                stack.push_back(*it);
            }
            print_stack(stack);
        }
    }

    // 栈空时，输入也应该消耗完
    if (ip == tokens.size()) return true;
    if (verbose) std::cout << "input not fully consumed, remaining=" << (tokens.size() - ip) << "\n";
    return false;
}

// 构建 LL(1) 分析表
void ProductionSet::build_LL1_table()
{
    // 先计算能否推导出空以及每个产生式的 FIRST（生产式级别），再计算非终结符级的 FIRST
    calculate_nullable();
    // 先计算每个产生式的 FIRST 和可空性（p.firstSet, p.is_nullable）以供后续使用
    calculate_all_production_firstSet();
    calculate_followSet();
    // 重新收集 FIRST/FOLLOW 依赖的终结符集合
    // ensure getAllTerminals() will include any terminals added to FOLLOW above
    // (no-op here since FOLLOW updated in-place)

    LL1Table.clear();
    LL1Conflicts.clear();

    for (size_t i = 0; i < productions.size(); ++i)
    {
        const Production &p = productions[i];
        NoTerminal A = p.left();

        // 直接使用该产生式预先计算好的 FIRST 集合，避免重复合并
        TerminalsSet firstAlpha = p.firstSet;

        // 使用在 calculate_all_production_firstSet 中计算好的标志
        bool all_nullable = p.is_nullable;

        // 对 FIRST(alpha) 中的每个终结符填写表项（允许多产生式 => 使用 vector 存索引）
        for (const auto &t : firstAlpha.all())
        {
            auto &row = LL1Table[A];
            auto &vec = row[t];
            // 如果当前产生式索引尚未存在于列表中，则添加
            if (std::find(vec.begin(), vec.end(), i) == vec.end())
            {
                if (!vec.empty())
                {
                    // 发生冲突：已有其它产生式映射到同一表项
                    LL1Conflicts.emplace_back(A, t);
                }
                vec.push_back(i);
            }
        }

        // 如果 alpha 可空，则对 FOLLOW(A) 中的每个终结符填写表项
        if (all_nullable)
        {
            for (const auto &t : FOLLOW[A].all())
            {
                auto &row = LL1Table[A];
                auto &vec = row[t];
                if (std::find(vec.begin(), vec.end(), i) == vec.end())
                {
                    if (!vec.empty())
                    {
                        LL1Conflicts.emplace_back(A, t);
                    }
                    vec.push_back(i);
                }
            }
        }
    }
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
// Access by index is not efficient for std::set; provide linear-time access.
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

//FOLLOW集合定义
//定义 https://www.bilibili.com/video/BV17W41187gL?spm_id_from=333.788.videopod.episodes&vd_source=220188dae404f2d7dbd030327e0acc36&p=53
void ProductionSet::calculate_followSet()
{
    // 计算每个非终结符的 FIRST（FIRST map），后者在计算 FOLLOW 时会被使用
    if (FIRST.empty()) 
	{//如果 FIRST 还未计算过，则先计算 FIRST
		calculate_firstSet();
    }

    FOLLOW.clear();
    bool set_changed = true;
    while (set_changed)
    {
        set_changed = false;
        for (const Production& p : productions)
        {
            auto temp = FOLLOW[p.left()];
            for (int i = p.right_size() - 1; i >= 0; --i)
            {
                const ProductionRightItem& item = p.right.items[i];
                if (item.is_terminal())
                {//终结符
                    temp = TerminalsSet({ item.terminal });
                }
                else
                {//非终结符
                    int old_size = FOLLOW[item.noTerminal].size();
                    FOLLOW[item.noTerminal].merge(temp);
                    set_changed = FOLLOW[item.noTerminal].size() != old_size;
                    if (NULLABLE.contains(item.noTerminal))
                    {
                        temp.merge(FIRST[item.noTerminal]);
                    }
                    else
                    {
                        temp = FIRST[item.noTerminal];
                    }
                }
            }
        }
    }
}

//仅计算非终结符的FIRST集
void ProductionSet::calculate_firstSet()
{
    FIRST.clear();
    
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto& p : productions)
        {
            size_t before = get_firstSet_size(p.left());

            // 遍历右部，从左到右收集 FIRST
            for (const auto& item : p.right.items)
            {
                if (item.getType() == ProductionRightItemType::terminal)
                {
                    // 终结符直接加入 FIRST
                    FIRST[p.left()].add(item.terminal);
                    break;
                }
                FIRST[p.left()].merge(FIRST[item.noTerminal]);

                // 如果该非终结符不可空，则停止向后看
                if (!is_nullable(item.noTerminal))
                {
                    break;
                }
                // 否则继续看下一个符号
            }

            if (get_firstSet_size(p.left()) != before)
                changed = true;
        }
    }
}

void ProductionSet::calculate_all_production_firstSet()
{
    // 清空所有产生式的 FIRST 集合
    for (auto& p : productions)
        p.firstSet.clear();

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto& p : productions)
        {
            size_t before = p.firstSet.size();

            // 遍历右部，从左到右收集 FIRST
            bool scan_all = true;
            for (const auto& item : p.right.items)
            {
                if (item.getType() == ProductionRightItemType::terminal)
                {
                    // 终结符直接加入 FIRST
                    p.firstSet.add(item.terminal);
                    scan_all = false;
                    break;
                }

                // 非终结符：把该非终结符所有产生式的 FIRST 并入
                for (const auto& q : productions)
                {
                    if (q.left() == item.noTerminal)
                    {
                        p.mergeFirstSet(q);
                    }
                }

                // 如果该非终结符不可空，则停止向后看
                if (!is_nullable(item.noTerminal))
                {
                    scan_all = false;
                    break;
                }
                // 否则继续看下一个符号
            }

            //https://www.bilibili.com/video/BV17W41187gL/?spm_id_from=333.1391.0.0&p=54&vd_source=220188dae404f2d7dbd030327e0acc36
            //2分53秒说要这样做
            /*
			* 问了AI，AI给出如下说法，我觉得AI说的有道理，后续可以看看别的教学视频是否也是这么说的：
            * https://chat.deepseek.com/a/chat/s/39bfd079-3da9-4ce3-800a-1431f729b3e2
            */
            //这里不应该将FOLLOW集并入FIRST集，我们后续在构造LL1分析表的时候，可能要用到FOLLOW集，但那是LL1分析表构造的事情，这里不应该混淆定义
            /*
            if (scan_all && FOLLOW.find(p.left()) != FOLLOW.end())
            {
                p.firstSet.merge(FOLLOW[p.left()]);
            }
            */

            // 记录产生式是否可空
            p.is_nullable = scan_all;

            if (p.firstSet.size() != before)
                changed = true;
        }
    }
}
