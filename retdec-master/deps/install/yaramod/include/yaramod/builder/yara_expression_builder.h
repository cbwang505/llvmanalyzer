/**
 * @file src/builder/yara_expression_builder.h
 * @brief Declaration of class YaraExpressionBuilder.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <memory>
#include <string>

#include "yaramod/types/expression.h"
#include "yaramod/types/token_stream.h"
#include "yaramod/types/symbol.h"
#include "yaramod/yaramod_error.h"

namespace yaramod {

/**
 * Enum representing KB and MB integer modifiers.
 */
enum class IntMultiplier
{
	None,
	Kilobytes,
	Megabytes
};

/**
 * Enum representing endianness of integer functions like
 * `int16`, `int16be` etc.
 */
enum class IntFunctionEndianness
{
	Little,
	Big
};

/**
 * Represents error during parsing.
 */
class YaraExpressionBuilderError : public YaramodError
{
public:
	YaraExpressionBuilderError(const std::string& errorMsg) : YaramodError("YaraExpressionBuilder error: " + errorMsg) {}
	YaraExpressionBuilderError(const YaraExpressionBuilderError&) = default;
};

/**
 * Class representing builder of condition expression. You use this builder
 * to specify what you want in your condition expression and then you can obtain
 * your condition expression by calling method @c get. As soon as @c get is called,
 * builder resets to default state and does not contain any data from
 * the previous build process. This builder modifies itself during the construction,
 * so if you want to hold the state of builder for later use you should first copy it.
 */
class YaraExpressionBuilder
{
public:
	/// @name Constructors
	/// @{
	YaraExpressionBuilder()
		: _tokenStream(std::make_shared<TokenStream>())
		, _expr()
	{
	}
	YaraExpressionBuilder(const Expression::Ptr& expr)
		: _tokenStream(std::make_shared<TokenStream>())
		, _expr(expr)
	{
		_tokenStream->moveAppend(_expr->getTokenStream(), _expr->getFirstTokenIt(), std::next(_expr->getLastTokenIt()));
	}
	YaraExpressionBuilder(Expression::Ptr&& expr)
		: _tokenStream(std::make_shared<TokenStream>())
		, _expr(std::move(expr))
	{
		_tokenStream->moveAppend(_expr->getTokenStream(), _expr->getFirstTokenIt(), std::next(_expr->getLastTokenIt()));
	}
	YaraExpressionBuilder(const Expression::Ptr& expr, const Expression::Type& type)
		: _tokenStream(std::make_shared<TokenStream>())
		, _expr(expr)
	{
		setType(type);
		_tokenStream->moveAppend(_expr->getTokenStream(), _expr->getFirstTokenIt(), std::next(_expr->getLastTokenIt()));
	}
	YaraExpressionBuilder(Expression::Ptr&& expr, const Expression::Type& type)
		: _tokenStream(std::make_shared<TokenStream>())
		, _expr(std::move(expr))
	{
		setType(type);
		_tokenStream->moveAppend(_expr->getTokenStream(), _expr->getFirstTokenIt(), std::next(_expr->getLastTokenIt()));
	}
	YaraExpressionBuilder(const std::shared_ptr<TokenStream>& ts)
		: _tokenStream(ts)
		, _expr()
	{
	}
	template <typename ExpPtr>
	YaraExpressionBuilder(const std::shared_ptr<TokenStream>& ts, ExpPtr&& expr)
		: _tokenStream(ts)
		, _expr(std::forward<ExpPtr>(expr))
	{
	}
	template <typename ExpPtr>
	YaraExpressionBuilder(const std::shared_ptr<TokenStream>& ts, ExpPtr&& expr, const Expression::Type& type)
		: _tokenStream(ts)
		, _expr(std::forward<ExpPtr>(expr))
	{
		setType(type);
	}
	YaraExpressionBuilder(const YaraExpressionBuilder&) = default;
	YaraExpressionBuilder(YaraExpressionBuilder&&) = default;
	/// @}

	/// @name Assignments
	/// @{
	YaraExpressionBuilder& operator=(const YaraExpressionBuilder&) = default;
	YaraExpressionBuilder& operator=(YaraExpressionBuilder&&) = default;
	/// @}

	/// @name Getter methods
	/// @{
	Expression::Type getType() const { return _expr->getType(); }
	/// @}

	/// @name Setter methods
	/// @{
	void setType(Expression::Type type) { _expr->setType(type); }
	/// @}

	/// @name Detection methods
	/// @{
	bool canBeBool() const { return _expr->isBool() || _expr->isFloat() || _expr->isInt() || _expr->isUndefined(); }
	/// @}

	/// @name Builder method
	/// @{
	Expression::Ptr get() const;
	TokenStream* getTokenStream() const { return _tokenStream.get(); }
	/// @}

	/// @name Building methods
	/// @{
	YaraExpressionBuilder& operator!();
	YaraExpressionBuilder& operator~();
	YaraExpressionBuilder& operator-();

