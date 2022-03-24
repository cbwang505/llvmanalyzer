/**
 * @file src/types/token.h
 * @brief Declaration of class Token.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <list>

#include "yaramod/types/literal.h"
#include "yaramod/types/location.h"
#include "yaramod/types/token_type.h"
#include "yaramod/yaramod_error.h"

namespace yaramod {

class Token;
using TokenIt = std::list<Token>::iterator;
using TokenConstIt = std::list<Token>::const_iterator;
using TokenItReversed = std::reverse_iterator<TokenIt>;
using TokenConstItReversed = std::reverse_iterator<TokenConstIt>;

class TokenStream;

/**
 * Class representing tokens that YARA rules consist of. Tokens do not store values and are stored in TokenStream
 */
class Token
{
public:
	Token(TokenType type, const Literal& value)
		: _type(type)
		, _value(std::make_shared<Literal>(value))
		, _location()
		, _wanted_column(0)
	{
	}

	Token(TokenType type, Literal&& value)
		: _type(type)
		, _value(std::make_shared<Literal>(std::move(value)))
		, _location()
		, _wanted_column(0)
	{
	}

	Token(const Token& other) = default;

	Token(Token&& other) = default;

	/// @name String representation
	/// @{
	std::string getText(bool pure = false) const;
	std::string getPureText() const;
	/// @}

	/// @name Setter methods
	/// @{
	void setValue(const Literal& new_value) { _value = std::make_shared<Literal>(new_value); }

	void setValue(const std::string& value) { _value->setValue(value); }
	void setValue(std::string&& value) { _value->setValue(std::move(value)); }
	void setValue(bool value) { _value->setValue(value); }
	void setValue(std::int64_t value, const std::optional<std::string>& integral_formated_value = std::nullopt) { _value->setValue(value, integral_formated_value); }
	void setValue(std::uint64_t value, const std::optional<std::string>& integral_formated_value = std::nullopt) { _value->setValue(value, integral_formated_value); }
	void setValue(double value, const std::optional<std::string>& integral_formated_value = std::nullopt) { _value->setValue(value, integral_formated_value); }
	void setValue(const std::shared_ptr<Symbol>& value) { _value->setValue(value); }
	void setValue(std::shared_ptr<Symbol>&& value) { _value->setValue(std::move(value)); }
	void setValue(Literal::ReferenceType value) { _value->setValue(value); }

	void setType(TokenType type) { _type = type; }
	void setFlag(bool flag) { _flag = flag; }
	void setLocation(const Location& location) { _location = location; }
	void setIndentation(std::size_t wanted_column) { _wanted_column = wanted_column; }
	void markEscaped() { _value->markEscaped(); }
	/// @}

	/// @name Detection methods
	/// @{
	bool isString() const { return _value->isString(); }
	bool isBool() const { return _value->isBool(); }
	bool isInt() const { return _value->isInt(); }
	bool isFloat() const { return _value->isFloat(); }
	bool isSymbol() const { return _value->isSymbol(); }
	bool isLiteralReference() const { return _value->isLiteralReference(); }

	bool isIncludeToken() const { return _subTokenStream != nullptr; }
	bool isLeftBracket() const
	{
		return _type == TokenType::LP
			|| _type == TokenType::LP_ENUMERATION
			|| _type == TokenType::HEX_JUMP_LEFT_BRACKET
			|| _type == TokenType::REGEXP_START_SLASH
			|| _type == TokenType::HEX_START_BRACKET
			|| _type == TokenType::LP_WITH_SPACE_AFTER
			|| _type == TokenType::LP_WITH_SPACES;
	}

	bool isRightBracket() const
	{
		return _type == TokenType::RP
			|| _type == TokenType::RP_ENUMERATION
			|| _type == TokenType::HEX_JUMP_RIGHT_BRACKET
			|| _type == TokenType::REGEXP_END_SLASH
			|| _type == TokenType::HEX_END_BRACKET
			|| _type == TokenType::RP_WITH_SPACE_BEFORE
			|| _type == TokenType::RP_WITH_SPACES;
	}

	bool isStringModifier() const
	{
		return _type == ASCII
			|| _type == WIDE
			|| _type == FULLWORD
			|| _type == NOCASE
			|| _type == XOR
			|| _type == PRIVATE;
	}
	/// @}

	friend std::ostream& operator<<(std::ostream& os, const Token& token)
	{
		switch(token._type)
		{
			case TokenType::META_VALUE:
			case TokenType::STRING_LITERAL:
			case TokenType::IMPORT_MODULE:
			case TokenType::INCLUDE_PATH:
				return os << token.getText();
			default:
				return os << token.getPureText();
		}
	}

	/// @name Getter methods
	/// @{
	TokenType getType() const { return _type; }
	const Literal& getLiteral() const;
	const std::string& getString() const;
	bool getBool() const;
	std::int64_t getInt() const;
	std::uint64_t getUInt() const;
	double getFloat() const;
	const std::shared_ptr<Symbol>& getSymbol() const;
	Literal::ReferenceType getLiteralReference() const;

	template <typename T>
	const T& getValue() const { return std::get<T>(_value); }
	bool getFlag() const { return _flag; }
	const Location& getLocation() const { return _location; }
	std::size_t getIndentation() const { return _wanted_column; }
	/// @}

	/// @name Include substream handler methods
	/// @{
	const std::shared_ptr<TokenStream>& getSubTokenStream() const;
	const std::shared_ptr<TokenStream>& initializeSubTokenStream();
	/// @}

private:
	bool _flag = false; // used for '(' to determine it's sector and whether to put newlines
	TokenType _type;
	std::shared_ptr<TokenStream> _subTokenStream = nullptr; // used only for INCLUDE_PATH tokens
	std::shared_ptr<Literal> _value; // pointer to the value owned by the Token
	Location _location; // Location in source input is stored in Tokens for precise error outputs
	std::size_t _wanted_column; // Wanted column where this Literal should be printed. Used for one-line comments.
};

} //namespace yaramod
