/**
 * @file src/types/plain_string.cpp
 * @brief Implementation of class PlainString.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/plain_string.h"
#include "yaramod/utils/utils.h"

namespace yaramod {

/**
 * Constructor.
 *
 * @param text Text of the plain string.
 */
PlainString::PlainString(const std::shared_ptr<TokenStream>& ts, const std::string& text)
	: String(ts, String::Type::Plain)
{
	_text = ts->emplace_back(TokenType::STRING_LITERAL, text);
}

/**
 * Constructor.
 *
 * @param text Text of the plain string.
 */
PlainString::PlainString(const std::shared_ptr<TokenStream>& ts, std::string&& text)
	: String(ts, String::Type::Plain)
{
	_text = ts->emplace_back(TokenType::STRING_LITERAL, std::move(text));
}

/**
 * Constructor.
 *
 * @param text Text of the plain string.
 */
PlainString::PlainString(const std::shared_ptr<TokenStream>&  ts, TokenIt text)
	: String(ts, String::Type::Plain)
	, _text(text)
{
	if (!text->isString())
		throw YaramodError("String class identifier must be string.");
	assert(text->getType() == TokenType::STRING_LITERAL);
}

PlainString::PlainString(const std::shared_ptr<TokenStream>& ts, TokenIt id, TokenIt assignToken, TokenIt text)
		: String(ts, String::Type::Plain, id, assignToken)
		, _text(text)
{
}

/**
 * Return the string representation of the plain string.
 *
 * @return String representation.
 */
std::string PlainString::getText() const
{
	return '"' + _text->getString() + '"' + getModifiersText();
}

/**
 * Return the pure string representation of the plain string.
 *
 * @return Pure string representation.
 */
std::string PlainString::getPureText() const
{
	return unescapeString(_text->getString());
}

TokenIt PlainString::getFirstTokenIt() const
{
	return _text;
}

TokenIt PlainString::getLastTokenIt() const
{
	return _text;
}

}
