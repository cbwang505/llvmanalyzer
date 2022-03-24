/**
 * @file src/utils/visitor.h
 * @brief Declaration of Visitor class.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <variant>

#include "yaramod/utils/visitor_result.h"

namespace yaramod {

class StringExpression;
class StringWildcardExpression;
class StringAtExpression;
class StringInRangeExpression;
class StringCountExpression;
class StringOffsetExpression;
class StringLengthExpression;
class NotExpression;
class UnaryMinusExpression;
class BitwiseNotExpression;
class AndExpression;
class OrExpression;
class LtExpression;
class GtExpression;
class LeExpression;
class GeExpression;
class EqExpression;
class NeqExpression;
class ContainsExpression;
class MatchesExpression;
class PlusExpression;
class MinusExpression;
class MultiplyExpression;
class DivideExpression;
class ModuloExpression;
class BitwiseXorExpression;
class BitwiseAndExpression;
class BitwiseOrExpression;
class ShiftLeftExpression;
class ShiftRightExpression;
class ForIntExpression;
class ForStringExpression;
class OfExpression;
class SetExpression;
class RangeExpression;
class IdExpression;
class StructAccessExpression;
class ArrayAccessExpression;
class FunctionCallExpression;
class BoolLiteralExpression;
class StringLiteralExpression;
class IntLiteralExpression;
class DoubleLiteralExpression;
class FilesizeExpression;
class EntrypointExpression;
class AllExpression;
class AnyExpression;
class ThemExpression;
class ParenthesesExpression;
class IntFunctionExpression;
class RegexpExpression;

/**
 * Abstract class representing visitor design pattern for visiting condition expressions
 * in YARA file. Subclass this class whenever you want to somehow
 * process expression in conditions.
 */
class Visitor
{
public:
	/// @name Visit methods
	/// @{
	virtual VisitResult visit(StringExpression* expr) = 0;
	virtual VisitResult visit(StringWildcardExpression* expr) = 0;
	virtual VisitResult visit(StringAtExpression* expr) = 0;
	virtual VisitResult visit(StringInRangeExpression* expr) = 0;
	virtual VisitResult visit(StringCountExpression* expr) = 0;
	virtual VisitResult visit(StringOffsetExpression* expr) = 0;
	virtual VisitResult visit(StringLengthExpression* expr) = 0;
	virtual VisitResult visit(NotExpression* expr) = 0;
	virtual VisitResult visit(UnaryMinusExpression* expr) = 0;
	virtual VisitResult visit(BitwiseNotExpression* expr) = 0;
	virtual VisitResult visit(AndExpression* expr) = 0;
	virtual VisitResult visit(OrExpression* expr) = 0;
	virtual VisitResult visit(LtExpression* expr) = 0;
	virtual VisitResult visit(GtExpression* expr) = 0;
	virtual VisitResult visit(LeExpression* expr) = 0;
	virtual VisitResult visit(GeExpression* expr) = 0;
	virtual VisitResult visit(EqExpression* expr) = 0;
	virtual VisitResult visit(NeqExpression* expr) = 0;
	virtual VisitResult visit(ContainsExpression* expr) = 0;
	virtual VisitResult visit(MatchesExpression* expr) = 0;
	virtual VisitResult visit(PlusExpression* expr) = 0;
	virtual VisitResult visit(MinusExpression* expr) = 0;
	virtual VisitResult visit(MultiplyExpression* expr) = 0;
	virtual VisitResult visit(DivideExpression* expr) = 0;
	virtual VisitResult visit(ModuloExpression* expr) = 0;
	virtual VisitResult visit(BitwiseXorExpression* expr) = 0;
	virtual VisitResult visit(BitwiseAndExpression* expr) = 0;
	virtual VisitResult visit(BitwiseOrExpression* expr) = 0;
	virtual VisitResult visit(ShiftLeftExpression* expr) = 0;
	virtual VisitResult visit(ShiftRightExpression* expr) = 0;
	virtual VisitResult visit(ForIntExpression* expr) = 0;
	virtual VisitResult visit(ForStringExpression* expr) = 0;
	virtual VisitResult visit(OfExpression* expr) = 0;
	virtual VisitResult visit(SetExpression* expr) = 0;
	virtual VisitResult visit(RangeExpression* expr) = 0;
	virtual VisitResult visit(IdExpression* expr) = 0;
	virtual VisitResult visit(StructAccessExpression* expr) = 0;
	virtual VisitResult visit(ArrayAccessExpression* expr) = 0;
	virtual VisitResult visit(FunctionCallExpression* expr) = 0;
	virtual VisitResult visit(BoolLiteralExpression* expr) = 0;
	virtual VisitResult visit(StringLiteralExpression* expr) = 0;
	virtual VisitResult visit(IntLiteralExpression* expr) = 0;
	virtual VisitResult visit(DoubleLiteralExpression* expr) = 0;
	virtual VisitResult visit(FilesizeExpression* expr) = 0;
	virtual VisitResult visit(EntrypointExpression* expr) = 0;
	virtual VisitResult visit(AllExpression* expr) = 0;
	virtual VisitResult visit(AnyExpression* expr) = 0;
	virtual VisitResult visit(ThemExpression* expr) = 0;
	virtual VisitResult visit(ParenthesesExpression* expr) = 0;
	virtual VisitResult visit(IntFunctionExpression* expr) = 0;
	virtual VisitResult visit(RegexpExpression* expr) = 0;
	/// @}
	bool resultIsDelete(const VisitResult& result) const
	{
		if (!std::holds_alternative<VisitAction>(result))
			return false;
		if (std::get<VisitAction>(result) != VisitAction::Delete)
			return false;
		return true;
	}
	bool resultIsModified(const VisitResult& result) const
	{
		if (std::holds_alternative<std::shared_ptr<Expression>>(result))
			return std::get<std::shared_ptr<Expression>>(result) != nullptr;
		return true;
	}
};

