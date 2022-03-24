/**
 * @file src/types/hex_string.h
 * @brief Declaration of class HexString.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <cassert>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <vector>

#include "yaramod/types/string.h"
#include "yaramod/types/token_stream.h"

namespace yaramod {

/**
 * Abstract class representing single unit in the hex string.
 * Hex string unit can be either nibble, wildcard, jump
 * or alternation.
 */
class HexStringUnit
{
public:
	///< Type of the hex string unit.
	enum class Type
	{
		Nibble,
		Wildcard,
		Jump,
		Or
	};

	/// @name Constructor
	/// @{
	HexStringUnit(Type type) : _type(type) {}
	virtual ~HexStringUnit() {}
	/// @}

	/// @name Virtual methods
	/// @{
	virtual std::string getText() const = 0;
	virtual std::size_t getLength() const = 0;
	/// @}

	/// @name Detection methods
	/// @{
	bool isNibble() const { return _type == Type::Nibble; }
	bool isWildcard() const { return _type == Type::Wildcard; }
	bool isJump() const { return _type == Type::Jump; }
	bool isOr() const { return _type == Type::Or; }
	/// @}

protected:
	Type _type; ///< Type of the unit
};

class YaraRuleBuilder;

/**
 * Class representing hex strings in the strings section
 * of the YARA rules.
 *
 * For example:
 * @code
 * $1 = { 9f 45 5d }
 * $2 = { 4c [5-9] ( 26 | 29 ) }
 * @endcode
 */
class HexString : public String
{
public:
	/// @name Constructors
	/// @{
	explicit HexString(const std::shared_ptr<TokenStream>& ts, const std::vector<std::shared_ptr<HexStringUnit>>& units);
	explicit HexString(const std::shared_ptr<TokenStream>& ts, std::vector<std::shared_ptr<HexStringUnit>>&& units);
	/// @}

	/// @name Virtual methods.
	/// @{
	virtual std::string getText() const override;
	virtual std::string getPureText() const override;
	virtual TokenIt getFirstTokenIt() const override;
	virtual TokenIt getLastTokenIt() const override;
	/// @}

	/// @name Getters
	/// @{
	const std::vector<std::shared_ptr<HexStringUnit>>& getUnits() const { return _units; }
	std::size_t getLength() const;
	/// @}

	/// @name Setters
	/// @{
	void setUnits(const std::vector<std::shared_ptr<HexStringUnit>>& units) { _units = units; }
	void setUnits(std::vector<std::shared_ptr<HexStringUnit>>&& units) { _units = std::move(units); }
	/// @}

	/// @name Iterators
	/// @{
	auto begin() const { return _units.begin(); }
	auto end() const { return _units.end(); }
	/// @}

	bool empty() const { return _units.empty(); }

private:
	std::vector<std::shared_ptr<HexStringUnit>> _units; ///< Units in the hex string
};

/**
 * Class representing nibble unit in the hex string.
 * Nibbles have values ranging from 0 to F (hexadecimal values).
 * Nibbles are occurring in pairs with other nibbles or wildcards
 * in the hex string.
 */
class HexStringNibble : public HexStringUnit
{
public:
	/// @name Constructors
	/// @{
	explicit HexStringNibble(TokenIt value) : HexStringUnit(Type::Nibble), _value(value) {}
	/// @}

	/// @name Virtual methods
	/// @{
	virtual std::string getText() const override
	{
		auto value = getValue();
		assert(value <= 0xf);

		if (value <= 9)
			return std::string(1, value + '0');
		else
			return std::string(1, value - 10 + 'A');
	}
	/// @}

	/// @name Getters
	/// @{
	std::uint8_t getValue() const
	{
		auto output = _value->getInt();
		assert(output <= 0xf);
		return output;
	}
	/// @}

	virtual std::size_t getLength() const override { return 1; }

private:
	TokenIt _value; ///< Value of the nibble
};

