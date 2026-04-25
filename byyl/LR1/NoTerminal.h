// 非终结符定义
#pragma once

#include <string>

class NoTerminal
{
public:
	NoTerminal() = default;
	NoTerminal(const std::string &inName) : name(inName) {}

	bool operator==(const NoTerminal &other) const { return name == other.name; }
	bool operator<(const NoTerminal &other) const { return name < other.name; }

	const std::string& getFormatString() const noexcept { return name; }

private:
	std::string name; // 非终结符名称
};


