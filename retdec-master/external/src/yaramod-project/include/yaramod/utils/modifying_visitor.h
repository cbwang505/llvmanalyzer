/**
 * @file src/utils/modifying_visitor.h
 * @brief Declaration of ModifyingVisitor class.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/builder/yara_expression_builder.h"
#include "yaramod/types/expressions.h"
#include "yaramod/utils/visitor.h"

namespace yaramod {

/**
 * Class capturing current TokenStream and first and last TokenIi of the expression which it is given
 * in it's constructor. This data can be very useful at the end of the visit method of ModifyingVisitors
 * by the cleanUpTokenStreams function.
 */
class TokenStreamContext
{
public:
	TokenStreamContext(Expression* expr)
		: _oldTokenStream(expr->getTokenStreamSharedPtr())
		, _oldBeforeFirst(std::prev(expr->getFirstTokenIt()))
		, _oldAfterLast(std::next(expr->getLastTokenIt()))
	{
	}
	TokenStreamContext(const std::shared_ptr<TokenStream>& oldTokenStream, TokenIt oldBeforeFirst, TokenIt oldAfterLast)
		: _oldTokenStream(oldTokenStream)
		, _oldBeforeFirst(oldBeforeFirst)
		, _oldAfterLast(oldAfterLast)
	{
	}

	/// @name Getter methods
	/// @{
	const std::shared_ptr<TokenStream>& oldTokenStream() const { return _oldTokenStream; }
	TokenIt oldBeforeFirst() const { return _oldBeforeFirst; }
	TokenIt oldAfterLast() const { return _oldAfterLast; }
	/// @}

private:
	std::shared_ptr<TokenStream> _oldTokenStream;
	TokenIt _oldBeforeFirst;
	TokenIt _oldAfterLast;
};

/**
 * Abstract class representing modifying visitor of condition expression in YARA files.
 * It is capable of modifying AST. Each visit() method has return value of
 * @c VisitResult what is just an alis for `variant<shared_ptr<Expression>, Visitee::Action>`.
 * There are 3 possible values you can return in order to control the behavior of the visitor:
 *
 * 1. nullptr - Returning unset variant means keeping the node as it is and not modifying it all.
 * 2. non nullptr - Returning valid instance of Expression means replace this AST node with the one I returned you.
 * 3. VisitAction::Delete - Delete this AST node.
 *
 * You can of course override this behavior by providing your own implementaion of each visit()
 * method but this is the default one. If you want to override some part of the logic but fall-back
 * to the default handling if the AST node does not meet your criteria, you can do that by calling
 * defaultHandler() and passing it the current expression together with results from visited child nodes.
 * Not every type of expression has defaultHandler() and you will get compilation error if you
 * try to call one if it does not exist.
 */
class ModifyingVisitor : public Visitor
{
public:
	/**
	 * Runs the modifier on the provided expression. Returns the valid Expression object.
	 * If the AST node should be replaced by some other expression, the new expression is returned.
	 * The same expression as originally passed is returned in case of no change to the current AST node.
	 * If the AST node should be deleted, `whenDeleted` is returned.
	 *
	 * @return New expresion or `whenDeleted`.
	 */
	Expression::Ptr modify(const Expression::Ptr& expr, Expression::Ptr whenDeleted = nullptr)
	{
		TokenStreamContext context{expr.get()};
		
		auto result = expr->accept(this);
		if (auto newExpr = std::get_if<Expression::Ptr>(&result))
		{
			return *newExpr ? *newExpr : expr;
		}
		else
		{
			cleanUpTokenStreams(context, whenDeleted.get());
			return whenDeleted;
		}
	}

	/// @name Visit methods
	/// @{
	virtual VisitResult visit(StringExpression*) override { return {}; }
	virtual VisitResult visit(StringWildcardExpression*) override { return {}; }

	virtual VisitResult visit(StringAtExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto atExpr = expr->getAtExpression()->accept(this);
		return defaultHandler(context, expr, atExpr);
	}

	virtual VisitResult visit(StringInRangeExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto rangeExpr = expr->getRangeExpression()->accept(this);
		return defaultHandler(context, expr, rangeExpr);
	}

	virtual VisitResult visit(StringCountExpression*) override { return {}; }

	virtual VisitResult visit(StringOffsetExpression* expr) override
	{
		TokenStreamContext context{expr};
		if (expr->getIndexExpression())
		{
			auto indexExpr = expr->getIndexExpression()->accept(this);
			return defaultHandler(context, expr, indexExpr);
		}

		return defaultHandler(context, expr, {});
	}

	virtual VisitResult visit(StringLengthExpression* expr) override
	{
		TokenStreamContext context{expr};
		if (expr->getIndexExpression())
		{
			auto indexExpr = expr->getIndexExpression()->accept(this);
			return defaultHandler(context, expr, indexExpr);
		}

		return defaultHandler(context, expr, {});
	}

