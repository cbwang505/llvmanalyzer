/**
 * @file src/types/symbols.h
 * @brief Declaration of all Symbol subclasses.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <optional>
#include <vector>

#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Class representing value symbol. Value symbol carries only name
 * of the symbol and some data type.
 */
class ValueSymbol : public Symbol
{
public:
	ValueSymbol(const std::string& name, ExpressionType dataType) : Symbol(Symbol::Type::Value, name, dataType) {}
};

/**
 * Abstract class representing iterable symbol. Iterable symbol may be
 * array or dictionary symbol. Iterable symbols store data type of the elements
 * they are iterating over. If the element type is @c ExpressionType::Object then
 * iterable symbol also carries the symbol representing structured type of the element.
 */
class IterableSymbol : public Symbol
{
public:
	ExpressionType getElementType() const { return _elementType; }
	const std::shared_ptr<Symbol>& getStructuredElementType() const { return _structuredType; }

	bool isStructured() const { return _elementType == ExpressionType::Object && _structuredType; }

protected:
	IterableSymbol(Symbol::Type type, const std::string& name, ExpressionType elementType)
		: Symbol(type, name, ExpressionType::Object), _elementType(elementType), _structuredType() {}
	IterableSymbol(Symbol::Type type, const std::string& name, const std::shared_ptr<Symbol>& structuredType)
		: Symbol(type, name, ExpressionType::Object), _elementType(ExpressionType::Object), _structuredType(structuredType) {}

	ExpressionType _elementType; ///< Element of the iterated data
	std::shared_ptr<Symbol> _structuredType; ///< Structured type of the object elements
};

/**
 * Class representing array symbol. Array symbol carries name of the array and type of the element of the array.
 * Data type of the whole array symbol is always @c Symbol::Type::Array.
 */
class ArraySymbol : public IterableSymbol
{
public:
	ArraySymbol(const std::string& name, ExpressionType elementType) : IterableSymbol(Symbol::Type::Array, name, elementType) {}
	ArraySymbol(const std::string& name, const std::shared_ptr<Symbol>& structuredType) : IterableSymbol(Symbol::Type::Array, name, structuredType) {}
};

/**
 * Class representing dictionary symbol. Dictionary symbol carries name of the dictionary and type of the element of the dictionary.
 * Data type of the whole dictionary symbol is always @c Symbol::Type::Dictionary.
 */
class DictionarySymbol : public IterableSymbol
{
public:
	DictionarySymbol(const std::string& name, ExpressionType elementType) : IterableSymbol(Symbol::Type::Dictionary, name, elementType) {}
	DictionarySymbol(const std::string& name, const std::shared_ptr<Symbol>& structuredType) : IterableSymbol(Symbol::Type::Array, name, structuredType) {}
};

/**
 * Class representing function symbol. Function symbol carries name of the function, return type of the function
 * and argument types of all possible overloads of that function.
 */
class FunctionSymbol : public Symbol
{
public:
	template <typename... Args>
	FunctionSymbol(const std::string& name, ExpressionType returnType, const Args&... args)
		: Symbol(Symbol::Type::Function, name, ExpressionType::Object), _returnType(returnType), _argTypesOverloads(1)
	{
		_initArgs(args...);
	}

	ExpressionType getReturnType() const { return _returnType; }
	const std::vector<std::vector<ExpressionType>>& getAllOverloads() const { return _argTypesOverloads; }

	std::size_t getArgumentCount(std::size_t overloadIndex = 0) const
	{
		assert(overloadIndex < _argTypesOverloads.size());
		return _argTypesOverloads[overloadIndex].size();
	}

	std::vector<ExpressionType> getArgumentTypes(std::size_t overloadIndex = 0) const
	{
		assert(overloadIndex < _argTypesOverloads.size());
		return _argTypesOverloads[overloadIndex];
	}

	bool addOverload(const std::vector<ExpressionType>& argTypes)
	{
		if (overloadExists(argTypes))
			return false;

		_argTypesOverloads.push_back(argTypes);
		return true;
	}

	bool overloadExists(const std::vector<ExpressionType>& args) const
	{
		for (const auto& overload : _argTypesOverloads)
		{
			if (overload.size() != args.size())
				continue;

			// No mismatch in two vectors, so they are completely the same.
			auto mismatch = std::mismatch(overload.begin(), overload.end(), args.begin());
			if (mismatch.first == overload.end())
				return true;
		}

		return false;
	}

private:
	void _initArgs() {}

	template <typename... Args>
	void _initArgs(ExpressionType argType, const Args&... args)
	{
		_argTypesOverloads.front().push_back(argType);
		_initArgs(args...);
	}

	ExpressionType _returnType; ///< Return type of the function
	std::vector<std::vector<ExpressionType>> _argTypesOverloads; ///< All possible overloads of the function
};

/**
 * Class representing structure symbol. Structure symbol carries name of the structure and its attributes.
 */
class StructureSymbol : public Symbol
{
public:
	StructureSymbol(const std::string& name) : Symbol(Symbol::Type::Structure, name, ExpressionType::Object) {}

	std::optional<std::shared_ptr<Symbol>> getAttribute(const std::string& name) const
	{
		auto itr = _attributes.find(name);
		if (itr == _attributes.end())
			return std::nullopt;

		return { itr->second };
	}

	bool addAttribute(const std::shared_ptr<Symbol>& attribute)
	{
		// Insertion result is pair of iterator and boolean indicator whether insertion was successful
		auto insertionResult = _attributes.emplace(attribute->getName(), attribute);
		if (insertionResult.second)
			return true;

		// Insertion did not succeed and we must handle that
		auto itr = insertionResult.first;

		// If we are trying to add a function and function with that name already exists,
		// it may be function overload, so check that.
		if (itr->second->isFunction() && attribute->isFunction())
		{
			auto oldFunction = std::static_pointer_cast<FunctionSymbol>(itr->second);
			auto newFunction = std::static_pointer_cast<const FunctionSymbol>(attribute);

			// Overload return types must be the same, only argument count and types may differ.
			if (oldFunction->getReturnType() != newFunction->getReturnType())
				return false;

			return oldFunction->addOverload(newFunction->getArgumentTypes());
		}

		return false;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Symbol>> _attributes; ///< Attributes of the structure
};

}
