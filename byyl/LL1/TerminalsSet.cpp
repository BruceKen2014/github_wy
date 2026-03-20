#include "TerminalsSet.h"

void TerminalsSet::add(const Terminal &t)
{
    if (!contains(t))
        terminals.push_back(t);
}

bool TerminalsSet::contains(const Terminal &terminal) const
{
    return std::any_of(terminals.begin(), terminals.end(), [&](const Terminal &t) { return t == terminal; });
}

bool TerminalsSet::removeTerminal(const Terminal& terminal)
{
    auto it = std::remove_if(terminals.begin(), terminals.end(), [&](const Terminal &t) { return t == terminal; });
    if (it == terminals.end()) return false;
    terminals.erase(it, terminals.end());
    return true;
}

void TerminalsSet::merge(const TerminalsSet& other)
{
    for (const auto& t : other.terminals)
        add(t);
}

std::string TerminalsSet::getFormatString() const
{
    std::string result = "{ ";
    for (const auto& t : terminals)
        result += t.getFormatString() + " ";
    result += "}";
    return result;
}
