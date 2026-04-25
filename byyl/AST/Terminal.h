// 终结符定义
#pragma once

#include <string>

class Terminal
{
public:
	static const Terminal END; // 定义一个特殊的终结符，表示输入结束
public:
	Terminal() = default;
	constexpr Terminal(const std::string& inName, int inValue = 0) : name(inName), value(inValue) {}

	bool operator==(const Terminal& other) const { return name == other.name; }
	bool operator<(const Terminal& other) const { return name < other.name; }

	bool is_end() const { return name == "$"; }
	const std::string& getFormatString() const noexcept { return name; }


public:
	int value = 0; //仅用于数字
	std::string name; // 终结符名称
};


