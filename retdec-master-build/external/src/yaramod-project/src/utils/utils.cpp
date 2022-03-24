/**
 * @file src/utils/utils.cpp
 * @brief Implementation of utility functions.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <locale>
#include <optional>
#include <sstream>

#include "yaramod/utils/utils.h"

// Enforce C locale because if we are called from Python bindings
// UTF-8 locale seems to be used. Mixing default <cctype> functions
// with non-default locales seems to be not portable across different systems.
//
// Linux: worked as usual
// macOS: expected 'char' instead of 'unsigned char'
// Windows: asserted on signed types
static std::locale cLocale("C");

namespace yaramod {

/**
 * Checks whether the string is valid identifier for meta key or rule name.
 *
 * @return `true` if valid identifier, otherwise `false`.
 */
bool isValidIdentifier(const std::string& id)
{
	if (id.empty())
		return false;

	const char firstChar = id[0];
	if (!std::isalpha(firstChar, cLocale) && firstChar != '_')
		return false;

	return std::all_of(id.begin() + 1, id.end(),
			[](const char c) {
				return c == '_' || std::isalnum(c, cLocale);
			});
}

/**
 * Escapes the string according to the YARA escaping rules. Only escaping sequences are
 * `\n`, `\t`, `\"`, `\\` and `\xXX`.
 * non-printable characters are escaped: 				'\x40' -> "@"
 * printable characters are delegated separately: 	'\n' -> "\n"  ...  string of size 2
 *
 * @param str String to escape.
 *
 * @return Escaped string.
 */
std::string escapeString(const std::string& str)
{
	std::ostringstream writer;
	std::string result;

	for (auto itr = str.begin(), end = str.end(); itr != end; ++itr)
	{
		const char c = *itr;
		switch (c)
		{
			case '\n':
				result += "\\n";
				break;
			case '\t':
				result += "\\t";
				break;
			case '\\':
			case '\"':
				result += "\\";
				result += *itr;
				break;
			default:
				if (std::isprint(c, cLocale))
				{
					result += c;
				}
				else
				{
					writer.str({});
					writer.clear();
					// At first, we need to get rid of possible sign-extension so cast to uint8_t and then cast to integer type
					writer << std::setw(2) << std::setfill('0') << std::hex << static_cast<std::uint32_t>(static_cast<std::uint8_t>(c));
					result += "\\x" + writer.str();
				}
				break;
		}
	}

	return result;
}

std::optional<std::uint8_t> nibbleToByte(char nibble)
{
	if ('0' <= nibble && nibble <= '9')
		return nibble - '0';
	else if (char nl = std::tolower(nibble); 'a' <= nl && nl <= 'f')
		return nl - 'a' + 10;
	else
		return {};
}

/**
 * Unescapes the string according to the YARA escaping rules. Only escaping sequences are
 * `\n`, `\t`, `\"`, `\\` and `\xXX`.
 *
 * @param str String to unescape.
 *
 * @return unescaped string.
 */
std::string unescapeString(std::string_view str)
{
	if (str.length() < 2)
		return std::string{str.data(), str.length()};

	std::string result;
	result.reserve(str.length());

	std::size_t i;
	for (i = 0; i < str.length() - 1; ++i)
	{
		if (str[i] == '\\')
		{
			if (i + 3 < str.length() && str[i + 1] == 'x')
			{
				auto high = nibbleToByte(str[i + 2]);
				auto low = nibbleToByte(str[i + 3]);
				if (!high || !low)
					std::copy(str.begin() + i, str.begin() + i + 4, std::back_inserter(result));
				else
				{
					result.push_back((high.value() << 4) | low.value());
				}
				i += 3;
			}
			else if (str[i + 1] == '\\')
			{
				result.push_back('\\');
				++i;
			}
			else if (str[i + 1] == '\"')
			{
				result.push_back('\"');
				++i;
			}
			else if (str[i + 1] == 'n')
			{
				result.push_back('\n');
				++i;
			}
			else if (str[i + 1] == 'r')
			{
				result.push_back('\r');
				++i;
			}
			else if (str[i + 1] == 't')
			{
				result.push_back('\t');
				++i;
			}
			else
			{
				result.push_back(str[i]);
			}
		}
		else
		{
			result.push_back(str[i]);
		}
	}

	std::copy(str.begin() + i, str.end(), std::back_inserter(result));
	return result;
}

bool endsWith(const std::string& str, const std::string& withWhat)
{
	return (str.length() >= withWhat.length()) &&
		(str.compare(str.length() - withWhat.length(), withWhat.length(), withWhat) == 0);
}

bool endsWith(const std::string& str, char withWhat)
{
	return !str.empty() && str.back() == withWhat;
}

std::string trim(std::string str)
{
	// Based on
	// http://www.codeproject.com/Articles/10880/A-trim-implementation-for-std-string
	const std::string toTrim = " \n\r\t\v";
	std::string::size_type pos = str.find_last_not_of(toTrim);
	if (pos != std::string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(toTrim);
		if (pos != std::string::npos)
			str.erase(0, pos);
	}
	else
	{
		str.erase(str.begin(), str.end());
	}

	return str;
}

}
