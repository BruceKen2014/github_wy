#pragma once

#include <vector>
#include "StateProduction.h"

using namespace std;

enum class ActionType
{
    Shift,
    Reduce,
    Accept,
    Error
};
struct StateAction
{
    ActionType type;
    vector<int> target; // Shift: 移进终结符，目标状态ID；Reduce: 产生式ID；Accept/Error: 无意义
    //如果target数数组，则表示对于同一个输入符号，可能有多个规约动作（LR(0)分析表中可能存在冲突），需要记录所有的规约动作
    StateAction() = default;
    StateAction(ActionType inType, int inTarget = -1) : type(inType), target({ inTarget }) {}
    StateAction(ActionType inType, const vector<int>& inTarget) : type(inType), target({ inTarget }) {}
};
class LR1State
{
public:
    inline static int ID = 0;
    LR1State() { id = ID++; };
    LR1State(const LR1StateProduction& inStateProduction) :productions{ inStateProduction } {
        id = ID++;
    };
    bool add_production(const LR1StateProduction& production)
    {
        // 如果生产已经存在于状态中，则不添加
        if (std::find(productions.begin(), productions.end(), production) != productions.end())
            return false;
        productions.push_back(production);
        return true;
    }
    bool empty() const { return productions.empty(); }
    bool operator == (const LR1State& other) const
    {
        if (productions.size() != other.productions.size())
            return false;
        return std::equal(productions.begin(), productions.end(), other.productions.begin());
    }
    string getFormatString() const
    {
        string result = "State " + to_string(id) + ":\n";
        for (const auto& prod : productions)
        {
            result += "  " + prod.getFormatString() + "\n";
        }
        return result;
    }
    void print_need_id() const
    {
        string result;
        for (const auto& prod : productions)
        {
            const ProductionRightItem& item = prod.production.right.at(prod.dotPos);
            result += "  " + item.getFormatString();
        }
        std::cout << result << endl;
    }
    void print() const
    {
        std::cout << getFormatString();
    }
    // 判断状态中是否存在可以规约的产生式
    bool can_reduce() const
    {
        for (const auto& prod : productions)
        {
            if (prod.can_reduce())
                return true;
        }
        return false;
    }
    // 获取状态中所有可以规约的状态产生式index
    std::vector<int> get_reduce_production_ids() const
    {
        std::vector<int> reduce_ids;
        for (size_t i = 0; i < productions.size(); ++i)
        {
            if (productions[i].can_reduce())
                reduce_ids.push_back(i);
        }
        return reduce_ids;
    }
    //当处理某个终结符的时候，获取状态中所有可以规约给定终结符的产生式ID
    std::vector<int> get_reduce_production_ids(const Terminal& terminal) const
    {
        std::vector<int> reduce_ids;
        for (size_t i = 0; i < productions.size(); ++i)
        {
            if (productions[i].can_reduce() && productions[i].lookahead.contains(terminal))
                reduce_ids.push_back(i);
        }
        return reduce_ids;
    }
    // 判断状态中是否存在可以移进给定终结符的产生式
    bool can_shift(const Terminal& terminal) const
    {
        for (const auto& prod : productions)
        {
            if (prod.dotPos < prod.production.right.count() && prod.production.right.at(prod.dotPos).is_terminal() && prod.production.right.at(prod.dotPos).terminal == terminal)
                return true;
        }
        return false;
    }


public:
    int id = 0; // 状态ID，可以根据需要设置为唯一值
    std::vector<LR1StateProduction> productions;
    map<Terminal, StateAction> actionTable;
    map<NoTerminal, int> gotoTable;
};