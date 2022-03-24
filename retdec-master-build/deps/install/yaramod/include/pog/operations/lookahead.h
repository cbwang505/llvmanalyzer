#pragma once

#include <pog/operations/operation.h>
#include <pog/operations/follow.h>
#include <pog/relations/lookback.h>
#include <pog/types/state_and_rule.h>

namespace pog {

/**
 * Lookahead operation maps (q, R), where q is state and R is rule from grammar, to set of symbols.
 *
 * Formal definition for Lookahead(q, A -> x) is
 *   Lookahead(q, A -> x) = union { Follow(p, B) | (q, A -> x) lookback (p, B) }
 * So it's union of all Follow sets of state p and symbol B such that (q, A -> x) is in lookback
 * relation with (p, B).
 *
 * To put it simply:
 * 1. Follow set of (p, B) represents what symbols can follow symbol B when we are in the state B.
 * 2. Lookback relation represents that in order to preform some reduction A -> x in state q, we first
 *    had to go through some other state p and use what follows A in B -> a A b to know when to perform
 *    reduction.
 * So we'll take all rules A -> x and find in which state they can be reduced (there is an item A -> x <*>).
 * We'll then union all Follow() sets according to lookback relation and for each state and rule, we now
 * know what symbols need to follow in order to perform reductions by such rule in that particular state.
 */
template <typename ValueT>
class Lookahead : public Operation<ValueT, StateAndRule<ValueT>, const Symbol<ValueT>*>
{
public:
	using Parent = Operation<ValueT, StateAndRule<ValueT>, const Symbol<ValueT>*>;

	using AutomatonType = Automaton<ValueT>;
	using GrammarType = Grammar<ValueT>;

	using StateAndRuleType = StateAndRule<ValueT>;

	// TODO: Follow<> should not be non-const but we need it for operator[]
	Lookahead(const AutomatonType* automaton, const GrammarType* grammar, const Lookback<ValueT>& lookback, Follow<ValueT>& follow_op)
		: Parent(automaton, grammar), _lookback(lookback), _follow_op(follow_op) {}
	Lookahead(const Lookahead&) = delete;
	Lookahead(Lookahead&&) noexcept = default;

	virtual void calculate() override
	{
		// Iterate over all rules in grammar
		for (const auto& rule : Parent::_grammar->get_rules())
		{
			for (const auto& state : Parent::_automaton->get_states())
			{
				// Find lookback of the current state and rule
				auto sr = StateAndRuleType{state.get(), rule.get()};
				auto lookback_with = _lookback.find(sr);
				if (!lookback_with)
					continue;

				// Union all Follow() sets of the current state and rule to compute Lookahead()
				for (const auto& ss : *lookback_with)
				{
					if (auto itr = Parent::_operation.find(sr); itr == Parent::_operation.end())
						Parent::_operation.emplace(std::move(sr), _follow_op[ss]);
					else if (auto follow_res = _follow_op.find(ss); follow_res)
						std::copy(follow_res->begin(), follow_res->end(), std::inserter(itr->second, itr->second.begin()));
				}
			}
		}

	}

private:
	const Lookback<ValueT>& _lookback;
	Follow<ValueT>& _follow_op;
};

} // namespace pog
