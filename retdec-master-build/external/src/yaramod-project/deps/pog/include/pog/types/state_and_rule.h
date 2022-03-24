#pragma once

#include <pog/rule.h>
#include <pog/state.h>

namespace pog {

template <typename ValueT>
struct StateAndRule
{
	const State<ValueT>* state;
	const Rule<ValueT>* rule;

	bool operator==(const StateAndRule& rhs) const
	{
		return state->get_index() == rhs.state->get_index() && rule->get_index() == rhs.rule->get_index();
	}

	bool operator!=(const StateAndRule& rhs) const
	{
		return !(*this == rhs);
	}
};

} // namespace pog

namespace std {

template <typename ValueT>
struct hash<pog::StateAndRule<ValueT>>
{
	std::size_t operator()(const pog::StateAndRule<ValueT>& sr) const
	{
		return pog::hash_combine(sr.state->get_index(), sr.rule->get_index());
	}
};

}
