#pragma once


#include "Production.h"
#include "ProductionSet.h"
using namespace std;

class StateProduction
{
public:
    StateProduction(const Production& inProduction, int inDotPos = 0) : production(inProduction), dotPos(inDotPos) {
        str = getFormatString();
    };

    virtual string getFormatString() const
    {
        std::string result = production.no_terminal.getFormatString() + " -> ";
        for (int i = 0; i < production.right.count(); ++i)
        {
            if (i == dotPos)
                result += ".";
            result += production.right.at(i).getFormatString() + " ";
        }
        if (dotPos == production.right.count())
            result += ".";
        return result;
    }
    virtual void print() const
    {
        std::cout << getFormatString();
    }
    bool operator == (const StateProduction& other)const
    {
        if (dotPos != other.dotPos)
            return false;
        return production == other.production;
    }
    bool can_reduce() const
    {
        return dotPos == production.right.count();
    }
public:
    const Production& production; // 该状态产生式对应的原始产生式
    int dotPos; // 点的位置，范围是0到right.count()，表示点在右部的哪个位置
    string str;
};


// LR(1)状态中的产生式，包含一个lookahead集合，表示在这个状态下，如果输入lookahead集合中的terminal，那么就可以规约掉这个产生式
class LR1StateProduction :public StateProduction
{
public:
    LR1StateProduction(const Production& inProduction, int inDotPos = 0) : StateProduction(inProduction, inDotPos) {}
    LR1StateProduction(const Production& inProduction, const TerminalsSet& inLookahead, int inDotPos = 0) : StateProduction(inProduction, inDotPos), lookahead(inLookahead) {}

    virtual string getFormatString() const override
    {
        std::string result = production.no_terminal.getFormatString() + " -> ";
        for (int i = 0; i < production.right.count(); ++i)
        {
            if (i == dotPos)
                result += ".";
            result += production.right.at(i).getFormatString() + " ";
        }
        if (dotPos == production.right.count())
            result += ".";

        result = result + " , lookahead: " + lookahead.getFormatString();
        return result;
    }
    virtual void print() const override
    {
        std::cout << getFormatString() << endl;
    }

    //本产生式是从other产生式发展而来的，other是一个LR1StateProduction，dot_pos是other的点位置，productionSet是所有产生式的集合。根据LR(1)分析的定义，other的lookahead集合应该被加入到当前产生式的lookahead集合中。
    void generate_lookahead(const LR1StateProduction& other, int dot_pos, const ProductionSet& productionSet)
    {
        ProductionRight slice_right = other.production.right.slice_from(dot_pos);
        slice_right.add_items(other.lookahead);
        lookahead = productionSet.calculate_production_right_firstSet(slice_right);
    }
    bool operator == (const LR1StateProduction& other)const
    {
        if (lookahead != other.lookahead)
            return false;
        return StateProduction::operator==(other);
    }
public:
    TerminalsSet lookahead; // lookahead集合，表示在这个状态下，如果输入lookahead集合中的terminal，那么就可以规约掉这个产生式
};