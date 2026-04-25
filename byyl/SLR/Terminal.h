// 终结符定义
#pragma once

#include <string>

class Terminal
{
public:
	Terminal() = default;
	Terminal(const std::string &inName) : name(inName) {}

	bool operator==(const Terminal &other) const { return name == other.name; }
	bool operator<(const Terminal &other) const { return name < other.name; }

	bool is_end() const { return name == "$"; }
	const std::string &getFormatString() const noexcept { return name; }

private:
    std::string name; // 终结符名称
};


