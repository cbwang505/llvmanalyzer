/**
 * @file src/types/modules/math_module.cpp
 * @brief Implementation of MathModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/math_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
MathModule::MathModule() : Module("math", ImportFeatures::Basic)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool MathModule::initialize(ImportFeatures/* features*/)
{
	using Type = Expression::Type;

	auto mathStruct = std::make_shared<StructureSymbol>("math");
	mathStruct->addAttribute(std::make_shared<ValueSymbol>("MEAN_BYTES", Type::Float));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("in_range", Type::Int, Type::Float, Type::Float, Type::Float));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("deviation", Type::Float, Type::Int, Type::Int, Type::Float));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("deviation", Type::Float, Type::String, Type::Float));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("mean", Type::Float, Type::Int, Type::Int));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("mean", Type::Float, Type::String));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("serial_correlation", Type::Float, Type::Int, Type::Int));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("serial_correlation", Type::Float, Type::String));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("monte_carlo_pi", Type::Float, Type::Int, Type::Int));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("monte_carlo_pi", Type::Float, Type::String));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("entropy", Type::Float, Type::Int, Type::Int));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("entropy", Type::Float, Type::String));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("min", Type::Int, Type::Int, Type::Int));
	mathStruct->addAttribute(std::make_shared<FunctionSymbol>("max", Type::Int, Type::Int, Type::Int));

	_structure = mathStruct;
	return true;
}

}
