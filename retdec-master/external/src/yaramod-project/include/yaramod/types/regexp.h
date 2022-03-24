/**
 * @file src/types/regexp.h
 * @brief Declaration of class Regexp.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <memory>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

#include <optional>

#include "yaramod/types/string.h"
#include "yaramod/types/token_stream.h"
#include "yaramod/utils/visitor.h"

namespace yaramod {

/**
 * Abstract class representing single unit in the regular expression.
 */
class RegexpUnit
{
public:
	RegexpUnit()
		: _tokenStream(std::make_shared<TokenStream>())
	{
	}
	virtual ~RegexpUnit() {}

	virtual std::string getText() const = 0;

	virtual RegexpVisitResult accept(RegexpVisitor* v) = 0;
	std::shared_ptr<TokenStream>&& getTokenStream() { return std::move(_tokenStream); }

protected:
	std::shared_ptr<TokenStream> _tokenStream;
};

/**
 * Class representing class unit in regular expressions. Class units
 * are enclosed in [] and can be either positive or negative (class starts with symbol ^, e.g. [^xyz]).
 */
class RegexpClass : public RegexpUnit
{
public:
	RegexpClass(const std::string& characters, bool negative = false)
	{
		_leftRectBracket = _tokenStream->emplace_back(TokenType::LSQB, "[");
		_negative = _tokenStream->emplace_back(TokenType::REGEXP_CLASS_NEGATIVE, negative, negative? "^" : std::string{});
		addCharacters(std::move(characters));
		_rightRectBracket = _tokenStream->emplace_back(TokenType::RSQB, "]");
	}

	virtual std::string getText() const override
	{
		std::ostringstream ss;
		ss << _leftRectBracket->getPureText();
		ss << _negative->getPureText();
		for (TokenIt it : _characters)
			ss << it->getPureText();
		ss << _rightRectBracket->getPureText();

		return ss.str();
	}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}

	void addCharacters(const std::string& text)
	{
		for (char c : text)
			_characters.push_back(_tokenStream->emplace_back(TokenType::REGEXP_CHAR, std::string(1, c)));
	}

	std::string getCharacters() const
	{
		std::stringstream ss;
		for (TokenIt it : _characters)
			ss << it->getPureText();
		return ss.str();
	}

	void setCharacters(const std::string& characters)
	{
		_characters.clear();
		addCharacters(characters);
	}

	bool isNegative() const { return _negative->getBool(); }

private:
	TokenIt _leftRectBracket; ///< '[' token
	std::vector<TokenIt> _characters; ///< Characters in the class
	TokenIt _negative; ///< Negative class
	TokenIt _rightRectBracket; ///< ']' token
};

/**
 * Class representing text unit in regular expression.
 * Text unit is sequence of characters with no special meaning.
 */
class RegexpText : public RegexpUnit
{
public:
	RegexpText(const std::string& text, bool storeAsOne = false)
	{
		if (storeAsOne)
			_characters.push_back(_tokenStream->emplace_back(TokenType::REGEXP_TEXT, text));
		else
			addCharacters(text);
	}

	virtual ~RegexpText() override {}

	virtual std::string getText() const override
	{
		std::string output;
		for (const auto& it : _characters)
			output += it->getPureText();
		return output;
	}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}

	void addCharacters(const std::string& text)
	{
		for (char c : text)
			_characters.push_back(_tokenStream->emplace_back(TokenType::REGEXP_CHAR, std::string(1, c)));
	}

private:
	std::vector<TokenIt> _characters; ///< Text
};

/**
 * Class representing unit for any character.
 * This is denoted as character @c . in regular expression.
 */
