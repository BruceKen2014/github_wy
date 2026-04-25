#pragma once


#include "Production.h"
using namespace std;

class StateProduction :public Production
{
public:
    StateProduction(const Production& inProduction, int inDotPos = 0) : Production(inProduction), dotPos(inDotPos) {
        str = getFormatString();
    };

    virtual string getFormatString() const override
    {
        std::string result = no_terminal.getFormatString() + " -> ";
        for (int i = 0; i < right.count(); ++i)
        {
            if (i == dotPos)
                result += ".";
            result += right.at(i).getFormatString() + " ";
        }
        if (dotPos == right.count())
            result += ".";
        return result;
    }
    virtual void print() const override
    {
        std::cout << getFormatString();
    }
    bool operator == (const StateProduction& other)const
    {
        if (dotPos != other.dotPos)
            return false;
        return Production::operator==(other);
    }
    bool can_reduce() const
    {
        return dotPos == right.count();
	}
public:
    int dotPos; // 点的位置，范围是0到right.count()，表示点在右部的哪个位置
    string str;
};