	virtual VisitResult visit(NotExpression* expr) override
	{
		return _handleUnaryOperation(expr);
	}

	virtual VisitResult visit(UnaryMinusExpression* expr) override
	{
		return _handleUnaryOperation(expr);
	}

	virtual VisitResult visit(BitwiseNotExpression* expr) override
	{
		return _handleUnaryOperation(expr);
	}

	virtual VisitResult visit(AndExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(OrExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(LtExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(GtExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(LeExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(GeExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(EqExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(NeqExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(ContainsExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(MatchesExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(PlusExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(MinusExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(MultiplyExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(DivideExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(ModuloExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(BitwiseXorExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(BitwiseAndExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(BitwiseOrExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(ShiftLeftExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(ShiftRightExpression* expr) override
	{
		return _handleBinaryOperation(expr);
	}

	virtual VisitResult visit(ForIntExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto var = expr->getVariable()->accept(this);
		auto iteratedSet = expr->getIteratedSet()->accept(this);
		auto body = expr->getBody()->accept(this);
		return defaultHandler(context, expr, var, iteratedSet, body);
	}

	virtual VisitResult visit(ForStringExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto var = expr->getVariable()->accept(this);
		auto iteratedSet = expr->getIteratedSet()->accept(this);
		auto body = expr->getBody()->accept(this);
		return defaultHandler(context, expr, var, iteratedSet, body);
	}

	virtual VisitResult visit(OfExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto var = expr->getVariable()->accept(this);
		auto iteratedSet = expr->getIteratedSet()->accept(this);
		return defaultHandler(context, expr, var, iteratedSet, {});
	}

	virtual VisitResult visit(SetExpression* expr) override
	{
		TokenStreamContext context{expr};
		std::vector<VisitResult> newElements;
		for (auto& element : expr->getElements())
			newElements.push_back(element->accept(this));

		return defaultHandler(context, expr, newElements);
	}

	virtual VisitResult visit(RangeExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto low = expr->getLow()->accept(this);
		auto high = expr->getHigh()->accept(this);
		return defaultHandler(context, expr, low, high);
	}

	virtual VisitResult visit(IdExpression*) override { return {}; }

	virtual VisitResult visit(StructAccessExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto structure = expr->getStructure()->accept(this);
		return defaultHandler(context, expr, structure);
	}

	virtual VisitResult visit(ArrayAccessExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto array = expr->getArray()->accept(this);
		auto accessor = expr->getAccessor()->accept(this);
		return defaultHandler(context, expr, array, accessor);
	}

	virtual VisitResult visit(FunctionCallExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto function = expr->getFunction()->accept(this);

		std::vector<VisitResult> arguments;
		for (auto& arg : expr->getArguments())
		{
			arguments.push_back(arg->accept(this));
		}

		return defaultHandler(context, expr, function, arguments);
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
		TokenStreamContext context{expr};
		auto enclosedExpr = expr->getEnclosedExpression()->accept(this);
		return defaultHandler(context, expr, enclosedExpr);
	}

	virtual VisitResult visit(IntFunctionExpression* expr) override
	{
		TokenStreamContext context{expr};
		auto argument = expr->getArgument()->accept(this);
		return defaultHandler(context, expr, argument);
	}

	virtual VisitResult visit(RegexpExpression*) override { return {}; }
	/// @}

	/// @name Default handlers
	/**
	 * If a handler gets a Delete result from a subexpression of handled expression, it has two options:
	 * A. Take care of the deletion of the expression and also it's tokens.
	 * B. Emit Delete result so the whole expression will be deleted by the caller.
	 */
	/// @{
	VisitResult defaultHandler(const TokenStreamContext& context, StringAtExpression* expr, const VisitResult& atExprRet)
	{
		if (auto atExpr = std::get_if<Expression::Ptr>(&atExprRet))
		{
			if (*atExpr)
				expr->setAtExpression(*atExpr);
		}
		else
			expr->setAtExpression(nullptr);

		if (!expr->getAtExpression())
			return VisitAction::Delete;

		return {};
	}

	VisitResult defaultHandler(const TokenStreamContext& context, StringInRangeExpression* expr, const VisitResult& rangeExprRet)
	{
		if (auto rangeExpr = std::get_if<Expression::Ptr>(&rangeExprRet))
		{
			if (*rangeExpr)
				expr->setRangeExpression(*rangeExpr);
		}
		else
			expr->setRangeExpression(nullptr);

		if (!expr->getRangeExpression())
			return VisitAction::Delete;

		return {};
	}

	template <typename T>
	std::enable_if_t<isAnyOf<T, StringOffsetExpression, StringLengthExpression>::value, VisitResult>
		defaultHandler(const TokenStreamContext& context, T* expr, const VisitResult& indexExprRet)
	{
		if (auto indexExpr = std::get_if<Expression::Ptr>(&indexExprRet))
		{
			if (*indexExpr)
				expr->setIndexExpression(*indexExpr);
		}
		else
			expr->setIndexExpression(nullptr);

		return {};
	}

	template <typename T>
	std::enable_if_t<std::is_base_of<UnaryOpExpression, T>::value, VisitResult>
		defaultHandler(const TokenStreamContext& context, T* expr, const VisitResult& operandRet)
	{
		if (auto operand = std::get_if<Expression::Ptr>(&operandRet))
		{
			if (*operand)
			{
				expr->setOperand(*operand);
			}
		}
		else
		{
			return VisitAction::Delete;
		}

		return {};
	}

	template <typename T>
	std::enable_if_t<std::is_base_of<BinaryOpExpression, T>::value, VisitResult>
		defaultHandler(const TokenStreamContext& context, T* expr, const VisitResult& leftRet, const VisitResult& rightRet)
	{
		if (auto left = std::get_if<Expression::Ptr>(&leftRet))
		{
			if (*left)
				expr->setLeftOperand(*left);
		}
		else
			expr->setLeftOperand(nullptr);

		if (auto right = std::get_if<Expression::Ptr>(&rightRet))
		{
			if (*right)
				expr->setRightOperand(*right);
		}
		else
			expr->setRightOperand(nullptr);

		if (!expr->getLeftOperand() && !expr->getRightOperand())
			return VisitAction::Delete;
		else if (!expr->getLeftOperand() && expr->getRightOperand())
		{
			auto output = YaraExpressionBuilder{expr->getRightOperand()}.get();
			cleanUpTokenStreams(context, output.get());
			return output;
		}
		else if (expr->getLeftOperand() && !expr->getRightOperand())
		{
			auto output = YaraExpressionBuilder{expr->getLeftOperand()}.get();
			cleanUpTokenStreams(context, output.get());
			return output;
		}

		return {};
	}

	template <typename T>
	std::enable_if_t<std::is_base_of<ForExpression, T>::value, VisitResult>
		defaultHandler(const TokenStreamContext& context, T* expr, const VisitResult& varRet, const VisitResult& iteratedSetRet, const VisitResult& bodyRet)
	{
		if (auto var = std::get_if<Expression::Ptr>(&varRet))
		{
			if (*var)
				expr->setVariable(*var);
		}
		else
			expr->setVariable(nullptr);

		if (auto iteratedSet = std::get_if<Expression::Ptr>(&iteratedSetRet))
		{
			if (*iteratedSet)
				expr->setIteratedSet(*iteratedSet);
		}
		else
			expr->setIteratedSet(nullptr);

		auto oldBody = expr->getBody();
		if (auto body = std::get_if<Expression::Ptr>(&bodyRet))
		{
			if (*body)
				expr->setBody(*body);
		}
		else
			expr->setBody(nullptr);

		// If any subnode needs to be deleted, we delete whole expr.
		if (!expr->getVariable() || !expr->getIteratedSet() || (oldBody && !expr->getBody()))
			return VisitAction::Delete;

		return {};
	}

	VisitResult defaultHandler(const TokenStreamContext& context, SetExpression* expr, const std::vector<VisitResult>& elementsRet)
	{
		if (elementsRet.empty())
			return VisitAction::Delete;

		if (std::all_of(elementsRet.begin(), elementsRet.end(),
				[](const auto& element) {
					auto e = std::get_if<Expression::Ptr>(&element);
					return e && (*e == nullptr);
				}))
		{
			return {};
		}

		std::vector<Expression::Ptr> newElements;
		for (std::size_t i = 0, end = elementsRet.size(); i < end; ++i)
		{
			if (auto element = std::get_if<Expression::Ptr>(&elementsRet[i]))
			{
				if (*element)
					newElements.push_back(*element);
				else
					newElements.push_back(expr->getElements()[i]);
			}
		}

		if (newElements.empty())
			return VisitAction::Delete;

		expr->setElements(newElements);
		return {};
	}

	VisitResult defaultHandler(const TokenStreamContext& context, RangeExpression* expr, const VisitResult& lowRet, const VisitResult& highRet)
	{
		if (auto low = std::get_if<Expression::Ptr>(&lowRet))
		{
			if (*low)
				expr->setLow(*low);
		}
		else
			expr->setLow(nullptr);

		if (auto high = std::get_if<Expression::Ptr>(&highRet))
		{
			if (*high)
				expr->setHigh(*high);
		}
		else
			expr->setHigh(nullptr);

		if (!expr->getLow() || !expr->getHigh())
			return VisitAction::Delete;

		return {};
	}

	VisitResult defaultHandler(const TokenStreamContext& context, StructAccessExpression* expr, const VisitResult& structureRet)
	{
		if (auto structure = std::get_if<Expression::Ptr>(&structureRet))
		{
			if (*structure)
				expr->setStructure(*structure);
		}
		else
			return VisitAction::Delete;

		return {};
	}

	VisitResult defaultHandler(const TokenStreamContext& context, ArrayAccessExpression* expr, const VisitResult& arrayRet, const VisitResult& accessorRet)
	{
		if (auto array = std::get_if<Expression::Ptr>(&arrayRet))
		{
			if (*array)
				expr->setArray(*array);
		}
		else
			expr->setArray(nullptr);

		if (auto accessor = std::get_if<Expression::Ptr>(&accessorRet))
		{
			if (*accessor)
				expr->setAccessor(*accessor);
		}
		else
			expr->setAccessor(nullptr);

		if (!expr->getArray() || !expr->getAccessor())
			return VisitAction::Delete;

		return {};
	}

	VisitResult defaultHandler(const TokenStreamContext& context, FunctionCallExpression* expr, const VisitResult& functionRet, const std::vector<VisitResult>& argumentsRet)
	{
		if (auto function = std::get_if<Expression::Ptr>(&functionRet))
		{
			if (*function)
				expr->setFunction(*function);
		}
		else
			expr->setFunction(nullptr);

		if (std::all_of(argumentsRet.begin(), argumentsRet.end(),
				[](const auto& arg) {
					auto a = std::get_if<Expression::Ptr>(&arg);
					return a && (*a == nullptr);
				}))
		{
			return {};
		}

		std::vector<Expression::Ptr> newArguments;
		for (std::size_t i = 0, end = argumentsRet.size(); i < end; ++i)
		{
			if (auto arg = std::get_if<Expression::Ptr>(&argumentsRet[i]))
			{
				if (*arg)
					newArguments.push_back(*arg);
				else
					newArguments.push_back(expr->getArguments()[i]);
			}
		}

		expr->setArguments(newArguments);
		return {};
	}

	VisitResult defaultHandler(const TokenStreamContext& context, ParenthesesExpression* expr, const VisitResult& enclosedExprRet)
	{
		if (auto enclosedExpr = std::get_if<Expression::Ptr>(&enclosedExprRet))
		{
			if (*enclosedExpr)
				expr->setEnclosedExpression(*enclosedExpr);
		}
		else
			expr->setEnclosedExpression(nullptr);

		if (!expr->getEnclosedExpression())
			return VisitAction::Delete;

		return {};
	}

	VisitResult defaultHandler(const TokenStreamContext& context, IntFunctionExpression* expr, const VisitResult& argumentRet)
	{
		if (auto argument = std::get_if<Expression::Ptr>(&argumentRet))
		{
			if (*argument)
				expr->setArgument(*argument);
		}
		else
			expr->setArgument(nullptr);

		if (!expr->getArgument())
			return VisitAction::Delete;

		return {};
	}
	/// @}

	/**
	 * In the case an expression has been created in a builder, it has a new TokenStream which may partially
	 * contain some Tokens stolen from the original TS. In this method, the new TokenStream is move appended
	 * inside the original TS and replaces all remaining tokens which were associated with the old version
	 * of the expression.
	 * 
	 * Removes all tokens in the TokenStream that the given expression has been associated with.
	 * Then all tokens, which are currently associated with the given expression are moved to the TokenStream
	 * The expression is assigned the TokenStream.
	 * @return true iff any changes performed (when the expression has different tokenstream)
	 */
	bool cleanUpTokenStreams(const TokenStreamContext& context, Expression* new_expression)
	{
		auto& oldTokenStream = context.oldTokenStream();
		if (oldTokenStream.get() != new_expression->getTokenStream())
		{
			auto oldBeforeFirst = context.oldBeforeFirst();
			auto oldAfterLast = context.oldAfterLast();
			// remove old tokens which has not been moved away by builder
			oldTokenStream->erase(std::next(oldBeforeFirst), oldAfterLast);
			// transfer builded tokens
			oldTokenStream->moveAppend(oldAfterLast, new_expression->getTokenStream());
			new_expression->setTokenStream(oldTokenStream);
			return true;
		}
		else
			return false;
	}

protected:
	ModifyingVisitor() = default;

private:
	template <typename T>
	VisitResult _handleUnaryOperation(T* expr)
	{
		TokenStreamContext context{expr};
		auto operandRet = expr->getOperand()->accept(this);
		return defaultHandler(context, expr, operandRet);
	}

	template <typename T>
	VisitResult _handleBinaryOperation(T* expr)
	{
		
		TokenStreamContext context{expr};
		auto leftOperand = expr->getLeftOperand()->accept(this);
		auto rightOperand = expr->getRightOperand()->accept(this);
		return defaultHandler(context, expr, leftOperand, rightOperand);
	}
};

} // namespace yaramod