/**
 * Class representing wildcard unit in the hex string.
 * Wildcards are occurring in pairs with other nibbles or wildcards
 * in the hex string.
 */
class HexStringWildcard : public HexStringUnit
{
public:
	/// @name Constructors
	/// @{
	HexStringWildcard(TokenIt value) : HexStringUnit(Type::Wildcard), _value(value) {}
	/// @}

	/// @name Virtual methods
	/// @{
	virtual std::string getText() const override { return "?"; }
	virtual std::size_t getLength() const override { return 1; }
	/// @}
private:
	TokenIt _value; ///< Value of the nibble
};

/**
 * Class representing jump unit in the hex string.
 * Jumps can be either varying jumps ([-]), fixed jumps ([N]),
 * varying range jumps ([N-]) or range jumps ([N-M]).
 * Jumps can't be located at the beginning or at the end of hex string.
 */
class HexStringJump : public HexStringUnit
{
public:
	/// @name Constructors
	/// @{
	HexStringJump() : HexStringUnit(Type::Jump) {}
	HexStringJump(TokenIt low) : HexStringUnit(Type::Jump), _low(low), _high() {}
	HexStringJump(TokenIt low, TokenIt high) : HexStringUnit(Type::Jump), _low(low), _high(high) {}
	/// @}

	/// @name Virtual methods
	/// @{
	virtual std::string getText() const override
	{
		std::ostringstream ss;
		ss << '[';
		// If both low and high bound is defined and they are the same, it is the fixed jump.
		if (_low.has_value() && _high.has_value() && _low.value() == _high.value())
		{
			ss << _low.value()->getUInt();
		}
		else
		{
			if (_low.has_value())
				ss << _low.value()->getUInt();
			ss << '-';
			if (_high.has_value())
				ss << _high.value()->getUInt();
		}
		ss << ']';
		return ss.str();
	}

	virtual std::size_t getLength() const override { return 0; }
	/// @}

	/// @name Getters
	/// @{
	std::optional<std::uint64_t> getLow() const
	{
		if (_low.has_value())
			return _low.value()->getUInt();
		return std::nullopt;
	}
	std::optional<std::uint64_t> getHigh() const {
		if (_high.has_value())
			return _high.value()->getUInt();
		return std::nullopt;
   }
	/// @}

private:
	std::optional<TokenIt> _low, _high; ///< Low and high bounds of the jump.
};

/**
 * Class representing alternation (or) unit in the hex string.
 * Alternation holds another hex string in it recursively until it hits
 * the primitive units again.
 */
class HexStringOr : public HexStringUnit
{
public:
	/// @name Constructors
	/// @{
	explicit HexStringOr(const std::vector<std::shared_ptr<HexString>>& substrings) : HexStringUnit(Type::Or), _substrings(substrings) {}
	explicit HexStringOr(std::vector<std::shared_ptr<HexString>>&& substrings) : HexStringUnit(Type::Or), _substrings(std::move(substrings)) {}
	/// @}

	/// @name Virtual methods
	/// @{
	virtual std::string getText() const override
	{
		std::ostringstream ss;
		ss << "( ";
		for (const auto& substring : _substrings)
		{
			ss << substring->getPureText() << " | ";
		}
		ss << ")";

		// Remove last '| ' from the result.
		auto text = ss.str();
		return text.erase(text.length() - 4, 2);
	}

	virtual std::size_t getLength() const override
	{
		return std::accumulate(_substrings.begin(), _substrings.end(), static_cast<std::size_t>(0),
			[](std::size_t acc, const auto& substring) {
				return acc + substring->getLength();
			});
	}
	/// @}

	/// @name Getters
	/// @{
	const std::vector<std::shared_ptr<HexString>> getSubstrings() const { return _substrings; }
	/// @}

	/// @name Iterators
	/// @{
	auto begin() const { return _substrings.begin(); }
	auto end() const { return _substrings.end(); }
	/// @}

private:
	std::vector<std::shared_ptr<HexString>> _substrings;
};

}
