/**
 * @file src/parser/value.h
 * @brief Declaration of class Value.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <cstdint>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

#include "yaramod/types/hex_string.h"
#include "yaramod/types/rule.h"
#include "yaramod/types/regexp.h"
#include "yaramod/types/token_stream.h"

#pragma once

namespace yaramod {

using RegexpRangePair = std::pair<std::optional<std::uint64_t>, std::optional<std::uint64_t>>;
using StringModifiers = std::vector<std::shared_ptr<StringModifier>>;
using RegexpClassRecord = std::pair<bool, std::string>;

/**
 * Value is the type of all tokens produced by POG parser. Both token and rule actions return Value. The rule action parameters are also Values.
 */
class Value
{
public:
	using Variant = std::variant<
		std::string, //0
		int,
		bool, //2
		std::optional<TokenIt>,
		Rule, //4
		std::vector<Meta>,
		std::shared_ptr<Rule::StringsTrie>, //6
		std::shared_ptr<StringModifier>,
		StringModifiers, //8
		Literal,
		Expression::Ptr, //10
		std::vector<Expression::Ptr>,
		std::vector<TokenIt>, //12
		std::vector<std::shared_ptr<HexStringUnit>>,
		std::shared_ptr<HexStringUnit>, //14
		std::vector<std::shared_ptr<HexString>>,
		std::shared_ptr<String>, //16
		std::shared_ptr<RegexpUnit>,
		std::vector<std::shared_ptr<RegexpUnit>>, //18
		TokenIt,
		RegexpRangePair, //20
		RegexpClassRecord
	>;

	/// @name Constructors
	/// @{
	template <typename T>
	Value(T&& v)
		: _value(std::forward<T>(v))
	{
	}
	Value() = default;
	/// @}

	/// @name Getter methods
	/// @{
	const std::string& getString() const
	{
		return getValue<std::string>();
	}

	int getInt() const
	{
		return getValue<int>();
	}

	bool getBool() const
	{
		return getValue<bool>();
	}

	TokenIt getTokenIt() const
	{
		return getValue<TokenIt>();
	}

	std::optional<TokenIt> getOptionalTokenIt() const
	{
		return getValue<std::optional<TokenIt>>();
	}

	const Rule& getRule() const
	{
		return getValue<Rule>();
	}

	std::vector<Meta>&& getMetas()
	{
		return std::move(moveValue<std::vector<Meta>>());
	}

	std::shared_ptr<Rule::StringsTrie>&& getStringsTrie()
	{
		return std::move(moveValue<std::shared_ptr<Rule::StringsTrie>>());
	}

	std::shared_ptr<StringModifier>&& getStringMod()
	{
		return std::move(moveValue<std::shared_ptr<StringModifier>>());
	}

	StringModifiers&& getStringMods()
	{
		return std::move(moveValue<StringModifiers>());
	}

	const Literal& getLiteral() const
	{
		return getValue<Literal>();
	}

	Expression::Ptr getExpression() const
	{
		return getValue<Expression::Ptr>();
	}

	std::vector<Expression::Ptr>&& getMultipleExpressions()
	{
		return std::move(moveValue<std::vector<Expression::Ptr>>());
	}

	std::vector<TokenIt>&& getMultipleTokenIt()
	{
		return std::move(moveValue<std::vector<TokenIt>>());
	}

	std::vector<std::shared_ptr<HexStringUnit>>&& getMultipleHexUnits()
	{
		return std::move(moveValue<std::vector<std::shared_ptr<HexStringUnit>>>());
	}

	std::shared_ptr<HexStringUnit>&& getHexUnit()
	{
		return std::move(moveValue<std::shared_ptr<HexStringUnit>>());
	}

	std::vector<std::shared_ptr<HexString>>&& getMultipleHexStrings()
	{
		return std::move(moveValue<std::vector<std::shared_ptr<HexString>>>());
	}

	std::shared_ptr<String>&& getYaramodString()
	{
		return std::move(moveValue<std::shared_ptr<String>>());
	}

	std::shared_ptr<RegexpUnit>&& getRegexpUnit()
	{
		return std::move(moveValue<std::shared_ptr<RegexpUnit>>());
	}

	std::vector<std::shared_ptr<RegexpUnit>>&& getMultipleRegexpUnits()
	{
		return std::move(moveValue<std::vector<std::shared_ptr<RegexpUnit>>>());
	}

	RegexpRangePair&& getRegexpRangePair()
	{
		return std::move(moveValue<RegexpRangePair>());
	}

	RegexpClassRecord&& getRegexpClassRecord()
	{
		return std::move(moveValue<RegexpClassRecord>());
	}
	/// @}

protected:
	template <typename T>
	const T& getValue() const
	{
		try
		{
			return std::get<T>(_value);
		}
		catch (std::bad_variant_access& exp)
		{
			// Uncomment for debugging
			// std::cerr << "Called Value.getValue() with incompatible type. Actual index is '" << _value.index() << "'" << std::endl << exp.what() << std::endl;
			// std::cerr << "Call: '" << __PRETTY_FUNCTION__ << "'" << std::endl;
			throw YaramodError("Called getValue<T>() with incompatible type T.", exp.what());
		}
	}
	template < typename T>
	T&& moveValue()
	{
		try
		{
			return std::move(std::get<T>(std::move(_value)));
		}
		catch (std::bad_variant_access& exp)
		{
			// Uncomment for debugging
			// std::cerr << "Called Value.moveValue() with incompatible type. Actual index is '" << _value.index() << "'" << std::endl << exp.what() << std::endl;
			// std::cerr << __PRETTY_FUNCTION__ << std::endl;
			throw YaramodError("Called getValue<T>() with incompatible type T.", exp.what());
		}
	}

private:
	Variant _value;
};

} // namespace yaramod
