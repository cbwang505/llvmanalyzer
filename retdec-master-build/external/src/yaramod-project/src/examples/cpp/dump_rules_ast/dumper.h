/**
 * @file src/examples/dump_rules_ast/dumper.h
 * @brief Implementation of AST dumper.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <iostream>

#include "yaramod/utils/observing_visitor.h"

class Dumper : public yaramod::ObservingVisitor, public yaramod::ObservingRegexpVisitor
{
public:
	Dumper() : _indent(0) {}

	void observe(const yaramod::Expression::Ptr& expr)
	{
		expr->accept(this);
	}

	virtual yaramod::VisitResult visit(yaramod::StringExpression* expr) override
	{
		dump("String", expr, " id=", expr->getId());
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::StringWildcardExpression* expr) override
	{
		dump("StringWildcard", expr, " id=", expr->getId());
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::StringAtExpression* expr) override
	{
		dump("StringAt", expr, " id=", expr->getId());
		indentUp();
		expr->getAtExpression()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::StringInRangeExpression* expr) override
	{
		dump("StringInRange", expr, " id=", expr->getId());
		indentUp();
		expr->getRangeExpression()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::StringCountExpression* expr) override
	{
		dump("StringCount", expr, " id=", expr->getId());
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::StringOffsetExpression* expr) override
	{
		dump("StringOffset", expr, " id=", expr->getId());
		if (auto indexExpression = expr->getIndexExpression())
		{
			indentUp();
			indexExpression->accept(this);
			indentDown();
		}

		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::StringLengthExpression* expr) override
	{
		dump("StringLength", expr, " id=", expr->getId());
		if (auto indexExpression = expr->getIndexExpression())
		{
			indentUp();
			indexExpression->accept(this);
			indentDown();
		}

		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::NotExpression* expr) override
	{
		dump("Not", expr);
		indentUp();
		expr->getOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::UnaryMinusExpression* expr) override
	{
		dump("UnaryMinus", expr);
		indentUp();
		expr->getOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::BitwiseNotExpression* expr) override
	{
		dump("BitwiseNot", expr);
		indentUp();
		expr->getOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::AndExpression* expr) override
	{
		dump("And", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::OrExpression* expr) override
	{
		dump("Or", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::LtExpression* expr) override
	{
		dump("LessThan", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::GtExpression* expr) override
	{
		dump("GreaterThan", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::LeExpression* expr) override
	{
		dump("LessThanOrEqual", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::GeExpression* expr) override
	{
		dump("GreaterThanOrEqual", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::EqExpression* expr) override
	{
		dump("Equal", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::NeqExpression* expr) override
	{
		dump("NotEqual", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ContainsExpression* expr) override
	{
		dump("Contains", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::MatchesExpression* expr) override
	{
		dump("Matches", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::PlusExpression* expr) override
	{
		dump("Plus", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::MinusExpression* expr) override
	{
		dump("Minus", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::MultiplyExpression* expr) override
	{
		dump("Multiply", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::DivideExpression* expr) override
	{
		dump("Divide", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ModuloExpression* expr) override
	{
		dump("Modulo", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::BitwiseXorExpression* expr) override
	{
		dump("BitwiseXor", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::BitwiseAndExpression* expr) override
	{
		dump("BitwiseAnd", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::BitwiseOrExpression* expr) override
	{
		dump("BitwiseOr", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ShiftLeftExpression* expr) override
	{
		dump("ShiftLeft", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ShiftRightExpression* expr) override
	{
		dump("ShiftRight", expr);
		indentUp();
		expr->getLeftOperand()->accept(this);
		expr->getRightOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ForIntExpression* expr) override
	{
		dump("ForInt", expr, " id=", expr->getId());
		indentUp();
		expr->getVariable()->accept(this);
		expr->getIteratedSet()->accept(this);
		expr->getBody()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ForStringExpression* expr) override
	{
		dump("ForString", expr);
		indentUp();
		expr->getVariable()->accept(this);
		expr->getIteratedSet()->accept(this);
		expr->getBody()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::OfExpression* expr) override
	{
		dump("Of", expr);
		indentUp();
		expr->getVariable()->accept(this);
		expr->getIteratedSet()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::SetExpression* expr) override
	{
		dump("Set", expr, " size=", expr->getElements().size());
		indentUp();
		for (auto& elem : expr->getElements())
			elem->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::RangeExpression* expr) override
	{
		dump("Range", expr);
		indentUp();
		expr->getLow()->accept(this);
		expr->getHigh()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::IdExpression* expr) override
	{
		dump("Id", expr, " id=", expr->getSymbol()->getName());
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::StructAccessExpression* expr) override
	{
		dump("StructAccess", expr, " id=", expr->getSymbol()->getName());
		indentUp();
		expr->getStructure()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ArrayAccessExpression* expr) override
	{
		dump("ArrayAccess", expr, " id=", expr->getSymbol()->getName());
		indentUp();

		dump("[array]", expr);
		indentUp();
		expr->getArray()->accept(this);
		indentDown();

		dump("[accessor]", expr);
		indentUp();
		expr->getAccessor()->accept(this);
		indentDown();

		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::FunctionCallExpression* expr) override
	{
		dump("FunctionCall", expr, " args_count=", expr->getArguments().size());
		indentUp();

		dump("[symbol]", expr);
		indentUp();
		expr->getFunction()->accept(this);
		indentDown();

		dump("[args]", expr);
		indentUp();
		for (auto& arg : expr->getArguments())
			arg->accept(this);
		indentDown();

		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::BoolLiteralExpression* expr) override
	{
		dump("BoolLiteral", expr, " value=", expr->getText());
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::StringLiteralExpression* expr) override
	{
		dump("StringLiteral", expr, " value=", expr->getText());
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::IntLiteralExpression* expr) override
	{
		dump("IntLiteral", expr, " value=", expr->getText());
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::DoubleLiteralExpression* expr) override
	{
		dump("DoubleLiteral", expr, " value=", expr->getText());
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::FilesizeExpression* expr) override
	{
		dump("Filesize", expr);
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::EntrypointExpression* expr) override
	{
		dump("Entrypoint", expr);
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::AllExpression* expr) override
	{
		dump("All", expr);
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::AnyExpression* expr) override
	{
		dump("Any", expr);
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ThemExpression* expr) override
	{
		dump("Them", expr);
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::ParenthesesExpression* expr) override
	{
		dump("Parentheses", expr);
		indentUp();
		expr->getEnclosedExpression()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::IntFunctionExpression* expr) override
	{
		dump("IntFunction", expr, " function=", expr->getFunction());
		indentUp();
		expr->getArgument()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::VisitResult visit(yaramod::RegexpExpression* expr) override
	{
		dump("Regexp", expr, " text=", expr->getRegexpString()->getPureText());
		indentUp();
		observe(std::static_pointer_cast<yaramod::Regexp>(expr->getRegexpString())->getUnit());
		indentDown();
		return {};
	}

	// ==================== ObservingRegexVisitor ====================
	yaramod::RegexpVisitResult observe(const std::shared_ptr<yaramod::RegexpUnit>& unit)
	{
		unit->accept(this);
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpClass* expr) override
	{
		dump("RegexpClass", expr, " text=", expr->getText(), " characters=", expr->getCharacters());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpText* expr) override
	{
		dump("RegexpClass", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpAnyChar* expr) override
	{
		dump("RegexpAnyChar", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpWordChar* expr) override
	{
		dump("RegexpWordChar", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpNonWordChar* expr) override
	{
		dump("RegexpNonWordChar", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpSpace* expr) override
	{
		dump("RegexpSpace", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpNonSpace* expr) override
	{
		dump("RegexpNonSpace", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpDigit* expr) override
	{
		dump("RegexpDigit", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpNonDigit* expr) override
	{
		dump("RegexpNonDigit", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpWordBoundary* expr) override
	{
		dump("RegexpWordBoundary", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpNonWordBoundary* expr) override
	{
		dump("RegexpNonWordBoundary", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpStartOfLine* expr) override
	{
		dump("RegexpStartOfLine", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpEndOfLine* expr) override
	{
		dump("RegexpEndOfLine", expr, " text=", expr->getText());
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpIteration* expr) override
	{
		dump("RegexpIteration", expr, " text=", expr->getText());
		indentUp();
		expr->getOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpPositiveIteration* expr) override
	{
		dump("RegexpPositiveIteration", expr, " text=", expr->getText());
		indentUp();
		expr->getOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpOptional* expr) override
	{
		dump("RegexpOptional", expr, " text=", expr->getText());
		indentUp();
		expr->getOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpRange* expr) override
	{
		dump("RegexpRange", expr, " text=", expr->getText());
		indentUp();
		expr->getOperand()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpOr* expr) override
	{
		dump("RegexpOr", expr, " text=", expr->getText());
		indentUp();
		expr->getLeft()->accept(this);
		expr->getRight()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpGroup* expr) override
	{
		dump("RegexpGroup", expr, " text=", expr->getText());
		indentUp();
		expr->getUnit()->accept(this);
		indentDown();
		return {};
	}

	virtual yaramod::RegexpVisitResult visit(yaramod::RegexpConcat* expr) override
	{
		dump("RegexpConcat", expr, " text=", expr->getText());
		indentUp();
		for (auto& unit : expr->getUnits())
			unit->accept(this);
		indentDown();
		return {};
	}

private:
	void indentUp() { _indent += 4; }
	void indentDown() { _indent -= 4; }

	template <typename T, typename... Args>
	void dump(const std::string& name, T* expr, Args&&... args)
	{
		std::cout << std::string(_indent, ' ') << name << "[" << expr << "] ";
		dump_helper(args...);
	}

	void dump_helper()
	{
		std::cout << std::endl;
	}

	template <typename T, typename... Args>
	void dump_helper(T&& val, Args&&... args)
	{
		std::cout << val;
		dump_helper(args...);
	}

	std::uint32_t _indent;
};
