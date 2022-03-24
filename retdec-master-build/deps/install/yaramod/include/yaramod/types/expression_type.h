/**
 * @file src/types/expression_type.h
 * @brief Declaration of class ExpressionType.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

namespace yaramod {

///< Type of the expression.
enum class ExpressionType
{
	Undefined,
	Bool,
	Int,
	String,
	Regexp,
	Object,
	Float
};

}
