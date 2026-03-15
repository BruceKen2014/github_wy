#include "ProductionSet.h"
#include <iostream>

void ProductionSet::print() const
{
    for (const auto &p : productions)
        p.print();
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

void ProductionSet::print_all_firstSet() const
{
    for (const auto& p : productions)
    {
        std::cout << "production " << p.getFormatString() << " FIRST(" << p.firstSet.getFormatString() << ")" << std::endl;
    }
    for (const auto& [key, value] : FIRST)
    {
		cout << "FIRST(" << key.getFormatString() << ") = " << value.getFormatString() << std::endl;
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
