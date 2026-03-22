#include "Production.h"

void Production::print() const
{
    std::cout << getFormatString();
}

std::string Production::getFormatString() const
{
    std::string result = no_terminal.getFormatString() + " -> " + right.getFormatString() + "\n";
    return result;
}

NoTerminal Production::left() const { return no_terminal; }

int Production::right_size() const { return right.count(); }

void Production::mergeFirstSet(const Production& other) const
{
    firstSet.merge(other.firstSet);
}

// 产物的可空性标志在 ProductionSet::calculate_all_production_firstSet 中设置


