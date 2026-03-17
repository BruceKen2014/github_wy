#include "ProductionRight.h"

bool ProductionRightItem::operator==(const ProductionRightItem& other) const
{
    if (type != other.type)
        return false;
    if (type == ProductionRightItemType::terminal)
        return terminal == other.terminal;
    else
        return noTerminal == other.noTerminal;
}

bool ProductionRightItem::operator<(const ProductionRightItem& other) const
{
    if (type != other.type)
        return type < other.type;
    if (type == ProductionRightItemType::terminal)
        return terminal < other.terminal;
    return noTerminal < other.noTerminal;
}

const std::string& ProductionRightItem::getFormatString() const
{
    if (type == ProductionRightItemType::terminal)
        return terminal.getFormatString();
    else
        return noTerminal.getFormatString();
}

bool ProductionRight::operator==(const ProductionRight& other) const
{
    if (items.size() != other.items.size())
        return false;
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (!(items[i] == other.items[i]))
            return false;
    }
    return true;
}

const std::string ProductionRight::getFormatString() const
{
    std::string result;
    for (const auto& item : items)
        result += item.getFormatString() + " ";
    return result;
}
