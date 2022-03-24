/**
 * @file src/types/token_stream.h
 * @brief Declaration of class TokenStream.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <list>
#include <set>
#include <vector>

#include "yaramod/types/token.h"

namespace yaramod {

/**
 * A container storing all parsed tokens. Formated text of the parsed input is accessible with getText method or <<operator.
 */
class TokenStream
{
public:
	/**
	 * Class serves as a context storage for printing TokenStream and determining comment alignment.
	 */
	class PrintHelper
	{
	public:
		std::size_t getCurrentLine() const { return lineCounter; }
		const std::vector<TokenIt>& getCommentPool() const { return commentPool; }

		std::size_t insertIntoStream(std::stringstream* ss, char what);
		std::size_t insertIntoStream(std::stringstream* ss, const std::string& what, std::size_t length = 0);
		std::size_t insertIntoStream(std::stringstream* ss, TokenStream* ts, TokenIt what);
		std::size_t printComment(std::stringstream* ss, TokenStream* ts, TokenIt it, size_t currentLineTabs, bool alignComment, bool ignoreUserIndent);
	private:
		std::size_t lineCounter = 0;
		std::size_t columnCounter = 0;
		bool commentOnThisLine = false;
		std::size_t maximalCommentColumn = 0;
		std::vector<TokenIt> commentPool;
	};

	TokenStream() = default;
	TokenStream(const TokenStream &ts) = delete;

	/// @name Insertion methods
	/// @{
	TokenIt emplace_back(TokenType type, char value);
	template <typename Value, typename T>
	TokenIt emplace_back(TokenType type, Value&& value, T&& formatted_value)
	{
		_tokens.emplace_back(type, Literal(std::forward<Value>(value), std::forward<T>(formatted_value)));
		return --_tokens.end();
	}
	template <typename Value>
	TokenIt emplace_back(TokenType type, Value&& value)
	{
		_tokens.emplace_back(type, Literal(std::forward<Value>(value)));
		return --_tokens.end();
	}
	TokenIt emplace_back(TokenType type, const Literal& literal);
	TokenIt emplace_back(TokenType type, Literal&& literal);

	TokenIt emplace(const TokenIt& before, TokenType type, char value);
	template <typename Value, typename T>
	TokenIt emplace(const TokenIt& before, TokenType type, Value&& value, T&& formatted_value)
	{
		_tokens.emplace(before, type, Literal(std::forward<Value>(value), std::forward<T>(formatted_value)));
		auto output = before;
		return --output;
	}
	template <typename Value>
	TokenIt emplace(const TokenIt& before, TokenType type, Value&& value)
	{
		_tokens.emplace(before, type, Literal(std::forward<Value>(value)));
		auto output = before;
		return --output;
	}
	TokenIt emplace(const TokenIt& before, TokenType type, const Literal& literal);
	TokenIt emplace(const TokenIt& before, TokenType type, Literal&& literal);

	TokenIt push_back(const Token& t);
	TokenIt push_back(Token&& t);
	TokenIt insert(TokenIt before, TokenType type, const Literal& literal);
	TokenIt insert(TokenIt before, TokenType type, Literal&& literal);
	TokenIt erase(TokenIt element);
	TokenIt erase(TokenIt first, TokenIt last);

	// Steals all data from donor and append it at the end.
	void moveAppend(TokenStream* donor);
	// Steals all data from donor and append it at position before.
	void moveAppend(TokenIt before, TokenStream* donor);
	// Steals only data in [first, last) from donor and append it at the end.
	void moveAppend(TokenStream* donor, TokenIt first, TokenIt last);
	// Steals only data in [first, last) from donor and append it at position before.
	void moveAppend(TokenIt before, TokenStream* donor, TokenIt first, TokenIt last);
	// Exchanges data in [local_first, local_last) for data in [other_first,other_last)
	// When other == this and [local_first, local_last) > [other_first,other_last), deletion occurs
	void swapTokens(TokenIt local_first, TokenIt local_last, TokenStream* other, TokenIt other_first, TokenIt other_last);
	/// @}

	/// @name Element access
	/// @{
	const Token& front() const { return _tokens.front(); }
	const Token& back() const { return _tokens.back(); }
	const std::list<Token>& getTokens() const { return _tokens; }
	/// @}

	/// @name Iterators
	/// @{
	TokenIt begin();
	TokenIt end();
	TokenConstIt begin() const;
	TokenConstIt end() const;
	TokenItReversed rbegin();
	TokenItReversed rend();
	TokenConstItReversed rbegin() const;
	TokenConstItReversed rend() const;
	/// @}

	/// @name Capacity
	/// @{
	std::size_t size() const;
	bool empty() const;
	/// @}

	/// @name Lookaround methods
	/// @{
	TokenIt find(TokenType type);
	TokenIt find(TokenType type, TokenIt from);
	TokenIt find(TokenType type, TokenIt from, TokenIt to);
	TokenIt find(const std::set<TokenType>& types, TokenIt from, TokenIt to);
	TokenIt findBackwards(TokenType type);
	TokenIt findBackwards(TokenType type, TokenIt to);
	TokenIt findBackwards(TokenType type, TokenIt from, TokenIt to);
	TokenIt findBackwards(const std::set<TokenType>& types, TokenIt from, TokenIt to);
	/// @}

	/// @name Text representation
	/// @{
	friend std::ostream& operator<<(std::ostream& os, TokenStream& ts) { return os << ts.getText(false); }
	std::string getText(bool withIncludes = false, bool alignComments = true);
	std::vector<std::string> getTokensAsText() const;
	/// @}

	/// @name New Line Characters
	/// @{
	const std::string& getNewLineStyle() const { return _new_line_style; }
	void setNewLineChar(std::string line) { _new_line_style = std::move(line); }
	/// @}

	/// @name Reseting method
	void clear();
	/// @}
protected:
	void getTextProcedure(PrintHelper& helper, std::stringstream* os, bool withIncludes, bool alignComments);
	void autoformat();
	bool determineNewlineSectors();
	void removeRedundantDoubleNewlines();
	void addMissingNewLines();

	std::optional<TokenIt> predecessor(TokenIt it);
private:
	std::list<Token> _tokens; ///< All tokens off the rule
	bool _formatted = false; ///< The flag is set once autoformat has been called
	std::string _new_line_style = "\n"; ///< The character used for line endings: usually '\n' on Unix or '\r' on MacOs or '\r\n' on Windows
};

} //namespace yaramod
