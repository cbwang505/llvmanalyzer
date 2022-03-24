/**
 * @file src/types/expressions.h
 * @brief Declaration of all Expression subclasses.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once
#include <sstream>

#include "yaramod/types/expression.h"
#include "yaramod/types/string.h"
#include "yaramod/types/symbol.h"
#include "yaramod/utils/utils.h"
#include "yaramod/utils/visitor.h"

namespace yaramod {

/**
 * Class representing expression which references string defined
 * in the strings section of the YARA rule.
 *
 * For example:
 * @code
 * $str at entrypoint
 * ^^^^
 * @endcode
 */
class StringExpression : public Expression
{
public:
	StringExpression(const std::string& id) { _id = _tokenStream->emplace_back(STRING_ID, id); }
	StringExpression(std::string&& id) { _id = _tokenStream->emplace_back(STRING_ID, std::move(id)); }
	StringExpression(TokenIt id) : _id(id) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	std::string getId() const { return _id->getPureText(); }

	void setId(const std::string& id) { _id->setValue(id); }
	void setId(std::string&& id) { _id->setValue(std::move(id)); }

	virtual TokenIt getFirstTokenIt() const override { return _id; }
	virtual TokenIt getLastTokenIt() const override { return _id; }

	virtual std::string getText(const std::string& /*indent*/ = std::string{}) const override
	{
		return getId();
	}

private:
	TokenIt _id; ///< Identifier of the string, std::string
};

/**
 * Class representing expression which references string using wildcard.
 * This is usable only in string sets used in string-based for loops.
 *
 * For example:
 * @code
 * for any of ($a*) : ( $ at entrypoint )
 *             ^^^
 * @endcode
 */
class StringWildcardExpression : public Expression
{
public:
	template <typename Str>
	StringWildcardExpression(Str&& id)
	{
		_id = _tokenStream->emplace_back(STRING_ID, std::forward<Str>(id));
	}
	StringWildcardExpression(TokenIt it) : _id(it) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	const std::string& getId() const { return _id->getString(); }

	void setId(const std::string& id) { _id->setValue(id); }
	void setId(std::string&& id) { _id->setValue(std::move(id)); }

	virtual TokenIt getFirstTokenIt() const override { return _id; }
	virtual TokenIt getLastTokenIt() const override { return _id; }

	virtual std::string getText(const std::string& /*indent*/ = std::string{}) const override
	{
		return getId();
	}

private:
	TokenIt _id; ///< Wildcard identifier of the string
};

/**
 * Class representing expression which references string at certain integer offset.
 *
 * For example:
 * @code
 * $str at 0x100
 * @endcode
 */
class StringAtExpression : public Expression
{
public:
	template <typename ExpPtr>
	StringAtExpression(const std::string& id, ExpPtr&& at)
		: _at(std::forward<ExpPtr>(at))
	{
		_id = _tokenStream->emplace_back(STRING_ID, id);
		_at_symbol = _tokenStream->emplace_back(OP_AT, "at");
		_tokenStream->moveAppend(_at->getTokenStream());
	}

	template <typename ExpPtr>
	StringAtExpression(TokenIt id, TokenIt at_symbol, ExpPtr&& at)
		: _id(id)
		, _at_symbol(at_symbol)
		, _at(std::forward<ExpPtr>(at))
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	std::string getId() const { return _id->getPureText(); }
	const Expression::Ptr& getAtExpression() const { return _at; }

	void setId(const std::string& id) { _id->setValue(id); }
	void setId(std::string&& id) { _id->setValue(std::move(id)); }
	void setAtExpression(const Expression::Ptr& at) { _at = at; }
	void setAtExpression(Expression::Ptr&& at) { _at = std::move(at); }

	virtual TokenIt getFirstTokenIt() const override { return _id; }
	virtual TokenIt getLastTokenIt() const override { return _at->getLastTokenIt(); }

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		return getId() + " " + _at_symbol->getString() + " " + _at->getText(indent);
	}

private:
	TokenIt _id; ///< Identifier of the string
	TokenIt _at_symbol; ///< Token holding "at"
	Expression::Ptr _at; ///< Integer part of the expression
};

/**
 * Class representing expression which references string in certain integer range.
 *
 * For example:
 * @code
 * $str in (0x100 .. 0x200)
 * @endcode
 */
class StringInRangeExpression : public Expression
{
public:
	template <typename ExpPtr>
	StringInRangeExpression(const std::string& id, ExpPtr&& range)
	{
		_id = _tokenStream->emplace_back(STRING_ID, id);
		_in_symbol = _tokenStream->emplace_back(OP_IN, "in");
		_range = std::forward<ExpPtr>(range);
		_tokenStream->moveAppend(_range->getTokenStream());
	}

	template <typename ExpPtr>
	StringInRangeExpression(TokenIt id, TokenIt in_symbol, ExpPtr&& range)
		: _id(id)
		, _in_symbol(in_symbol)
		, _range(std::forward<ExpPtr>(range))
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	std::string getId() const { return _id->getPureText(); }
	const Expression::Ptr& getRangeExpression() const { return _range; }

	void setId(const std::string& id) { _id->setValue(id); }
	void setId(std::string&& id) { _id->setValue(std::move(id)); }
	void setRangeExpression(const Expression::Ptr& range) { _range = range; }
	void setRangeExpression(Expression::Ptr&& range) { _range = std::move(range); }

	virtual TokenIt getFirstTokenIt() const override { return _id; }
	virtual TokenIt getLastTokenIt() const override { return _range->getLastTokenIt(); }

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		return getId() + " " + _in_symbol->getString() + " " + _range->getText(indent);
	}

private:
	TokenIt _id; ///< Identifier of the string
	TokenIt _in_symbol; ///< Token holding "at"
	Expression::Ptr _range; ///< Range expression
};

/**
 * Class representing expression which references string match count.
 *
 * For example:
 * @code
 * #str > 1
 * ^^^^
 * @endcode
 */
class StringCountExpression : public Expression
{
public:
	StringCountExpression(TokenIt id) : _id(id) {}

