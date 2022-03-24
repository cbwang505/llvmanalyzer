/**
 * @file src/types/plain_string.h
 * @brief Declaration of class PlainString.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/string.h"

namespace yaramod {

/**
 * Class representing plain strings in the strings section
 * of the YARA rules.
 *
 * For example:
 * @code
 * $hello = "Hello World!"
 * $bye = "Bye World!" wide
 * @endcode
 */
class PlainString : public String
{
public:
	/// @name Constructors
	/// @{
	explicit PlainString(const std::shared_ptr<TokenStream>& ts, const std::string& text);
	explicit PlainString(const std::shared_ptr<TokenStream>& ts, std::string&& text);
	explicit PlainString(const std::shared_ptr<TokenStream>& ts, TokenIt text);
	explicit PlainString(const std::shared_ptr<TokenStream>& ts, TokenIt id, TokenIt assignToken, TokenIt text);
	~PlainString() = default;
	/// @}

	/// @name Virtual methods
	/// @{
	virtual std::string getText() const override;
	virtual std::string getPureText() const override;
	virtual TokenIt getFirstTokenIt() const override;
	virtual TokenIt getLastTokenIt() const override;
	/// @}

private:
	TokenIt _text; ///< Text of the plain string
};

}
