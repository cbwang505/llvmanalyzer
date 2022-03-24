#pragma once

#include <exception>
#include <string>
#include <vector>

#include <fmt/format.h>

#include <pog/rule.h>
#include <pog/symbol.h>

namespace pog {

class Error : public std::exception
{
public:
	Error() : _msg() {}
	template <typename T>
	Error(T&& msg) noexcept : _msg(std::forward<T>(msg)) {}
	Error(const Error& o) noexcept : _msg(o._msg) {}
	virtual ~Error() noexcept {}

	virtual const char* what() const noexcept override { return _msg.c_str(); }

protected:
	std::string _msg;
};

class SyntaxError : public Error
{
public:
	template <typename T>
	SyntaxError(const Symbol<T>* unexpected_symbol, const std::vector<const Symbol<T>*>& expected_symbols) : Error()
	{
		std::vector<std::string> expected_symbols_str(expected_symbols.size());
		std::transform(expected_symbols.begin(), expected_symbols.end(), expected_symbols_str.begin(), [](const auto& sym) {
			return sym->get_description();
		});

		_msg = fmt::format(
			"Syntax error: Unexpected {}, expected one of {}",
			unexpected_symbol->get_description(),
			fmt::join(expected_symbols_str.begin(), expected_symbols_str.end(), ", ")
		);
	}

	template <typename T>
	SyntaxError(const std::vector<const Symbol<T>*>& expected_symbols) : Error()
	{
		std::vector<std::string> expected_symbols_str(expected_symbols.size());
		std::transform(expected_symbols.begin(), expected_symbols.end(), expected_symbols_str.begin(), [](const auto& sym) {
			return sym->get_description();
		});

		_msg = fmt::format(
			"Syntax error: Unknown symbol on input, expected one of {}",
			fmt::join(expected_symbols_str.begin(), expected_symbols_str.end(), ", ")
		);
	}
};

} // namespace pog
