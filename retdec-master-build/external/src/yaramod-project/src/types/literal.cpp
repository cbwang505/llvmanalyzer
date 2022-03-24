/**
 * @file src/types/literal.cpp
 * @brief Implementation of class Literal.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <algorithm>
#include <cassert>
#include <stack>

#include "yaramod/types/literal.h"
#include "yaramod/utils/utils.h"


namespace yaramod {

/**
 * Constructor.
 *
 * @param value String value of the literal.
 * @param formated_value formatted value of the literal.
 */
Literal::Literal(const std::string& value, const std::optional<std::string>& formated_value/* = std::nullopt*/)
	: _value(value)
	, _formatted_value(formated_value)
{
}

/**
 * Constructor.
 *
 * @param value String value of the literal.
 * @param formated_value formatted value of the literal.
 */
Literal::Literal(const char* value, const std::optional<std::string>& formated_value/* = std::nullopt*/)
	: _value(std::string(value))
	, _formatted_value(formated_value)
{
}

/**
 * Constructor.
 *
 * @param value String value of the literal.
 * @param formated_value formatted value of the literal.
 */
Literal::Literal(std::string&& value, const std::optional<std::string>& formated_value/* = std::nullopt*/)
	: _value(std::move(value))
	, _formatted_value(formated_value)
{
}

/**
 * Constructor.
 *
 * @param value Bool value of the literal.
 */
Literal::Literal(bool value, const std::optional<std::string>& formated_value/* = std::nullopt*/)
	: _value(value)
	, _formatted_value(formated_value)
{
}

/**
 * Constructor.
 *
 * @param value integral value of the literal.
 * @param integral_formatted_value formatted value of the integral literal.
 */
Literal::Literal(std::int64_t value, const std::optional<std::string>& integral_formatted_value/* = std::nullopt*/)
	: _value(value)
	, _formatted_value(integral_formatted_value)
{
}

/**
 * Constructor.
 *
 * @param value integral value of the literal.
 * @param integral_formatted_value formatted value of the integral literal.
 */
Literal::Literal(std::uint64_t value, const std::optional<std::string>& integral_formatted_value/* = std::nullopt*/)
	: _value(value)
	, _formatted_value(integral_formatted_value)
{
}

/**
 * Constructor.
 *
 * @param value integral value of the literal.
 * @param integral_formatted_value formatted value of the integral literal.
 */
Literal::Literal(double value, const std::optional< std::string >& integral_formatted_value/*= std::nullopt*/)
	: _value(value)
	, _formatted_value(integral_formatted_value)
{
}

/**
 * Constructor.
 *
 * @param value Symbol value of the literal.
 */
Literal::Literal(const std::shared_ptr<Symbol>& value)
	: _value(value)
{
}

/**
 * Constructor.
 *
 * @param value Symbol value of the literal.
 */
Literal::Literal(std::shared_ptr<Symbol>&& value)
	: _value(std::move(value))
{
}

/**
 * Constructor.
 *
 * @param value Reference to another Literal.
 */
Literal::Literal(ReferenceType value)
	: _value(value)
{
}

/**
 * Setter methods
 *
 */
void Literal::setValue(const std::string& s)
{
	if (isLiteralReference())
		const_cast<Literal*>(getLiteralReference())->setValue(s);
	else
		_value = s;
}

void Literal::setValue(std::string&& s)
{
	if (isLiteralReference())
		const_cast<Literal*>(getLiteralReference())->setValue(std::move(s));
	else
		_value = std::move(s);
}

void Literal::setValue(bool b)
{
	if (isLiteralReference())
		const_cast<Literal*>(getLiteralReference())->setValue(b);
	else
		_value = b;
}

void Literal::setValue(std::int64_t i, const std::optional<std::string>& integral_formatted_value/*= std::nullopt*/)
{
	if (isLiteralReference())
		const_cast<Literal*>(getLiteralReference())->setValue(i, integral_formatted_value);
	else
	{
		_value = i;
		_formatted_value = integral_formatted_value;
	}
}

void Literal::setValue(std::uint64_t i, const std::optional<std::string>& integral_formatted_value/*= std::nullopt*/)
{
	if (isLiteralReference())
		const_cast<Literal*>(getLiteralReference())->setValue(i, integral_formatted_value);
	else
	{
		_value = i;
		_formatted_value = integral_formatted_value;
	}
}

void Literal::setValue(double d, const std::optional<std::string>& integral_formatted_value/*= std::nullopt*/)
{
	if (isLiteralReference())
		const_cast<Literal*>(getLiteralReference())->setValue(d, integral_formatted_value);
	else
	{
		_value = d;
		_formatted_value = integral_formatted_value;
	}
}

void Literal::setValue(const std::shared_ptr<Symbol>& s)
{
	if (isLiteralReference())
		const_cast<Literal*>(getLiteralReference())->setValue(s);
	else
		_value = s;
}

void Literal::setValue(std::shared_ptr<Symbol>&& s)
{
	if (isLiteralReference())
		const_cast<Literal*>(getLiteralReference())->setValue(std::move(s));
	else
		_value = std::move(s);
}

void Literal::setValue(ReferenceType l)
{
	_value = l;
}

std::string Literal::getFormattedValue() const
{
	return _formatted_value.value_or(std::string());
}

/**
 * Returns the string representation of the literal in a specified format:
 *
 * @param pure Flag only used for string literals. If set, the exact form this was created in is returned -- without quotes.
 * @return String representation.
 */
std::string Literal::getText(bool pure/* = false*/) const
{
	if (isString())
	{
		const auto& output = getString();
		if (pure)
			return _escaped ? unescapeString(output) : output;
		else
			return '"' + output + '"';
	}
	else if (isBool())
	{
		if (_formatted_value.has_value())
			return _formatted_value.value();
		std::ostringstream ss;
		ss << std::boolalpha << getBool();
		return ss.str();
	}
	else if (isInt())
	{
		if (_formatted_value.has_value())
			return _formatted_value.value();
		else
			return numToStr(getInt());
	}
	else if (isFloat())
	{
		if (_formatted_value.has_value())
			return _formatted_value.value();
		else
			return numToStr(getFloat());
	}
	else if (isSymbol())
	{
		return getSymbol()->getName();
	}
	else if (isLiteralReference())
	{
		return getLiteralReference()->getText(pure);
	}
	else
	{
		std::stringstream err;
		err << "Error: Unexpected index of Literal value. Index: " << _value.index() << std::endl;
		throw YaramodError(err.str());
	}
	return std::string();
}

/**
 * Returns the string in the exact form it was written in.
 *
 * @return String representation.
 */
std::string Literal::getPureText() const
{
	return getText(true);
}

} //namespace yaramod
