#pragma once

#include <pog/relations/relation.h>
#include <pog/types/state_and_symbol.h>

namespace pog {

/**
 * Includes is a relation on tuples of (q,x) where q is state and x is symbol.
 * Basically it's a relation on transitions over symbols coming out from states.
 *
 * Let's imagine that state Q contains item A -> a <*> B b where a and b are sequences
 * of terminals and nonterminals. We had to get to this state Q from state P which
 * contains item A -> <*> a B b. If and only if b can be reduced down to empty string (b =>* eps),
 * we say that (Q, B) includes (P, A).
 *
 * To put this into human-friendly language, if we are in a state with item A -> a <*> B b,
 * that means we are about to read B from the input. If the sequence b can possibly be empty,
 * then we say that the state with item A -> a <*> B b and symbol B includes state with
 * item A -> <*> a B b and symbol A.
 *
 * This is useful for construction of Follow sets because if b can be completely empty, then
 * what can follow A can also follow B (with respect to states they are in).
 */
template <typename ValueT>
class Includes : public Relation<ValueT, StateAndSymbol<ValueT>>
{
public:
	using Parent = Relation<ValueT, StateAndSymbol<ValueT>>;

	using AutomatonType = Automaton<ValueT>;
	using BacktrackingInfoType = BacktrackingInfo<ValueT>;
	using GrammarType = Grammar<ValueT>;
	using StateType = State<ValueT>;
	using SymbolType = Symbol<ValueT>;

	using StateAndSymbolType = StateAndSymbol<ValueT>;

	Includes(const AutomatonType* automaton, const GrammarType* grammar) : Parent(automaton, grammar) {}
	Includes(const Includes&) = delete;
	Includes(Includes&&) noexcept = default;

	virtual void calculate() override
	{
		// Iterate over all states in the LR automaton
		for (const auto& state : Parent::_automaton->get_states())
		{
			for (const auto& item : *state.get())
			{
				// We are looking for items in form A -> a <*> B b so we are not intersted in final items
				if (item->is_final())
					continue;

				// Get the symbol right next to <*> in an item
				auto next_symbol = item->get_read_symbol();

				// If the next symbol is not nonterminal then we are again not interested
				if (!next_symbol->is_nonterminal())
					continue;

				StateAndSymbolType src_ss{state.get(), next_symbol};
				Parent::_relation.emplace(src_ss, std::unordered_set<StateAndSymbolType>{});

				// Get the 'b' out of A -> a <*> B b
				// If b can't be reduced down to empty string - Empty(b) - then we are not interested
				auto right_rest = item->get_right_side_without_read_symbol();
				if (!right_rest.empty() && !Parent::_grammar->empty(right_rest))
					continue;

				// Now we'll start backtracking through LR automaton using backtransitions.
				// We'll basically just go in the different direction of arrows in the automata.
				// We know of what symbols 'a' in A -> a <*> B b is made of so we exactly know which
				// backtransitions to take. There can be multiple transitions through the same symbol
				// going into current state so we'll put them into queue and process until queue is empty.
				std::unordered_set<const StateType*> visited_states;
				std::deque<BacktrackingInfoType> to_process;
				// Let's insert the current state and item A -> a <*> B b into the queue as a starting point
				to_process.push_back(BacktrackingInfoType{state.get(), *item.get()});
				while (!to_process.empty())
				{
					auto backtracking_info = std::move(to_process.front());
					to_process.pop_front();

					// If we've reached state with item A -> <*> a B b, we've reached our destination
					if (backtracking_info.item.get_read_pos() == 0)
					{
						// Insert relation
						StateAndSymbolType dest_ss{backtracking_info.state, backtracking_info.item.get_rule()->get_lhs()};
						auto itr = Parent::_relation.find(src_ss);
						if (itr == Parent::_relation.end())
							assert(false && "This shouldn't happen");
						itr->second.insert(std::move(dest_ss));
						continue;
					}

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

	std::string generate_relation_graph()
	{
		std::vector<std::string> states_str, edges_str;
		for (const auto& [ss, dests] : Parent::_relation)
		{
			states_str.push_back(fmt::format("n_{}_{} [label=\"({}, {})\"]", ss.state->get_index(), ss.symbol->get_index(), ss.state->get_index(), ss.symbol->get_name()));
			for (const auto& dest_ss : dests)
			{
				states_str.push_back(fmt::format("n_{}_{} [label=\"({}, {})\"]", dest_ss.state->get_index(), dest_ss.symbol->get_index(), dest_ss.state->get_index(), dest_ss.symbol->get_name()));
				edges_str.push_back(fmt::format("n_{}_{} -> n_{}_{}", ss.state->get_index(), ss.symbol->get_index(), dest_ss.state->get_index(), dest_ss.symbol->get_index()));
			}
		}

		return fmt::format(R"(digraph Includes {{
node [shape=circle];

{}

{}
}})",
			fmt::join(states_str.begin(), states_str.end(), "\n"),
			fmt::join(edges_str.begin(), edges_str.end(), "\n")
		);
	}
};

} // namespace pog