	template <typename Str>
	StringCountExpression(Str&& id)
	{
		_id = _tokenStream->emplace_back(STRING_COUNT, std::forward<Str>(id));
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	std::string getId() const { return _id->getPureText(); }

	void setId(const std::string& id) { _id->setValue(id); }
	void setId(std::string&& id) { _id->setValue(std::move(id)); }

	virtual TokenIt getFirstTokenIt() const override { return _id; }
	virtual TokenIt getLastTokenIt() const override { return _id; }

	virtual std::string getText(const std::string& /*indent*/ = std::string{}) const override
	{
		auto output = getId();
		assert(output != std::string() && "String id must be non-empty.");
		output[0] = '#';
		return output;
	}

private:
	TokenIt _id; ///< Identifier of the string
};

/**
 * Class representing expression which references first string match offset
 * or specific Nth match offset.
 *
 * For example:
 * @code
 * (@str > 0x100) and (@str[2] < 0x1000)
 *  ^^^^               ^^^^^^^
 * @endcode
 */
class StringOffsetExpression : public Expression
{
public:
	StringOffsetExpression(TokenIt id)
		: _id(id)
	{
	}
	template <typename ExpPtr>
	StringOffsetExpression(TokenIt id, ExpPtr&& expr)
		: _id(id)
		, _expr(std::forward<ExpPtr>(expr))
	{
	}
	template <typename Str>
	StringOffsetExpression(Str&& id)
	{
		_id = _tokenStream->emplace_back(STRING_OFFSET, std::forward<Str>(id));
	}
	template <typename Str, typename ExpPtr>
	StringOffsetExpression(Str&& id, ExpPtr&& expr)
		: _expr(std::forward<ExpPtr>(expr))
	{
		_id = _tokenStream->emplace_back(STRING_OFFSET, std::forward<Str>(id));
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	std::string getId() const { return _id->getPureText(); }
	const Expression::Ptr& getIndexExpression() const { return _expr; }

	void setId(const std::string& id) { _id->setValue(id); }
	void setId(std::string&& id) { _id->setValue(std::move(id)); }
	void setIndexExpression(const Expression::Ptr& expr) { _expr = expr; }
	void setIndexExpression(Expression::Ptr&& expr) { _expr = std::move(expr); }

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		auto prefix = getId();
		assert(prefix != std::string() && "String id must be non-empty.");
		prefix[0] = '@';
		return _expr ? prefix + '[' + _expr->getText(indent) + ']' : prefix;
	}

	virtual TokenIt getFirstTokenIt() const override { return _id; }
	virtual TokenIt getLastTokenIt() const override { return _expr ? _expr->getLastTokenIt() : _id; }

private:
	TokenIt _id; ///< Identifier of the string
	Expression::Ptr _expr; ///< Index expression if any
};

/**
 * Class representing expression which references first string match length
 * or specific Nth match length.
 *
 * For example:
 * @code
 * (!str > 5) and (!str[2] < 10)
 *  ^^^^           ^^^^^^^
 * @endcode
 */
class StringLengthExpression : public Expression
{
public:
	StringLengthExpression(TokenIt id)
		: _id(id)
	{
	}
	template <typename ExpPtr>
	StringLengthExpression(TokenIt id, ExpPtr&& expr)
		: _id(id)
		, _expr(std::forward<ExpPtr>(expr))
	{
	}
	template <typename Str>
	StringLengthExpression(Str&& id)
	{
		_id = _tokenStream->emplace_back(STRING_LENGTH, std::forward<Str>(id));
	}
	template <typename Str, typename ExpPtr>
	StringLengthExpression(Str&& id, ExpPtr&& expr)
		: _expr(std::forward<ExpPtr>(expr))
	{
		_id = _tokenStream->emplace_back(STRING_LENGTH, std::forward<Str>(id));
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	std::string getId() const { return _id->getPureText(); }
	const Expression::Ptr& getIndexExpression() const { return _expr; }

	void setId(const std::string& id) { _id->setValue(id); }
	void setId(std::string&& id) { _id->setValue(std::move(id)); }
	void setIndexExpression(const Expression::Ptr& expr) { _expr = expr; }
	void setIndexExpression(Expression::Ptr&& expr) { _expr = std::move(expr); }

	virtual TokenIt getFirstTokenIt() const override { return _id; }
	virtual TokenIt getLastTokenIt() const override { return _expr ? _expr->getLastTokenIt() : _id; }

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		auto prefix = getId();
		assert(prefix != std::string() && "String id must be non-empty.");
		prefix[0] = '!';
		return _expr ? getId() + '[' + _expr->getText(indent) + ']' : getId();
	}

private:
	TokenIt _id; ///< Identifier of the string
	Expression::Ptr _expr; ///< Index expression if any
};

/**
 * Abstract class representing some unary operation.
 */
class UnaryOpExpression : public Expression
{
public:
	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		if (_op->getType() == NOT)
			return _op->getString() + " " + _expr->getText(indent);
		else
			return _op->getString() + _expr->getText(indent);
	}

	const Expression::Ptr& getOperand() const { return _expr; }

	void setOperand(const Expression::Ptr& expr) { _expr = expr; }
	void setOperand(Expression::Ptr&& expr) { _expr = std::move(expr); }

protected:
	template <typename ExpPtr>
	UnaryOpExpression(TokenIt op, ExpPtr&& expr)
		: _op(op)
		, _expr(std::forward<ExpPtr>(expr))
	{
	}
	template <typename ExpPtr>
	UnaryOpExpression(const std::string& op, TokenType type, ExpPtr&& expr)
		: _expr(std::forward<ExpPtr>(expr))
	{
		_op = _tokenStream->emplace_back(type, op);
	}
	virtual TokenIt getFirstTokenIt() const override { return _op; }
	virtual TokenIt getLastTokenIt() const override { return _expr->getLastTokenIt(); }

private:
	TokenIt _op; ///< Unary operation symbol, std::string
	Expression::Ptr _expr; ///< Expression to apply operator on
};

