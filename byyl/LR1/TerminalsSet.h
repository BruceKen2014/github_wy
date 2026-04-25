#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <initializer_list>
#include "Terminal.h"

// TerminalsSet，用于管理终结符数组
class TerminalsSet
{
public:
    TerminalsSet() = default;
    TerminalsSet(std::initializer_list<Terminal> inTerminals) :terminals(inTerminals) {};

    // 添加终结符
    void add(const Terminal &t);

    // 是否包含指定名称的终结符
    bool contains(const Terminal &terminal) const;

    // 根据名称删除终结符，返回是否删除成功
    bool removeTerminal(const Terminal& terminal);

    //并入另一个TerminalsSet
    void merge(const TerminalsSet& other);

    std::string getFormatString() const;

    void clear() noexcept { terminals.clear(); }

    size_t size() const noexcept { return terminals.size(); }

    const std::vector<Terminal> &all() const noexcept { return terminals; }

public:
    bool operator ==(const TerminalsSet &other) const
    {
        if (terminals.size() != other.terminals.size())
            return false;
        for (const auto &t : terminals)
        {
            if (!other.contains(t))
                return false;
        }
        return true;
	}
public:
    //for iterate
    auto begin() { return terminals.begin(); }
    auto end() { return terminals.end(); }

    //for iterate
    auto begin() const { return terminals.cbegin(); }
    auto end() const { return terminals.cend(); }
private:
    std::vector<Terminal> terminals;
};