class RegexpClass;
class RegexpText;
class RegexpAnyChar;
class RegexpWordChar;
class RegexpNonWordChar;
class RegexpSpace;
class RegexpNonSpace;
class RegexpDigit;
class RegexpNonDigit;
class RegexpWordBoundary;
class RegexpNonWordBoundary;
class RegexpStartOfLine;
class RegexpEndOfLine;
class RegexpIteration;
class RegexpPositiveIteration;
class RegexpOptional;
class RegexpRange;
class RegexpOr;
class RegexpGroup;
class RegexpConcat;

/**
 * Abstract class representing visitor design pattern for visiting regular expressions
 * in YARA file. Subclass this class whenever you want to somehow process units
 * in regular expression.
 */
class RegexpVisitor
{
public:
	/// @name Visit methods
	/// @{
	virtual RegexpVisitResult visit(RegexpClass* expr) = 0;
	virtual RegexpVisitResult visit(RegexpText* expr) = 0;
	virtual RegexpVisitResult visit(RegexpAnyChar* expr) = 0;
	virtual RegexpVisitResult visit(RegexpWordChar* expr) = 0;
	virtual RegexpVisitResult visit(RegexpNonWordChar* expr) = 0;
	virtual RegexpVisitResult visit(RegexpSpace* expr) = 0;
	virtual RegexpVisitResult visit(RegexpNonSpace* expr) = 0;
	virtual RegexpVisitResult visit(RegexpDigit* expr) = 0;
	virtual RegexpVisitResult visit(RegexpNonDigit* expr) = 0;
	virtual RegexpVisitResult visit(RegexpWordBoundary* expr) = 0;
	virtual RegexpVisitResult visit(RegexpNonWordBoundary* expr) = 0;
	virtual RegexpVisitResult visit(RegexpStartOfLine* expr) = 0;
	virtual RegexpVisitResult visit(RegexpEndOfLine* expr) = 0;
	virtual RegexpVisitResult visit(RegexpIteration* expr) = 0;
	virtual RegexpVisitResult visit(RegexpPositiveIteration* expr) = 0;
	virtual RegexpVisitResult visit(RegexpOptional* expr) = 0;
	virtual RegexpVisitResult visit(RegexpRange* expr) = 0;
	virtual RegexpVisitResult visit(RegexpOr* expr) = 0;
	virtual RegexpVisitResult visit(RegexpGroup* expr) = 0;
	virtual RegexpVisitResult visit(RegexpConcat* expr) = 0;
	/// @}
};

}