/**
 * Class representing logical not operation.
 *
 * For example:
 * @code
 * !(@str > 10)
 * @endcode
 */
class NotExpression : public UnaryOpExpression
{
public:
	template <typename ExpPtr>
	NotExpression(TokenIt op, ExpPtr&& expr) : UnaryOpExpression(op, std::forward<ExpPtr>(expr)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unary minus operation.
 *
 * For example:
 * @code
 * @str1 - @str2 == -20
 *                  ^^^
 * @endcode
 */
class UnaryMinusExpression : public UnaryOpExpression
{
public:
	template <typename ExpPtr>
	UnaryMinusExpression(TokenIt op, ExpPtr&& expr) : UnaryOpExpression(op, std::forward<ExpPtr>(expr)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing bitwise not operation.
 *
 * For example:
 * @code
 * ~uint8(0x0) == 0xab
 * ^^^^^^^^^^^
 * @endcode
 */
class BitwiseNotExpression : public UnaryOpExpression
{
public:
	template <typename ExpPtr>
	BitwiseNotExpression(TokenIt op, ExpPtr&& expr) : UnaryOpExpression(op, std::forward<ExpPtr>(expr)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Abstract class representing some binary operation.
 */
class BinaryOpExpression : public Expression
{
public:
	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		return _left->getText(indent) + ' ' + _op->getString() + (_linebreak ? "\n" + indent : " ") + _right->getText(indent);
	}

	TokenIt getOperator() const { return _op; }
	const Expression::Ptr& getLeftOperand() const { return _left; }
	const Expression::Ptr& getRightOperand() const { return _right; }

	virtual TokenIt getFirstTokenIt() const override { return _left->getFirstTokenIt(); }
	virtual TokenIt getLastTokenIt() const override { return _right->getLastTokenIt(); }

	void setLeftOperand(const Expression::Ptr& left) { _left = left; }
	void setLeftOperand(Expression::Ptr&& left) { _left = std::move(left); }
	void setRightOperand(const Expression::Ptr& right) { _right = right; }
	void setRightOperand(Expression::Ptr&& right) { _right = std::move(right); }

protected:
	template <typename ExpPtr1, typename ExpPtr2>
	BinaryOpExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right, bool linebreak = false)
		: _op(op)
		, _left(std::forward<ExpPtr1>(left))
		, _right(std::forward<ExpPtr2>(right))
		, _linebreak(linebreak)
	{
	}
	template <typename ExpPtr1, typename ExpPtr2>
	BinaryOpExpression(ExpPtr1&& left, const std::string& op, TokenType type, ExpPtr2&& right, bool linebreak = false)
		: _left(std::forward<ExpPtr1>(left))
		, _right(std::forward<ExpPtr2>(right))
		, _linebreak(linebreak)
	{
		_op = _tokenStream->emplace_back(type, op);
	}

private:
	TokenIt _op; ///< Binary operation symbol, std::string
	Expression::Ptr _left, _right; ///< Expressions to apply operation on
	bool _linebreak; ///< Put linebreak after operation symbol
};

/**
 * Class representing logical and operation.
 *
 * For example:
 * @code
 * $str1 and $str2
 * @endcode
 */
class AndExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	AndExpression(ExpPtr1&& left, TokenIt and_op, ExpPtr2&& right, bool linebreak = false) : BinaryOpExpression(std::forward<ExpPtr1>(left), and_op, std::forward<ExpPtr2>(right), linebreak) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing logical or operation.
 *
 * For example:
 * @code
 * $str1 or $str2
 * @endcode
 */
class OrExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	OrExpression(ExpPtr1&& left, TokenIt op_or, ExpPtr2&& right, bool linebreak = false) : BinaryOpExpression(std::forward<ExpPtr1>(left), op_or, std::forward<ExpPtr2>(right), linebreak) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing less than operation.
 *
 * For example:
 * @code
 * @str1 < @str2
 * @endcode
 */
class LtExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	LtExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing greater than operation.
 *
 * For example:
 * @code
 * @str1 > @str2
 * @endcode
 */
class GtExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	GtExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing less or equal than operation.
 *
 * For example:
 * @code
 * @str1 <= @str2
 * @endcode
 */
class LeExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	LeExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing greater or equal than operation.
 *
 * For example:
 * @code
 * @str1 >= @str2
 * @endcode
 */
class GeExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	GeExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing is equal operation.
 *
 * For example:
 * @code
 * !str1 == !str2
 * @endcode
 */
class EqExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	EqExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing is not equal operation.
 *
 * For example:
 * @code
 * !str1 != !str2
 * @endcode
 */
class NeqExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	NeqExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing contains operation on two strings.
 *
 * For example:
 * @code
 * pe.sections[0] contains "text"
 * @endcode
 */
class ContainsExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	ContainsExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing contains operation on string and regular expression.
 *
 * For example:
 * @code
 * pe.sections[0] matches /(text|data)/
 * @endcode
 */
class MatchesExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	MatchesExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing arithmetic plus operation.
 *
 * For example:
 * @code
 * @str1 + 0x100 == @str2
 * ^^^^^^^^^^^^^
 * @endcode
 */
class PlusExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	PlusExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing arithmetic minus operation.
 *
 * For example:
 * @code
 * @str1 - 0x100 == @str2
 * ^^^^^^^^^^^^^
 * @endcode
 */
class MinusExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	MinusExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing arithmetic multiply operation.
 *
 * For example:
 * @code
 * @str1 * 2 == @str2
 * ^^^^^^^^^
 * @endcode
 */
class MultiplyExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	MultiplyExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing arithmetic division operation.
 *
 * For example:
 * @code
 * @str1 \ 2 == @str2
 * ^^^^^^^^^
 * @endcode
 */
class DivideExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	DivideExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing arithmetic integral modulo operation.
 *
 * For example:
 * @code
 * @str1 % 2 == 0
 * ^^^^^^^^^
 * @endcode
 */
class ModuloExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	ModuloExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing bitwise xor operation
 *
 * For example:
 * @code
 * uint8(0x10) ^ uint8(0x20) == 0
 * ^^^^^^^^^^^^^^^^^^^^^^^^^
 * @endcode
 */
class BitwiseXorExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	BitwiseXorExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing bitwise and operation
 *
 * For example:
 * @code
 * pe.characteristics & pe.DLL
 * @endcode
 */
class BitwiseAndExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	BitwiseAndExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing bitwise and operation
 *
 * For example:
 * @code
 * pe.characteristics | pe.DLL
 * @endcode
 */
class BitwiseOrExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	BitwiseOrExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing bitwise shift left operation
 *
 * For example:
 * @code
 * uint8(0x10) << 2
 * @endcode
 */
class ShiftLeftExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	ShiftLeftExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing bitwise shift left operation
 *
 * For example:
 * @code
 * uint8(0x10) >> 2
 * @endcode
 */
class ShiftRightExpression : public BinaryOpExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	ShiftRightExpression(ExpPtr1&& left, TokenIt op, ExpPtr2&& right) : BinaryOpExpression(std::forward<ExpPtr1>(left), op, std::forward<ExpPtr2>(right)) {}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Abstract class representing for loop. For loop can be either
 * over integers or set of string references. They can be distinguished
 * by keywords 'in' and 'of'. Integer for loop uses 'in' and string for loop
 * uses 'of'. When iterating over set of integers, the symbol obtaining
 * values of these integer is defined together with for loop. String-based for loops
 * may also have string sets substituted with keyword 'them' to reference all strings
 * in the string section.
 */
class ForExpression : public Expression
{
public:
	const Expression::Ptr& getVariable() const { return _forExpr; }
	const Expression::Ptr& getIteratedSet() const { return _set; }
	const Expression::Ptr& getBody() const { return _expr; }

	void setVariable(const Expression::Ptr& forExpr) { _forExpr = forExpr; }
	void setVariable(Expression::Ptr&& forExpr) { _forExpr = std::move(forExpr); }
	void setIteratedSet(const Expression::Ptr& set) { _set = set; }
	void setIteratedSet(Expression::Ptr&& set) { _set = std::move(set); }
	void setBody(const Expression::Ptr& expr) { _expr = expr; }
	void setBody(Expression::Ptr&& expr) { _expr = std::move(expr); }

protected:
	template <typename ExpPtr1, typename ExpPtr2, typename ExpPtr3>
	ForExpression(ExpPtr1&& forExpr, TokenIt of_in, ExpPtr2&& set, ExpPtr3&& expr)
		: _forExpr(std::forward<ExpPtr1>(forExpr))
		, _set(std::forward<ExpPtr2>(set))
		, _expr(std::forward<ExpPtr3>(expr))
		, _of_in(of_in)
	{
	}

	template <typename ExpPtr1, typename ExpPtr2>
	ForExpression(ExpPtr1&& forExpr, TokenIt of_in, ExpPtr2&& set)
		: _forExpr(std::forward<ExpPtr1>(forExpr))
		, _set(std::forward<ExpPtr2>(set))
		, _expr(nullptr)
		, _of_in(of_in)
	{
	}

	Expression::Ptr _forExpr, _set, _expr;
	TokenIt _of_in;
};

/**
 * Class representing for loop over integer set or integer range.
 *
 * For example:
 * @code
 * for all i in (1 .. 5) : ( #str[i] > 0 }
 * @endcode
 */
class ForIntExpression : public ForExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2, typename ExpPtr3>
	ForIntExpression(TokenIt for_token, ExpPtr1&& forExpr, TokenIt id, TokenIt in, ExpPtr2&& set, TokenIt left_bracket, ExpPtr3&& expr, TokenIt right_bracket)
		: ForExpression(std::forward<ExpPtr1>(forExpr), in, std::forward<ExpPtr2>(set), std::forward<ExpPtr3>(expr))
		, _id(id)
		, _for (for_token)
		, _left_bracket(left_bracket)
		, _right_bracket(right_bracket)
	{
	}

	const std::string& getId() const { return _id->getString(); }

	void setId(const std::string& id) { _id->setValue(id); }
	void setId(std::string&& id) { _id->setValue(std::move(id)); }

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		assert(_set);
		std::stringstream ss;
		ss << _for->getString() << " " << _forExpr->getText(indent) << " " << _id->getString() << " "
			<< _of_in->getString() << " " << _set->getText(indent) << " : "
			<< _left_bracket->getString()<< " " << _expr->getText(indent) << " " << _right_bracket->getString();
		return ss.str();
	}

	virtual TokenIt getFirstTokenIt() const override { return _for; }
	virtual TokenIt getLastTokenIt() const override { return _right_bracket; }

private:
	TokenIt _id; ///< Iterating identifier
	TokenIt _for; ///< TokenIt of 'for'
	TokenIt _left_bracket; ///< TokenIt of '('
	TokenIt _right_bracket; ///< TokenIt of ')'
};

/**
 * Class representing for loop over string set.
 *
 * For example:
 * @code
 * for all of ($str1, $str2) : ( $ at entrypoint )
 * @endcode
 */
class ForStringExpression : public ForExpression
{
public:
	/**
	 * Constructor
	 */
	template <typename ExpPtr1, typename ExpPtr2, typename ExpPtr3>
	ForStringExpression(TokenIt for_token, ExpPtr1&& forExpr, TokenIt of, ExpPtr2&& set, TokenIt left_bracket, ExpPtr3&& expr, TokenIt right_bracket)
		: ForExpression(std::forward<ExpPtr1>(forExpr), of, std::forward<ExpPtr2>(set), std::forward<ExpPtr3>(expr))
		, _for (for_token)
		, _left_bracket(left_bracket)
		, _right_bracket(right_bracket)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		std::stringstream ss;
		ss	<< _for->getString() << " " << _forExpr->getText(indent) << " "
			<< _of_in->getString() + " " << _set->getText(indent) << " : "
			<< _left_bracket->getString() << " " << _expr->getText(indent) << " " << _right_bracket->getString();
		return ss.str();
	}

	virtual TokenIt getFirstTokenIt() const override { return _for; }
	virtual TokenIt getLastTokenIt() const override { return _right_bracket; }
private:
	TokenIt _for;
	TokenIt _left_bracket;
	TokenIt _right_bracket;
};

/**
 * Class representing 'of' expression. 'of' expression is shortened version of
 * for loop over string set with no loop body. It has hidden body which always contains just ( $ ).
 *
 * For example:
 * @code
 * all of ($str1, $str2)
 * @endcode
 */
class OfExpression : public ForExpression
{
public:
	/**
	 * Constructor
	 */
	template <typename ExpPtr1, typename ExpPtr2>
	OfExpression(ExpPtr1&& forExpr, TokenIt of, ExpPtr2&& set)
		: ForExpression(std::forward<ExpPtr1>(forExpr), of, std::forward<ExpPtr2>(set))
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		return _forExpr->getText(indent) + " " + _of_in->getString() + " " + _set->getText(indent);
	}

