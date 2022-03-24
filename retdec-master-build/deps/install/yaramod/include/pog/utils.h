#pragma once

#include <ctime>
#include <functional>
#include <iomanip>
#include <sstream>
#include <variant>
#include <utility>

namespace pog {

template <typename InputIterator, typename OutputIterator, typename UnaryOperator, typename Pred>
OutputIterator transform_if(InputIterator first1, InputIterator last1, OutputIterator result, Pred pred, UnaryOperator op)
{
	while (first1 != last1)
	{
		if (pred(*first1)) {
			*result = op(*first1);
			++result;
		}
		++first1;
	}
	return result;
}

template <typename InputIterator, typename T, typename BinaryOperation, typename Pred>
T accumulate_if(InputIterator first, InputIterator last, T init, Pred pred, BinaryOperation op)
{
	for (; first != last; ++first)
	{
		if (pred(*first))
			init = op(std::move(init), *first);
	}
	return init;
}

inline void hash_combine(std::size_t&) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
	hash_combine(seed, rest...);
}

template <typename... Rest>
inline std::size_t hash_combine(const Rest&... rest)
{
	std::size_t seed = 0;
	hash_combine(seed, rest...);
	return seed;
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename Variant, typename... Fs>
auto visit_with(Variant& v, Fs&&... fs)
{
	return std::visit(overloaded{
		std::forward<Fs>(fs)...
	}, v);
}

template <typename FormatT>
inline std::string current_time(FormatT&& format)
{
	auto now = std::time(nullptr);
	auto tm = std::localtime(&now);

	std::ostringstream ss;
	ss << std::put_time(tm, std::forward<FormatT>(format));
	return ss.str();
}

} // namespace pog
