/**
 * @file src/utils/observing_visitor.h
 * @brief Declaration of ObservingVisitor class.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/expressions.h"
#include "yaramod/types/regexp.h"
#include "yaramod/utils/visitor.h"

namespace yaramod {

/**
 * Abstract class representing observing visitor of condition expression in YARA files.
 * Its main purpose is just to traverse AST and collect information from it. Even though
 * each visit() method has return value same as with ModifyingVisitor it should be only
 * used to pass around information when collecting data, not directly for modification
 * of AST. If you implement all the logic of modification youself, you can do that but there
 * is no reason to do it on your own and not use ModifyingVisitor.
 */
class ObservingVisitor : public Visitor
{
public:
	/**
	 * Observes the specified expression.
	 */
	void observe(const Expression::Ptr& expr)
	{
		expr->accept(this);
	}

	/// @name Visit methods
	/// @{
	virtual VisitResult visit(StringExpression*) override { return {}; }
	virtual VisitResult visit(StringWildcardExpression*) override { return {}; }

	virtual VisitResult visit(StringAtExpression* expr) override
	{
		expr->getAtExpression()->accept(this);
		return {};
	}

	virtual VisitResult visit(StringInRangeExpression* expr) override
	{
		expr->getRangeExpression()->accept(this);
		return {};
	}

	virtual VisitResult visit(StringCountExpression*) override { return {}; }

	virtual VisitResult visit(StringOffsetExpression* expr) override
	{
		if (auto indexExpression = expr->getIndexExpression())
			indexExpression->accept(this);

		return {};
	}

	virtual VisitResult visit(StringLengthExpression* expr) override
	{
		if (auto indexExpression = expr->getIndexExpression())
			indexExpression->accept(this);

		return {};
	}

	virtual VisitResult visit(NotExpression* expr) override
	{
		expr->getOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(UnaryMinusExpression* expr) override
	{
		expr->getOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(BitwiseNotExpression* expr) override
	{
		expr->getOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(AndExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(OrExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(LtExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(GtExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(LeExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(GeExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(EqExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(NeqExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(ContainsExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(MatchesExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(PlusExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(MinusExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(MultiplyExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(DivideExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(ModuloExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(BitwiseXorExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(BitwiseAndExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(BitwiseOrExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(ShiftLeftExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(ShiftRightExpression* expr) override
	{
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		return {};
	}

	virtual VisitResult visit(ForIntExpression* expr) override
	{
		expr->getVariable()->accept(this);
		expr->getIteratedSet()->accept(this);
		expr->getBody()->accept(this);
		return {};
	}

	virtual VisitResult visit(ForStringExpression* expr) override
	{
		expr->getVariable()->accept(this);
		expr->getIteratedSet()->accept(this);
		expr->getBody()->accept(this);
		return {};
	}

	virtual VisitResult visit(OfExpression* expr) override
	{
		expr->getVariable()->accept(this);
		expr->getIteratedSet()->accept(this);
		return {};
	}

	virtual VisitResult visit(SetExpression* expr) override
	{
		for (auto& element : expr->getElements())
			element->accept(this);
		return {};
	}

	virtual VisitResult visit(RangeExpression* expr) override
	{
		expr->getLow()->accept(this);
		expr->getHigh()->accept(this);
		return {};
	}

	virtual VisitResult visit(IdExpression*) override { return {}; }

	virtual VisitResult visit(StructAccessExpression* expr) override
	{
		expr->getStructure()->accept(this);
		return {};
	}

	virtual VisitResult visit(ArrayAccessExpression* expr) override
	{
		expr->getArray()->accept(this);
		expr->getAccessor()->accept(this);
		return {};
	}

	virtual VisitResult visit(FunctionCallExpression* expr) override
	{
		expr->getFunction()->accept(this);
		for (auto& arg : expr->getArguments())
			arg->accept(this);
		return {};
	}

	virtual VisitResult visit(BoolLiteralExpression*) override { return {}; }
	virtual VisitResult visit(StringLiteralExpression*) override { return {}; }
	virtual VisitResult visit(IntLiteralExpression*) override { return {}; }
	virtual VisitResult visit(DoubleLiteralExpression*) override { return {}; }
	virtual VisitResult visit(FilesizeExpression*) override { return {}; }
	virtual VisitResult visit(EntrypointExpression*) override { return {}; }
	virtual VisitResult visit(AllExpression*) override { return {}; }
	virtual VisitResult visit(AnyExpression*) override { return {}; }
	virtual VisitResult visit(ThemExpression*) override { return {}; }

	virtual VisitResult visit(ParenthesesExpression* expr) override
	{
		expr->getEnclosedExpression()->accept(this);
		return {};
	}

	virtual VisitResult visit(IntFunctionExpression* expr) override
	{
		expr->getArgument()->accept(this);
		return {};
	}

	virtual VisitResult visit(RegexpExpression*) override { return {}; }
	/// @}

protected:
	ObservingVisitor() = default;
};

/**
 * Abstract class representing observing regexpVisitor of regular expression in YARA files.
 * Its main purpose is just to traverse units and collect information from them.
 */
class ObservingRegexpVisitor : public RegexpVisitor
{
public:
	/**
	 * Observes the specified expression.
	 */
	RegexpVisitResult observe(const std::shared_ptr<RegexpUnit>& unit)
	{
		return unit->accept(this);
	}

	/// @name Visit methods
	/// @{
	virtual RegexpVisitResult visit(RegexpClass*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpText*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpAnyChar*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpWordChar*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpNonWordChar*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpSpace*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpNonSpace*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpDigit*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpNonDigit*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpWordBoundary*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpNonWordBoundary*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpStartOfLine*) override { return {}; }
	virtual RegexpVisitResult visit(RegexpEndOfLine*) override { return {}; }

	virtual RegexpVisitResult visit(RegexpIteration* expr) override
	{
		expr->getOperand()->accept(this);
		return {};
	}

	virtual RegexpVisitResult visit(RegexpPositiveIteration* expr) override
	{
		expr->getOperand()->accept(this);
		return {};
	}

	virtual RegexpVisitResult visit(RegexpOptional* expr) override
	{
		expr->getOperand()->accept(this);
		return {};
	}

	virtual RegexpVisitResult visit(RegexpRange* expr) override
	{
		expr->getOperand()->accept(this);
		return {};
	}

	virtual RegexpVisitResult visit(RegexpOr* expr) override
	{
		expr->getLeft()->accept(this);
		expr->getRight()->accept(this);
		return {};
	}

	virtual RegexpVisitResult visit(RegexpGroup* expr) override
	{
		expr->getUnit()->accept(this);
		return {};
	}

	virtual RegexpVisitResult visit(RegexpConcat* expr) override
	{
		for (auto& element : expr->getUnits())
			element->accept(this);
		return {};
	}
	/// @}

protected:
	ObservingRegexpVisitor() = default;
};

} // namespace yaramod