	virtual TokenIt getFirstTokenIt() const override { return _forExpr->getFirstTokenIt(); }
	virtual TokenIt getLastTokenIt() const override { return _set->getLastTokenIt(); }
};

/**
 * Class representing set of either strings or integers. String set may also contain
 * string wildcard referencing more than one string with single identifier.
 *
 * For example:
 * @code
 * for all i in (1,2,3,4,5) : ( $str at ( entrypoint + i ) )
 *              ^^^^^^^^^^^
 * all of ($str*,$1,$2)
 *        ^^^^^^^^^^^^^
 * @endcode
 */
class SetExpression : public Expression
{
public:
	/**
	 * Constructor
	 */
	template <typename ExpPtrVector>
	SetExpression(TokenIt left_bracket, ExpPtrVector&& elements, TokenIt right_bracket)
		: _left_bracket(left_bracket)
		, _elements(std::forward<ExpPtrVector>(elements))
		, _right_bracket(right_bracket)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		std::ostringstream ss;
		ss << _left_bracket->getString();
		for (const auto& elem : _elements)
			ss << elem->getText(indent) << ", ";
		ss <<_right_bracket->getString();

		// Remove last ', ' from the result.
		auto text = ss.str();
		text.erase(text.length() - 3, 2);
		return text;
	}

	const std::vector<Expression::Ptr>& getElements() const { return _elements; }

	virtual TokenIt getFirstTokenIt() const override { return _left_bracket; }
	virtual TokenIt getLastTokenIt() const override { return _right_bracket; }

	void setElements(const std::vector<Expression::Ptr>& elements)
	{
		_elements = elements;
	}

	void setElements(std::vector<Expression::Ptr>&& elements)
	{
		_elements = std::move(elements);
	}

