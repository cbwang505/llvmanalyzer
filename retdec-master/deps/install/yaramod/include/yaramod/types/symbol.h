/**
 * @file src/types/symbol.h
 * @brief Declaration of class Symbol.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <unordered_map>

#include "yaramod/types/expression_type.h"
#include "yaramod/types/token_type.h"

namespace yaramod {

/**
 * Abstract class for representing symbols of the identifiers.
 * Symbols can be of type value symbol (representing dummy symbol or integer/string value),
 * array symbol (representing array object), dictionary symbol (representing dictionary object),
 * function symbol (representing function) or structure symbol (representing structure object).
 * Symbols carry certain data type of the expression. Data type depends on what symbol represents.
 */
class Symbol
{
public:
	/// Type of the symbol.
	enum class Type
	{
		Value,
		Array,
		Dictionary,
		Function,
		Structure,
	};

	/// @name Destructor
	/// @{
	virtual ~Symbol() = default;
	/// @}

	/// @name Getter methods
	/// @{
	const std::string& getName() const { return _name; }
	ExpressionType getDataType() const { return _dataType; }
	Symbol::Type getType() const { return _type; }
	TokenType getTokenType() const
	{
		switch(_type)
		{
			case Type::Value : return VALUE_SYMBOL;
			case Type::Array : return ARRAY_SYMBOL;
			case Type::Dictionary : return DICTIONARY_SYMBOL;
			case Type::Function : return FUNCTION_SYMBOL;
			case Type::Structure : return STRUCTURE_SYMBOL;
			default: return INVALID;
		}
	}
	/// @}

	/// @name Setter methods
	/// @{
	template<typename T>
	void setName(T&& name) { _name = std::forward<T>(name); }
	/// @}

	/// @name Detection methods
	/// @{
	bool isValue() const { return _type == Symbol::Type::Value; }
	bool isArray() const { return _type == Symbol::Type::Array; }
	bool isDictionary() const { return _type == Symbol::Type::Dictionary; }
	bool isFunction() const { return _type == Symbol::Type::Function; }
	bool isStructure() const { return _type == Symbol::Type::Structure; }
	/// @}

protected:
	/// @name Constructors
	/// @{
	Symbol(Symbol::Type type, const std::string& name, ExpressionType dataType)
		: _type(type), _name(name), _dataType(dataType) {}
	/// @}

	Symbol::Type _type; ///< Type of the symbol
	std::string _name; ///< Name
	ExpressionType _dataType; ///< Data type of the symbol
};

}
