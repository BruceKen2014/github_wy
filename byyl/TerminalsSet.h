#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "Terminal.h"

// TerminalsSet，用于管理终结符数组
class TerminalsSet
{
public:
    TerminalsSet() = default;

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

private:
    std::vector<Terminal> terminals;
};


