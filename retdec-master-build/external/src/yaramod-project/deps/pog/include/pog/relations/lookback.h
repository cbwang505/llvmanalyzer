#pragma once

#include <pog/relations/relation.h>
#include <pog/types/state_and_rule.h>
#include <pog/types/state_and_symbol.h>

namespace pog {

/**
 * Lookback is a relation of tuples (q,R) where q is state and R is a rule from grammar and
 * tuples of (p,x) where p is state and x is symbol. Basically it's a relation on state and rule
 * with state and symbol.
 *
 * Let's imagine that state Q contains final item A -> x <*> where x is a sequence of terminals and
 * nonterminals. That means we need to perform production of rule A -> x and reduce x on the stack
 * into A. That also means that there is some state P with item B -> a <*> A b through which we had to
 * go into state Q. If it happens then (Q, A -> x) lookbacks (P, A).
 *
 * To put it simply, in order to get to state with final item A -> x <*>, we first had to go through
 * state with item B -> a <*> A b. We just simply put state with item A -> x <*> and rule A -> x into relation
 * with the origin state with item B -> a <*> A b and symbol A.
 *
 * This is useful for so-called propagation of lookaheads. If we know that rule A -> x is being used
 * and it all originated in certain state where rule B -> a A b is being processed, we can use what
 * can possible follow A in B -> a A b to know whether to use production of A -> x.
 */
template <typename ValueT>
class Lookback : public Relation<ValueT, StateAndRule<ValueT>, StateAndSymbol<ValueT>>
{
public:
	using Parent = Relation<ValueT, StateAndRule<ValueT>, StateAndSymbol<ValueT>>;

	using AutomatonType = Automaton<ValueT>;
	using BacktrackingInfoType = BacktrackingInfo<ValueT>;
	using GrammarType = Grammar<ValueT>;
	using StateType = State<ValueT>;
	using SymbolType = Symbol<ValueT>;

	using StateAndSymbolType = StateAndSymbol<ValueT>;
	using StateAndRuleType = StateAndRule<ValueT>;

	Lookback(const AutomatonType* automaton, const GrammarType* grammar) : Parent(automaton, grammar) {}
	Lookback(const Lookback&) = delete;
	Lookback(Lookback&&) noexcept = default;

	virtual void calculate() override
	{
		// Iterate over all states of LR automaton
		for (const auto& state : Parent::_automaton->get_states())
		{
			for (const auto& item : *state.get())
			{
				// We are not interested in items other than in form A -> x <*>
				if (!item->is_final())
					continue;

				// Get left-hand side symbol of a rule
				auto prod_symbol = item->get_rule()->get_lhs();

				// Now we'll start backtracking through LR automaton using backtransitions.
				// We'll basically just go in the different direction of arrows in the automata.
				// We know that we have item A -> x <*> so we know which backtransitions to take (those contained in sequence x).
				// There can be multiple transitions through the same symbol
				// going into current state so we'll put them into queue and process until queue is empty.
				std::unordered_set<const StateType*> visited_states;
				std::deque<BacktrackingInfoType> to_process;
				// Let's insert the current state and item A -> x <*> into the queue as a starting point
				to_process.push_back(BacktrackingInfoType{state.get(), *item.get()});
				while (!to_process.empty())
				{
					auto backtracking_info = std::move(to_process.front());
					to_process.pop_front();

					// If the state has transition over the symbol A, that means there is an item B -> a <*> A b
					if (backtracking_info.state->get_transitions().find(prod_symbol) != backtracking_info.state->get_transitions().end())
					{
						// Insert relation
						StateAndRuleType src_sr{state.get(), item->get_rule()};
						StateAndSymbolType dest_ss{backtracking_info.state, prod_symbol};
						auto itr = Parent::_relation.find(src_sr);
						if (itr == Parent::_relation.end())
							Parent::_relation.emplace(std::move(src_sr), std::unordered_set<StateAndSymbolType>{std::move(dest_ss)});
						else
							itr->second.insert(std::move(dest_ss));
					}

					// We've reached item with <*> at the start so we are no longer interested in it
					if (backtracking_info.item.get_read_pos() == 0)
						continue;

					// Observe backtransitions over the symbol left to the <*> in an item
					const auto& back_trans = backtracking_info.state->get_back_transitions();
					auto itr = back_trans.find(backtracking_info.item.get_previous_symbol());
					if (itr == back_trans.end())
						assert(false && "This shouldn't happen");

					// Perform step back of an item so that <*> in an item is moved one symbol to the left
					backtracking_info.item.step_back();
					for (const auto& dest_state : itr->second)
					{
						if (visited_states.find(dest_state) == visited_states.end())
						{
							// Put non-visited states from backtransitions into the queue
							to_process.push_back(BacktrackingInfoType{dest_state, backtracking_info.item});
							visited_states.emplace(dest_state);
						}
					}
				}
			}
		}
	}
};

} // namespace pog