class RegexpAnyChar : public RegexpText
{
public:
	RegexpAnyChar() : RegexpText(".", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for word character.
 * This is denoted as character @c \\w in regular expression.
 */
class RegexpWordChar : public RegexpText
{
public:
	RegexpWordChar() : RegexpText("\\w", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for non-word character.
 * This is denoted as character @c \\W in regular expression.
 */
class RegexpNonWordChar : public RegexpText
{
public:
	RegexpNonWordChar() : RegexpText("\\W", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for space character.
 * This is denoted as character @c \\s in regular expression.
 */
class RegexpSpace : public RegexpText
{
public:
	RegexpSpace() : RegexpText("\\s", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for non-space character.
 * This is denoted as character @c \\S in regular expression.
 */
class RegexpNonSpace : public RegexpText
{
public:
	RegexpNonSpace() : RegexpText("\\S", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for digit.
 * This is denoted as character @c \\d in regular expression.
 */
class RegexpDigit : public RegexpText
{
public:
	RegexpDigit() : RegexpText("\\d", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for non-digit.
 * This is denoted as character @c \\D in regular expression.
 */
class RegexpNonDigit : public RegexpText
{
public:
	RegexpNonDigit() : RegexpText("\\D", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for word boundary.
 * This is denoted as character @c \\b in regular expression.
 */
class RegexpWordBoundary : public RegexpText
{
public:
	RegexpWordBoundary() : RegexpText("\\b", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for word boundary.
 * This is denoted as character @c \\B in regular expression.
 */
class RegexpNonWordBoundary : public RegexpText
{
public:
	RegexpNonWordBoundary() : RegexpText("\\B", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for start of line.
 * This is denoted as character @c ^ in regular expression.
 */
class RegexpStartOfLine : public RegexpText
{
public:
	RegexpStartOfLine() : RegexpText("^", true) {}
};

/**
 * Class representing unit for end of line.
 * This is denoted as character @c $ in regular expression.
 */
class RegexpEndOfLine : public RegexpText
{
public:
	RegexpEndOfLine() : RegexpText("$", true) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Abstract class representing unit for operation over certain text in the regular expression.
 * Operation can be either greedy or non-greedy denoted by @c ? at the end of the operation.
 */
class RegexpOperation : public RegexpUnit
{
public:
	virtual ~RegexpOperation() override {}

	virtual std::string getText() const override
	{
		assert(_operand != nullptr);
		return _operand->getText() + _operation->getString() + _greedy->getPureText();
	}

	virtual char getOperation() const { return _operation->getString()[0]; }

	bool isGreedy() const { return _greedy->getBool(); }

	const std::shared_ptr<RegexpUnit>& getOperand() const { return _operand; }

	void setOperand(const std::shared_ptr<RegexpUnit>& operand) { _operand = operand; }
	void setOperand(std::shared_ptr<RegexpUnit>&& operand) { _operand = std::move(operand); }

protected:
	RegexpOperation(TokenType operation_token_type, char operation_symbol, std::shared_ptr<RegexpUnit>&& operand, bool greedy)
		: _operand(std::move(operand))
	{
		//take the operand's tokenStream and append first the operation and then greedy
		_tokenStream = std::move(_operand->getTokenStream());
		_operation = _tokenStream->emplace_back(operation_token_type, std::string(1, operation_symbol));
		_greedy = _tokenStream->emplace_back(TokenType::REGEXP_GREEDY, greedy, greedy ? std::string() : "?");
	}

	RegexpOperation() = default;

protected:
	TokenIt _operation; ///< Operation character, char
	std::shared_ptr<RegexpUnit> _operand; ///< Operand of the operation
	TokenIt _greedy; ///< Greedy
};

/**
 * Class representing unit for operation of iteration.
 * This is denoted as character @c * in regular expression.
 */
class RegexpIteration : public RegexpOperation
{
public:
	RegexpIteration(std::shared_ptr<RegexpUnit>&& operand, bool greedy) : RegexpOperation(TokenType::REGEXP_ITER, '*', std::move(operand), greedy) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for operation of positive iteration.
 * This is denoted as character @c + in regular expression.
 */
class RegexpPositiveIteration : public RegexpOperation
{
public:
	RegexpPositiveIteration(std::shared_ptr<RegexpUnit>&& operand, bool greedy) : RegexpOperation(TokenType::REGEXP_PITER, '+', std::move(operand), greedy) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for operation of optional occurrence.
 * This is denoted as character @c ? in regular expression.
 */
class RegexpOptional : public RegexpOperation
{
public:
	RegexpOptional(std::shared_ptr<RegexpUnit>&& operand, bool greedy) : RegexpOperation(TokenType::REGEXP_OPTIONAL, '?', std::move(operand), greedy) {}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}
};

/**
 * Class representing unit for operation of ranged occurrence.
 * Ranged occurrence may be varying (range unspecified), fixed (N), varying ranged (N-*) and ranged (N-M).
 * This is denoted as @c {} in regular expression.
 */
class RegexpRange : public RegexpOperation
{
public:
	RegexpRange(std::shared_ptr<RegexpUnit>&& operand, std::pair<std::optional<std::uint64_t>, std::optional<std::uint64_t>>&& range, bool greedy)
	{
		assert(operand);
		_operand = std::move(operand);
		///  '{'  left  ','  right  '}'
		_tokenStream = std::move(_operand->getTokenStream());
		_leftBracket = _tokenStream->emplace_back(TokenType::LCB, '{');

		// If both start and end are defined and they are equal, it is fixed range.
		if (range.first && range.first == range.second)
		{
			_first = _tokenStream->emplace_back(TokenType::INTEGER, range.first.value());
			_operation = _tokenStream->emplace_back(TokenType::COMMA, std::string());
			_second = _first;
		}
		else
		{
			if (range.first)
				_first = _tokenStream->emplace_back(TokenType::INTEGER, range.first.value());
			_operation = _tokenStream->emplace_back(TokenType::COMMA, ',');
			if (range.second)
				_second = _tokenStream->emplace_back(TokenType::INTEGER, range.second.value());
		}

		_rightBracket = _tokenStream->emplace_back(TokenType::RCB, '}');
		_greedy = _tokenStream->emplace_back(TokenType::REGEXP_GREEDY, greedy, greedy ? std::string() : "?");
	}

	virtual char getOperation() const override { return ' '; }

	virtual std::string getText() const override
	{
		std::ostringstream ss;
		ss << _operand->getText();
		ss << _leftBracket->getPureText();

		if (_first && _first == _second)
			ss << (*_first)->getPureText();
		else
		{
			if (_first)
				ss << (*_first)->getPureText();
			ss << _operation->getPureText();
			if (_second)
				ss << (*_second)->getPureText();
		}
		ss << _rightBracket->getPureText();
		ss << _greedy->getPureText();

		return ss.str();
	}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}

	std::pair<std::optional<std::uint64_t>, std::optional<std::uint64_t>> getRange() const
	{
		std::optional<std::uint64_t> out1;
		std::optional<std::uint64_t> out2;
		if (_first)
			out1 = _first.value()->getUInt();
		if (_second)
			out2 = _second.value()->getUInt();
		return {std::move(out1), std::move(out2)};
	}

private:
	TokenIt _leftBracket; ///< '{' token
	std::optional<TokenIt> _first; ///< Lower bound of the range
	std::optional<TokenIt> _second; ///< Higher bound of the range
	TokenIt _rightBracket; ///< '}' token
};

/**
 * Class representing unit for or operation in regular expressions.
 * This operation cannot be greedy or non-greedy.
 * This is denoted as character @c | in regular expression.
 */
class RegexpOr : public RegexpUnit
{
public:
	RegexpOr(std::shared_ptr<RegexpUnit>&& left, std::shared_ptr<RegexpUnit>&& right)
		: _left(std::move(left))
		, _right(std::move(right))
	{
		_tokenStream = std::move(_left->getTokenStream());
		_or = _tokenStream->emplace_back(TokenType::REGEXP_OR, "|");
		_tokenStream->moveAppend(_right->getTokenStream().get());
	}

	virtual std::string getText() const override
	{
		return _left->getText() + _or->getPureText() + _right->getText();
	}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}

	const std::shared_ptr<RegexpUnit>& getLeft() const { return _left; }
	const std::shared_ptr<RegexpUnit>& getRight() const { return _right; }

private:
	TokenIt _or;
	std::shared_ptr<RegexpUnit> _left, _right; ///< Operands
};

/**
 * Class representing unit for group in regular expressions.
 * Groups are parts of regular expression enclosed in @c ().
 */
class RegexpGroup : public RegexpUnit
{
public:
	RegexpGroup(std::shared_ptr<RegexpUnit>&& unit)
		: _unit(std::move(unit))
	{
		_left_bracket = _tokenStream->emplace_back(TokenType::LP, "(");
		_tokenStream->moveAppend(_unit->getTokenStream().get());
		_right_bracket = _tokenStream->emplace_back(TokenType::RP, ")");
	}

	virtual std::string getText() const override
	{
		return _left_bracket->getPureText() + _unit->getText() + _right_bracket->getPureText();
	}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}

	const std::shared_ptr<RegexpUnit>& getUnit() const { return _unit; }

private:
	TokenIt _left_bracket; ///< '(' token
	std::shared_ptr<RegexpUnit> _unit; ///< Grouped units
	TokenIt _right_bracket; ///< ')' token
};

/**
 * Class representing concatenation in regular expressions.
 * This is not denoted explicitly in the regular expressions,
 * but it happens on the boundaries of two different units.
 */
class RegexpConcat : public RegexpUnit
{
public:
	RegexpConcat(std::vector<std::shared_ptr<RegexpUnit>>&& units)
		: _units(std::move(units))
	{
		for (auto&& unit : _units)
			_tokenStream->moveAppend(unit->getTokenStream().get());
	}

	virtual std::string getText() const override
	{
		std::string result;
		for (const auto& unit : _units)
			result += unit->getText();
		return result;
	}

	virtual RegexpVisitResult accept(RegexpVisitor* v) override
	{
		return v->visit(this);
	}

	const std::vector<std::shared_ptr<RegexpUnit>>& getUnits() const { return _units; }

	void setUnits(const std::vector<std::shared_ptr<RegexpUnit>>& units)
	{
		_units = units;
	}

	void setUnits(std::vector<std::shared_ptr<RegexpUnit>>&& units)
	{
		_units = std::move(units);
	}

private:
	std::vector<std::shared_ptr<RegexpUnit>> _units; ///< Concatenated units
};

/**
 * Class representing regular expressions in the strings section
 * of the YARA rules.
 *
 * For example:
 * @code
 * $1 = /md5: [0-9a-zA-Z]{32}/
 * $2 = /state: (on|off)/
 * @endcode
 */
class Regexp : public String
{
public:
	Regexp(const std::shared_ptr<TokenStream>& ts, std::shared_ptr<RegexpUnit>&& unit)
		: String(ts, String::Type::Regexp)
		, _unit(std::move(unit))
	{
		_leftSlash = _tokenStream->emplace_back(TokenType::REGEXP_START_SLASH, "/");
		_tokenStream->moveAppend(_unit->getTokenStream().get());
		_rightSlash = _tokenStream->emplace_back(TokenType::REGEXP_END_SLASH, "/");
	}

	Regexp(const std::shared_ptr<TokenStream>& ts, std::shared_ptr<RegexpUnit>&& unit, const std::string& id)
		: String(ts, String::Type::Regexp, id)
		, _unit(std::move(unit))
	{
		_leftSlash = _tokenStream->emplace_back(TokenType::REGEXP_START_SLASH, "/");
		_tokenStream->moveAppend(_unit->getTokenStream().get());
		_rightSlash = _tokenStream->emplace_back(TokenType::REGEXP_END_SLASH, "/");
	}

	virtual std::string getText() const override
	{
		return _leftSlash->getPureText() + getPureText() + _rightSlash->getPureText() + getSuffixModifiers() + getModifiersText();
	}

	virtual std::string getPureText() const override
	{
		return _unit->getText();
	}

	virtual TokenIt getFirstTokenIt() const override
	{
		return _leftSlash;
	}

	virtual TokenIt getLastTokenIt() const override
	{
		return _suffixMods.value_or(_rightSlash);
	}
	/**
	* Return regular expression suffix modifiers.
	*
	* For further information see https://perldoc.perl.org/perlre.html#Modifiers.
	* Allowed modifiers are 'i' and 's'.
	*
	* For example:
	* @code
	* $1 = /suffix_mod_i/i
	*                    ^
	* @endcode
	*/
	std::string getSuffixModifiers() const
	{
		if (!_suffixMods)
			return std::string{};
		else
			return (*_suffixMods)->getPureText();
	}

	void setSuffixModifiers(const std::string& suffixMods)
	{
		if (!suffixMods.empty())
		{
			if (_suffixMods.has_value())
				(*_suffixMods)->setValue(suffixMods);
			else
				_suffixMods = _tokenStream->emplace(std::next(_rightSlash), TokenType::REGEXP_MODIFIERS, suffixMods);
		}
	}

	const std::shared_ptr<RegexpUnit>& getUnit() const { return _unit; }

	void setUnit(const std::shared_ptr<RegexpUnit>& unit) { _unit = unit; }
	void setUnit(std::shared_ptr<RegexpUnit>&& unit) { _unit = std::move(unit); }

	void setSuffixModifiers(TokenIt suffixMods)
	{
		if (_suffixMods == suffixMods)
			return;
		else
		{
			if (_suffixMods.has_value())
				_tokenStream->erase(*_suffixMods);
			_suffixMods = suffixMods;
		}
	}

	std::shared_ptr<TokenStream>&& getTokenStream()
	{
		return std::move(_unit->getTokenStream());
	}

private:
	TokenIt _leftSlash; ///< right '/' token
	std::shared_ptr<RegexpUnit> _unit; ///< Unit defining other units in regular expression
	TokenIt _rightSlash; ///< left '/' token
	std::optional<TokenIt> _suffixMods; ///< Regular expression suffix modifiers, std::string
};

}
