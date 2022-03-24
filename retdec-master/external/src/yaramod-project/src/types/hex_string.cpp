/**
 * @file src/types/hex_string.cpp
 * @brief Implementation of class HexString.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/hex_string.h"
#include "yaramod/utils/utils.h"

namespace yaramod {

/**
 * Constructor.
 *
 * @param units Units of the hex string.
 */
HexString::HexString(const std::shared_ptr<TokenStream>& ts, const std::vector<std::shared_ptr<HexStringUnit>>& units)
	: String(ts, String::Type::Hex), _units(units)
{
}

/**
 * Constructor.
 *
 * @param units Units of the hex string.
 */
HexString::HexString(const std::shared_ptr<TokenStream>& ts, std::vector<std::shared_ptr<HexStringUnit>>&& units)
	: String(ts, String::Type::Hex), _units(std::move(units))
{
}

/**
 * Return the string representation of the hex string.
 *
 * @return String representation.
 */
std::string HexString::getText() const
{
	return "{ " + getPureText() + " }";
}

/**
 * Return the string representation of the hex string.
 *
 * @return Pure string representation.
 */
std::string HexString::getPureText() const
{
	std::ostringstream ss;
	for (auto itr = _units.begin(), end = _units.end(); itr != end; )
	{
		const auto& unit = *itr;
		assert(unit);
		// Nibbles and wildcards are always in pairs.
		if (unit->isNibble() || unit->isWildcard())
		{
			assert(itr + 1 != end);
			const auto& second = *(itr + 1);
			assert(second->isNibble() || second->isWildcard());
			ss << unit->getText() << second->getText() << ' ';
			itr += 2;
		}
		else
		{
			ss << unit->getText() << ' ';
			++itr;
		}
	}
	// Remove last space from the result.
	return trim(ss.str());
}

TokenIt HexString::getFirstTokenIt() const
{
	return _tokenStream->begin();
}

TokenIt HexString::getLastTokenIt() const
{
	if (_units.empty())
		return _tokenStream->begin();
	else
		return std::prev(_tokenStream->end());
}

/**
 * Returns the length of the hex string as the number of nibbles.
 * Jumps and wildcards are not counted towards the length.
 *
 * @return Length of hex string.
 */
std::size_t HexString::getLength() const
{
	return std::accumulate(_units.begin(), _units.end(), static_cast<std::size_t>(0),
			[](std::size_t acc, const auto& unit) {
				return acc + unit->getLength();
			});
}

}
