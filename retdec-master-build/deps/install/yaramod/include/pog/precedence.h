#pragma once

#include <cstdint>

namespace pog {

enum class Associativity
{
	Left,
	Right
};

struct Precedence
{
	std::uint32_t level;
	Associativity assoc;

	bool operator==(const Precedence& rhs) const { return level == rhs.level && assoc == rhs.assoc; }
	bool operator!=(const Precedence& rhs) const { return !(*this == rhs); }

	bool operator<(const Precedence& rhs) const
	{
		if (level < rhs.level)
			return true;
		else if (level == rhs.level)
		{
			if (assoc == Associativity::Right)
				return true;
		}

		return false;
	}

	bool operator>(const Precedence& rhs) const
	{
		if (level > rhs.level)
			return true;
		else if (level == rhs.level)
		{
			if (assoc == Associativity::Left)
				return true;
		}

		return false;
	}
};

} // namespace pog
