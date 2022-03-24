#pragma once

#include <cstdint>
#include <variant>

#include <pog/rule.h>
#include <pog/state.h>

namespace pog {

template <typename ValueT>
struct Shift
{
	const State<ValueT>* state;
};

template <typename ValueT>
struct Reduce
{
	const Rule<ValueT>* rule;
};

struct Accept {};

template <typename ValueT>
using Action = std::variant<Shift<ValueT>, Reduce<ValueT>, Accept>;

} // namespace pog