private:
	TokenIt _left_bracket;
	std::vector<Expression::Ptr> _elements; ///< Elements of the set
	TokenIt _right_bracket;
};

/**
 * Class representing range of integers.
 *
 * For example:
 * @code
 * $str in (0x100 .. 0x200)
 *         ^^^^^^^^^^^^^^^^
 * @endcode
 */
class RangeExpression : public Expression
{
public:
	/**
	 * Constructor
	 */
	template <typename ExpPtr1, typename ExpPtr2>
	RangeExpression(TokenIt left_bracket, ExpPtr1&& low, TokenIt double_dot, ExpPtr2&& high, TokenIt right_bracket)
		: _left_bracket(left_bracket)
		, _low(std::forward<ExpPtr1>(low))
		, _double_dot(double_dot)
		, _high(std::forward<ExpPtr2>(high))
		, _right_bracket(right_bracket)
	{
	}
	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		return _left_bracket->getString() + _low->getText(indent) + " " + _double_dot->getString() + " " + _high->getText(indent) + _right_bracket->getString();
	}

	const Expression::Ptr& getLow() const { return _low; }
	const Expression::Ptr& getHigh() const { return _high; }

	virtual TokenIt getFirstTokenIt() const override { return _left_bracket; }
	virtual TokenIt getLastTokenIt() const override { return _right_bracket; }

	void setLow(const Expression::Ptr& low) { _low = low; }
	void setLow(Expression::Ptr&& low) { _low = std::move(low); }
	void setHigh(const Expression::Ptr& high) { _high = high; }
	void setHigh(Expression::Ptr&& high) { _high = std::move(high); }

private:
	TokenIt _left_bracket; ///< '('
	Expression::Ptr _low;  ///< Upper bound of the range
	TokenIt _double_dot; ///< '..'
	Expression::Ptr _high; ///< Lower bound of the range
	TokenIt _right_bracket; ///< ')'
};

/**
 * Class representing identifier expression. This can be either identifier of the imported module,
 * identifier of the variable in the integer-based for loop or reference to another rule in the YARA file.
 *
 * For example:
 * @code
 * rule1 and pe.number_of_sections > 2
 * ^^^^^     ^^
 * @endcode
 */
class IdExpression : public Expression
{
public:
	/**
	 * Constructors
	 */
	IdExpression(TokenIt symbolToken)
		: _symbol(symbolToken->getSymbol())
		, _symbolToken(symbolToken)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& /*indent*/ = std::string{}) const override
	{
		assert(_symbol);
		return _symbol->getName();
	}

	const std::shared_ptr<Symbol>& getSymbol() const
	{
		return _symbol;
	}

	TokenIt getSymbolToken() const
	{
		return _symbolToken;
	}

	virtual TokenIt getFirstTokenIt() const override { return _symbolToken; }
	virtual TokenIt getLastTokenIt() const override { return _symbolToken; }

	void setSymbol(const std::shared_ptr<Symbol>& symbol)
	{
		_symbol = symbol;
		_symbolToken->setValue(_symbol);
	}

protected:
	IdExpression(const std::shared_ptr<Symbol>& symbol)
		: _symbol(symbol)
	{
	}

	std::shared_ptr<Symbol> _symbol; ///< Symbol of the identifier
	TokenIt _symbolToken; ///< Token of the identifier
};