	YaraExpressionBuilder& operator&&(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator||(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator<(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator<=(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator>(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator>=(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator==(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator!=(const YaraExpressionBuilder& other);

	YaraExpressionBuilder& operator+(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator-(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator*(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator/(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator%(const YaraExpressionBuilder& other);

	YaraExpressionBuilder& operator^(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator&(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator|(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator<<(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& operator>>(const YaraExpressionBuilder& other);

	YaraExpressionBuilder& comment(const std::string& message, bool multiline = false, const std::string& indent = {});
	YaraExpressionBuilder& call(const std::vector<YaraExpressionBuilder>& args);
	/**
	 * Calls function from an expression
	 *
	 * @param args Arguments of the function.
	 *
	 * @return Builder.
	 */
	template <typename... Args> YaraExpressionBuilder& operator()(const Args&... args)
	{
		std::vector<YaraExpressionBuilder> realArgs;
		_buildArgs(realArgs, args...);
		call(realArgs);
		return *this;
	}

	YaraExpressionBuilder& contains(const YaraExpressionBuilder& other);
	YaraExpressionBuilder& matches(const YaraExpressionBuilder& other);

	YaraExpressionBuilder& access(const std::string& attr);
	YaraExpressionBuilder& operator[](const YaraExpressionBuilder& other);

	YaraExpressionBuilder& readInt8(IntFunctionEndianness endianness);
	YaraExpressionBuilder& readInt16(IntFunctionEndianness endianness);
	YaraExpressionBuilder& readInt32(IntFunctionEndianness endianness);
	YaraExpressionBuilder& readUInt8(IntFunctionEndianness endianness);
	YaraExpressionBuilder& readUInt16(IntFunctionEndianness endianness);
	YaraExpressionBuilder& readUInt32(IntFunctionEndianness endianness);
	/// @}

protected:
	void _buildArgs(std::vector<YaraExpressionBuilder>&) {}

	void _buildArgs(std::vector<YaraExpressionBuilder>& realArgs, const YaraExpressionBuilder& arg)
	{
		realArgs.push_back(arg);
	}

	template <typename... Args> void _buildArgs(std::vector<YaraExpressionBuilder>& realArgs, const YaraExpressionBuilder& arg, const Args&... args)
	{
		realArgs.push_back(arg);
		_call(realArgs, args...);
	}

private:
	YaraExpressionBuilder& readIntegerFunction(const std::string& function_name);

	std::shared_ptr<TokenStream> _tokenStream;
	Expression::Ptr _expr;
};

/// @name Helper functions
/// These functions serve for readable and easy way to construct
/// condition expressions using @c YaraExpressionBuilder.
/// @{
YaraExpressionBuilder intVal(std::int64_t value, IntMultiplier mult = IntMultiplier::None);
YaraExpressionBuilder uintVal(std::uint64_t value, IntMultiplier mult = IntMultiplier::None);
YaraExpressionBuilder hexIntVal(std::uint64_t value);
YaraExpressionBuilder doubleVal(double value);
YaraExpressionBuilder stringVal(const std::string& value);
YaraExpressionBuilder boolVal(bool value);

YaraExpressionBuilder id(const std::string& id);
YaraExpressionBuilder paren(const YaraExpressionBuilder& other, bool linebreak = false);

YaraExpressionBuilder stringRef(const std::string& id);
YaraExpressionBuilder matchCount(const std::string& id);
YaraExpressionBuilder matchLength(const std::string& id);
YaraExpressionBuilder matchOffset(const std::string& id);
YaraExpressionBuilder matchLength(const std::string& id, const YaraExpressionBuilder& other);
YaraExpressionBuilder matchOffset(const std::string& id, const YaraExpressionBuilder& other);
YaraExpressionBuilder matchAt(const std::string& id, const YaraExpressionBuilder& other);
YaraExpressionBuilder matchInRange(const std::string& id, const YaraExpressionBuilder& other);

YaraExpressionBuilder forLoop(const YaraExpressionBuilder& forExpr, const std::string& id, const YaraExpressionBuilder& set, const YaraExpressionBuilder& expr);
YaraExpressionBuilder forLoop(const YaraExpressionBuilder& forExpr, const YaraExpressionBuilder& set, const YaraExpressionBuilder& expr);
YaraExpressionBuilder of(const YaraExpressionBuilder& ofExpr, const YaraExpressionBuilder& set);

YaraExpressionBuilder set(const std::vector<YaraExpressionBuilder>& elements);
YaraExpressionBuilder range(const YaraExpressionBuilder& low, const YaraExpressionBuilder& high);

YaraExpressionBuilder conjunction(const YaraExpressionBuilder& lhs, const YaraExpressionBuilder& rhs, bool linebreak = false);
YaraExpressionBuilder disjunction(const YaraExpressionBuilder& lhs, const YaraExpressionBuilder& rhs, bool linebreak = false);
YaraExpressionBuilder conjunction(const std::vector<YaraExpressionBuilder>& terms, bool linebreaks = false);
YaraExpressionBuilder disjunction(const std::vector<YaraExpressionBuilder>& terms, bool linebreaks = false);
YaraExpressionBuilder conjunction(const std::vector<std::pair<YaraExpressionBuilder, std::string>>& terms);
YaraExpressionBuilder disjunction(const std::vector<std::pair<YaraExpressionBuilder, std::string>>& terms);

YaraExpressionBuilder filesize();
YaraExpressionBuilder entrypoint();
YaraExpressionBuilder all();
YaraExpressionBuilder any();
YaraExpressionBuilder them();

YaraExpressionBuilder regexp(const std::string& text, const std::string& suffixMods = std::string{});
/// @}

}
