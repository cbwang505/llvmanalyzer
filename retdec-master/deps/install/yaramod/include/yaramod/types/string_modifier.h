/**
 * @file src/types/string_modifier.h
 * @brief Declaration of class StringModifier.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <optional>
#include <string>
#include <variant>

#include "yaramod/types/token.h"
#include "yaramod/utils/utils.h"

namespace yaramod {

class StringModifier
{
public:
	/// String modifiers type.
	enum class Type
	{
		Ascii,
		Wide,
		Nocase,
		Fullword,
		Private,
		Xor
	};

	StringModifier(Type type, const std::string& name, TokenIt firstToken, TokenIt lastToken) : _type(type), _name(name), _tokens(firstToken, lastToken) {}
	StringModifier(const StringModifier&) = default;
	StringModifier(StringModifier&&) noexcept = default;
	virtual ~StringModifier() = default;

	StringModifier& operator=(const StringModifier&) = default;
	StringModifier& operator=(StringModifier&&) noexcept = default;

	Type getType() const { return _type; }
	const std::string& getName() const { return _name; }

	/**
	 * Returns token range [first, last]. It is a closed interval.
	 */
	const std::pair<TokenIt, TokenIt>& getTokenRange() const { return _tokens; }

	bool isAscii() const { return _type == Type::Ascii; }
	bool isWide() const { return _type == Type::Wide; }
	bool isNocase() const { return _type == Type::Nocase; }
	bool isFullword() const { return _type == Type::Fullword; }
	bool isPrivate() const { return _type == Type::Private; }
	bool isXor() const { return _type == Type::Xor; }

	virtual std::string getText() const = 0;

private:
	Type _type;
	std::string _name;
	std::pair<TokenIt, TokenIt> _tokens;
};

class AsciiStringModifier : public StringModifier
{
public:
	AsciiStringModifier(TokenIt token) : StringModifier(Type::Ascii, "ascii", token, token) {}

	virtual std::string getText() const override { return getName(); }
};

class WideStringModifier : public StringModifier
{
public:
	WideStringModifier(TokenIt token) : StringModifier(Type::Wide, "wide", token, token) {}

	virtual std::string getText() const override { return getName(); }
};

class NocaseStringModifier : public StringModifier
{
public:
	NocaseStringModifier(TokenIt token) : StringModifier(Type::Nocase, "nocase", token, token) {}

	virtual std::string getText() const override { return getName(); }
};

class FullwordStringModifier : public StringModifier
{
public:
	FullwordStringModifier(TokenIt token) : StringModifier(Type::Fullword, "fullword", token, token) {}

	virtual std::string getText() const override { return getName(); }
};

class PrivateStringModifier : public StringModifier
{
public:
	PrivateStringModifier(TokenIt token) : StringModifier(Type::Private, "private", token, token) {}

	virtual std::string getText() const override { return getName(); }
};

class XorStringModifier : public StringModifier
{
public:
	XorStringModifier(TokenIt token) : StringModifier(Type::Xor, "xor", token, token), _low(), _high() {}

	XorStringModifier(TokenIt firstToken, TokenIt lastToken, std::uint32_t key) : StringModifier(Type::Xor, "xor", firstToken, lastToken), _low(key), _high()
	{
		if (key > 255)
			throw YaramodError("Error: XOR string modifier key is out of allowed range");
	}

	XorStringModifier(TokenIt firstToken, TokenIt lastToken, std::uint32_t low, std::uint32_t high) : StringModifier(Type::Xor, "xor", firstToken, lastToken), _low(low), _high(high)
	{
		if (low > 255 || high > 255)
			throw YaramodError("Error: XOR string modifier key is out of allowed range");

		if (low > high)
			throw YaramodError("Error: XOR string modifier has lower bound of key greater then higher bound");
	}

	/**
	 * Indicates whether it is xor modifier with range of keys from N to M.
	 */
	bool isRange() const { return _low && _high; }

	/**
	 * Indicates whether it is xor modifier with single key N.
	 */
	bool isSingleKey() const { return _low && !_high; }

	virtual std::string getText() const override
	{
		if (isRange())
			return getName() + '(' + numToStr(_low.value()) + '-' + numToStr(_high.value()) + ')';
		else if (isSingleKey())
			return getName() + '(' + numToStr(_low.value()) + ')';
		else
			return getName();
	}

private:
	std::optional<std::uint32_t> _low, _high;
};

}