/**
 * Class representing access to the structure identifier. Structure identifier may only be imported module identifier,
 * or another attributes of the imported module structure.
 *
 * For example:
 * @code
 * pe.number_of_sections > 2
 * ^^^^^^^^^^^^^^^^^^^^^
 * @endcode
 */
class StructAccessExpression : public IdExpression
{
public:
	template <typename ExpPtr>
	StructAccessExpression(ExpPtr&& structure, TokenIt dot, TokenIt symbol)
		: IdExpression(symbol)
		, _structure(std::forward<ExpPtr>(structure))
		, _dot(dot)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		if (_symbol)
			return _structure->getText(indent) + _dot->getString() + _symbol->getName();
		return _structure->getText(indent) + _dot->getString();
	}

	const Expression::Ptr& getStructure() const { return _structure; }

	virtual TokenIt getFirstTokenIt() const override { return _structure->getFirstTokenIt(); }
	virtual TokenIt getLastTokenIt() const override { return _symbolToken; }

	void setStructure(const Expression::Ptr& structure) { _structure = structure; }
	void setStructure(Expression::Ptr&& structure) { _structure = std::move(structure); }

private:
	Expression::Ptr _structure; ///< Structure identifier expression
	TokenIt _dot; ///< '.'
};

/**
 * Class representing access to the array or the dictionary identifier. Array and dictionary identifiers may only be
 * attributes of the imported module structure.
 *
 * For example:
 * @code
 * pe.sections[0].name contains "text"
 *    ^^^^^^^^^^^
 * @endcode
 */
class ArrayAccessExpression : public IdExpression
{
public:
	template <typename ExpPtr1, typename ExpPtr2>
	ArrayAccessExpression(const std::shared_ptr<Symbol>& symbol, ExpPtr1&& array, TokenIt left_bracket, ExpPtr2&& accessor, TokenIt right_bracket)
		: IdExpression(symbol)
		, _array(std::forward<ExpPtr1>(array))
		, _left_bracket(left_bracket)
		, _accessor(std::forward<ExpPtr2>(accessor))
		, _right_bracket(right_bracket)
	{
		_symbolToken = std::static_pointer_cast<const IdExpression>(_array)->getSymbolToken();
	}
	template <typename ExpPtr1, typename ExpPtr2>
	ArrayAccessExpression(ExpPtr1&& array, TokenIt left_bracket, ExpPtr2&& accessor, TokenIt right_bracket)
		: IdExpression(std::static_pointer_cast<const IdExpression>(array)->getSymbolToken())
		, _array(std::forward<ExpPtr1>(array))
		, _left_bracket(left_bracket)
		, _accessor(std::forward<ExpPtr2>(accessor))
		, _right_bracket(right_bracket)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		return _array->getText(indent) + _left_bracket->getString() + _accessor->getText(indent) + _right_bracket->getString();
	}

	const Expression::Ptr& getArray() const { return _array; }
	const Expression::Ptr& getAccessor() const { return _accessor; }

	virtual TokenIt getFirstTokenIt() const override { return _array->getFirstTokenIt(); }
	virtual TokenIt getLastTokenIt() const override { return _right_bracket; }

	void setArray(const Expression::Ptr& array) { _array = array; }
	void setArray(Expression::Ptr&& array) { _array = std::move(array); }
	void setAccessor(const Expression::Ptr& accessor) { _accessor = accessor; }
	void setAccessor(Expression::Ptr&& accessor) { _accessor = std::move(accessor); }

private:
	Expression::Ptr _array; ///< Array identifier expression
	TokenIt _left_bracket; ///< '['
	Expression::Ptr _accessor; ///< Accessor expression (expression enclosed in [])
	TokenIt _right_bracket; ///< ']'
};

/**
 * Class representing call to a function. Functions may only be attributes of the imported module structure.
 *
 * For example:
 * @code
 * pe.exports("ExitProcess")
 *    ^^^^^^^^^^^^^^^^^^^^^^
 * @endcode
 */
class FunctionCallExpression : public IdExpression
{
public:
	template <typename ExpPtr, typename ExpPtrVector>
	FunctionCallExpression(ExpPtr&& func, TokenIt left_bracket, ExpPtrVector&& args, TokenIt right_bracket)
		: IdExpression(std::static_pointer_cast<const IdExpression>(func)->getSymbolToken())
		, _func(std::forward<ExpPtr>(func))
		, _left_bracket(left_bracket)
		, _args(std::forward<ExpPtrVector>(args))
		, _right_bracket(right_bracket)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		// Use just empty parentheses for parameter-less function
		if (_args.empty())
			return _func->getText(indent) + _left_bracket->getString() + _right_bracket->getString();

		std::ostringstream ss;
		ss << _func->getText(indent) << _left_bracket->getString();
		for (const auto& arg : _args)
		{
			ss << arg->getText(indent) << ", ";
		}
		ss << _right_bracket->getString();

		// Remove last ', ' from the result.
		auto text = ss.str();
		text.erase(text.length() - 3, 2);
		return text;
	}

	const Expression::Ptr& getFunction() const { return _func; }
	const std::vector<Expression::Ptr>& getArguments() const { return _args; }

	virtual TokenIt getFirstTokenIt() const override { return _func->getFirstTokenIt(); }
	virtual TokenIt getLastTokenIt() const override { return _right_bracket; }

	void setFunction(const Expression::Ptr& func) { _func = func; }
	void setFunction(Expression::Ptr&& func) { _func = std::move(func); }
	void setArguments(const std::vector<Expression::Ptr>& args) { _args = args; }
	void setArguments(std::vector<Expression::Ptr>&& args) { _args = std::move(args); }

private:
	Expression::Ptr _func; ///< Function identifier expression
	TokenIt _left_bracket; ///< '('
	std::vector<Expression::Ptr> _args; ///< Arguments expressions
	TokenIt _right_bracket; ///< ')'
};

/**
 * Abstract class representing literal value expression of certain type T. This class is intended to be
 * inherited as specialization.
 */
template <typename T>
class LiteralExpression : public Expression
{
public:
	using LiteralType = T;

	LiteralExpression() : _valid(false) {}
	LiteralExpression(TokenIt value) : _value(value) {}
	LiteralExpression(const std::shared_ptr<TokenStream>& ts, TokenIt value)
		: Expression(ts)
		, _value(value)
	{
	}

	virtual LiteralType getValue() const = 0;

	virtual std::string getText(const std::string& /*indent*/ = std::string{}) const override
	{
		if (_valid)
			return _value->getText();
		else
			return std::string();
	}

	virtual TokenIt getFirstTokenIt() const override { return _value; }
	virtual TokenIt getLastTokenIt() const override { return _value; }

	void clear()
	{
		if (_valid)
			_tokenStream->erase(_value);
	}

protected:
	bool _valid = true; ///< Set if _value is valid
	TokenIt _value; ///< Value of the literal
};

/**
 * Class representing boolean literal expression. Can be either true or false.
 *
 * For example:
 * @code
 * true or false
 * ^^^^    ^^^^^
 * @endcode
 */
class BoolLiteralExpression : public LiteralExpression<bool>
{
public:
	BoolLiteralExpression(TokenIt value)
		: LiteralExpression<bool>(value)
	{
	}

	BoolLiteralExpression(bool value)
		: LiteralExpression<bool>()
	{
		if (value)
			_value = _tokenStream->emplace_back(BOOL_TRUE, value, "true");
		else
			_value = _tokenStream->emplace_back(BOOL_FALSE, value, "false");
		_valid = true;
	}

	BoolLiteralExpression(const std::shared_ptr<TokenStream>& ts, TokenIt value)
		: LiteralExpression<bool>(ts, value)
	{
	}

	virtual LiteralType getValue() const override
	{
		return _value->getBool();
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing string literal expression. Strings are enclosed in double quotes.
 *
 * For example:
 * @code
 * pe.section[0].name contains "text"
 *                             ^^^^^^
 * @endcode
 */
class StringLiteralExpression : public LiteralExpression<std::string>
{
public:
	StringLiteralExpression(TokenIt value)
		: LiteralExpression<std::string>(value)
	{
	}
	StringLiteralExpression(const std::shared_ptr<TokenStream>& ts, TokenIt value)
		: LiteralExpression<std::string>(ts, value)
	{
	}

	virtual LiteralType getValue() const override
	{
		return _value->getString();
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing integer literal expression. Integers are stored in string representation to preserve
 * base and preceding zeroes.
 *
 * For example:
 * @code
 * @str1 == 0x100
 *          ^^^^^
 * @endcode
 */
class IntLiteralExpression : public LiteralExpression<uint64_t>
{
public:
	IntLiteralExpression(TokenIt value)
		: LiteralExpression<uint64_t>(value)
	{
	}

	IntLiteralExpression(const std::shared_ptr<TokenStream>& ts, TokenIt value)
		: LiteralExpression<uint64_t>(ts, value)
	{
	}

	virtual LiteralType getValue() const override
	{
		return _value->getUInt();
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing floating point literal expression. Floats are stored in string representation to preserve
 * preceding and trailing zeroes.
 *
 * For example:
 * @code
 * math.mean(0, filesize) < 72.0
 *                          ^^^^
 * @endcode
 */
class DoubleLiteralExpression : public LiteralExpression<double>
{
public:
	DoubleLiteralExpression(TokenIt value)
		: LiteralExpression<double>(value)
	{
	}

	DoubleLiteralExpression(const std::shared_ptr<TokenStream>& ts, TokenIt value)
		: LiteralExpression<double>(ts, value)
	{
	}

	virtual LiteralType getValue() const override
	{
		return _value->getFloat();
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Abstract class representing expression that is formed just of one keyword.
 */
class KeywordExpression : public Expression
{
public:
	virtual std::string getText(const std::string& /*indent*/ = std::string{}) const override
	{
		return _keyword->getString();
	}

	virtual TokenIt getFirstTokenIt() const override { return _keyword; }
	virtual TokenIt getLastTokenIt() const override { return _keyword; }

protected:
	KeywordExpression() = default;
	KeywordExpression(TokenIt keyword)
		: _keyword(keyword)
	{
		assert(keyword->isString());
	}
	KeywordExpression(const std::shared_ptr<TokenStream>& ts, TokenIt keyword)
		: Expression(ts) 
		, _keyword(keyword)
	{
		assert(keyword->isString());
	}
	TokenIt _keyword; ///< Keyword
};

/**
 * Class representing 'filesize' expression. This is integer expression.
 *
 * For example:
 * @code
 * uint32(@str1) < filesize
 *                 ^^^^^^^^
 * @endcode
 */
class FilesizeExpression : public KeywordExpression
{
public:
	FilesizeExpression(TokenIt t)
		: KeywordExpression(t)
	{
	}
	FilesizeExpression(const std::shared_ptr<TokenStream>& ts, TokenIt t)
		: KeywordExpression(ts, t)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing 'entrypoint' expression. This is integer expression.
 *
 * For example:
 * @code
 * $str1 at entrypoint
 *          ^^^^^^^^^^
 * @endcode
 */
class EntrypointExpression : public KeywordExpression
{
public:
	EntrypointExpression(TokenIt t)
		: KeywordExpression(t)
	{
	}
	EntrypointExpression(const std::shared_ptr<TokenStream>& ts, TokenIt t)
		: KeywordExpression(ts, t)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing 'all' expression. Can be used in conjunction with for loops indicating that for loop
 * needs to be evaluated true for all variables in the referenced set. This expression does not have a type.
 *
 * For example:
 * @code
 * all of them
 * ^^^
 * @endcode
 */
class AllExpression : public KeywordExpression
{
public:
	AllExpression(TokenIt t)
		: KeywordExpression(t)
	{
	}
	AllExpression(const std::shared_ptr<TokenStream>& ts, TokenIt t)
		: KeywordExpression(ts, t)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing 'any' expression. Can be used in conjunction with for loops indicating that for loop
 * needs to be evaluated true for at least one variables in the referenced set. This expression does not have a type.
 *
 * For example:
 * @code
 * any of them
 * ^^^
 * @endcode
 */
class AnyExpression : public KeywordExpression
{
public:
	AnyExpression(TokenIt t)
		: KeywordExpression(t)
	{
	}
	AnyExpression(const std::shared_ptr<TokenStream>& ts, TokenIt t)
		: KeywordExpression(ts, t)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing 'them' expression. Can be used in conjunction with string-based for loops referencing
 * all string from the strings section instead of specific set. This expression does not have a type.
 *
 * For example:
 * @code
 * any of them
 *        ^^^^
 * @endcode
 */
class ThemExpression : public KeywordExpression
{
public:
	ThemExpression(TokenIt t)
		: KeywordExpression(t)
	{
	}
	ThemExpression(const std::shared_ptr<TokenStream>& ts, TokenIt t)
		: KeywordExpression(ts, t)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing expression enclosed in parentheses. We need this kind of expression to preserve
 * parentheses when obtaining string representation of YARA file.
 *
 * For example:
 * @code
 * ((5 + 6) * 30) < filesize
 * ^^^^^^^^^^^^^^
 * @endcode
 */
class ParenthesesExpression : public Expression
{
public:
	/**
	 * Constructor
	 *
	 * @param TokenIt left_bracket.
	 * @param Expression::Ptr expr  argument inside the brackets.
	 * @param TokenIt right_bracket.
	 * @param bool linebreak.
	 */
	template <typename ExpPtr>
	ParenthesesExpression(TokenIt left_bracket, ExpPtr&& expr, TokenIt right_bracket, bool linebreak = false)
		: _expr(std::forward<ExpPtr>(expr))
		, _linebreak(linebreak)
		, _left_bracket(left_bracket)
		, _right_bracket(right_bracket)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		if (_linebreak)
		{
			auto newIndent = indent + '\t';
			return _left_bracket->getString() + '\n' + newIndent + _expr->getText(newIndent) + '\n' + indent + _right_bracket->getString();
		}

		return _left_bracket->getString() + _expr->getText(indent) + _right_bracket->getString();
	}

	const Expression::Ptr& getEnclosedExpression() const { return _expr; }

	virtual TokenIt getFirstTokenIt() const override { return _left_bracket; }
	virtual TokenIt getLastTokenIt() const override { return _right_bracket; }

	void setEnclosedExpression(const Expression::Ptr& expr) { _expr = expr; }
	void setEnclosedExpression(Expression::Ptr&& expr) { _expr = std::move(expr); }

private:
	Expression::Ptr _expr; ///< Enclosed expression
	bool _linebreak; ///< Put linebreak after opening and before closing parentheses and indent content by one more level.
	TokenIt _left_bracket;
	TokenIt _right_bracket;
};

/**
 * Class representing call to special built-in functions for reading fixed-width integer values from the file.
 * These functions are @c int8, @c int16, @c int32 and their unsigned counterparts prefixed with @c u. These functions also
 * have big-endian version suffixed with @c be.
 *
 * For example:
 * @code
 * uint16(0) == 0x5A4D
 * ^^^^^^^^^
 * @endcode
 */
class IntFunctionExpression : public Expression
{
public:
	/**
	 * Constructor
	 *
	 * @param TokenIt func  name of the function.
	 * @param TokenIt left_bracket.
	 * @param Expression::Ptr expr  argument of the function.
	 * @param TokenIt right_bracket.
	 */
	template <typename ExpPtr>
	IntFunctionExpression(TokenIt func, TokenIt left_bracket, ExpPtr&& expr, TokenIt right_bracket)
		: _func(func)
		, _expr(std::forward<ExpPtr>(expr))
		, _left_bracket(left_bracket)
		, _right_bracket(right_bracket)
	{
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& indent = std::string{}) const override
	{
		return _func->getString() + _left_bracket->getString() + _expr->getText(indent) + _right_bracket->getString();
	}

	const std::string& getFunction() const { return _func->getString(); }
	const Expression::Ptr& getArgument() const { return _expr; }

	virtual TokenIt getFirstTokenIt() const override { return _func; }
	virtual TokenIt getLastTokenIt() const override { return _right_bracket; }

	void setFunction(const std::string& func) { _func->setValue(func); }
	void setFunction(std::string&& func) { _func->setValue(std::move(func)); }
	void setArgument(const Expression::Ptr& expr) { _expr = expr; }
	void setArgument(Expression::Ptr&& expr) { _expr = std::move(expr); }

private:
	TokenIt _func; ///< Function identifier
	Expression::Ptr _expr; ///< Function argument
	TokenIt _left_bracket; ///< left parentheses token
	TokenIt _right_bracket; ///< right parentheses token
};

/**
 * Class representing regular expression.
 *
 * For example:
 * @code
 * pe.sections[0].name matches /(text|data)/
 *                             ^^^^^^^^^^^^^
 * @endcode
 */
class RegexpExpression : public Expression
{
public:
	/**
	 * Constructor.
	 *
	 * @param std::string regexp.
	 */
	template <typename S>
	RegexpExpression(S&& regexp)
		: _regexp(std::forward<S>(regexp))
	{
		_tokenStream = _regexp->getTokenStream();
	}

	virtual VisitResult accept(Visitor* v) override
	{
		return v->visit(this);
	}

	virtual std::string getText(const std::string& /*indent*/ = std::string{}) const override { return _regexp->getText(); }

	const std::shared_ptr<String>& getRegexpString() const { return _regexp; }

	virtual TokenIt getFirstTokenIt() const override { return _regexp->getFirstTokenIt(); }
	virtual TokenIt getLastTokenIt() const override { return _regexp->getLastTokenIt(); }

	void setRegexpString(const std::shared_ptr<String>& regexp) { _regexp = regexp; }
	void setRegexpString(std::shared_ptr<String>&& regexp) { _regexp = std::move(regexp); }

private:
	std::shared_ptr<String> _regexp; ///< Regular expression string
};

}