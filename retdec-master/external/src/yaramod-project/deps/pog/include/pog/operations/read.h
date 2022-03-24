#pragma once

#include <pog/operations/operation.h>
#include <pog/types/state_and_symbol.h>

namespace pog {

/**
 * Read operations maps (q,x) where q is state and x is symbol into set of symbols.
 *
 * Let's take state Q and non-final item A -> a <*> B b. Read(Q, B) represents set
 * of symbols we can possibly read after reading B while in state Q. Originally,
 * in all papers you see this proposed to be calculated using DirectRead() function
 * and reads relation but in the end, it's the same as if we do First(b). First(b)
 * incorporates situation if some symbol in sequence b can be reduced to empty string.
 *
 * So to put it shortly, for state Q and item A -> a <*> B b, Read(Q, B) = First(b).
 */
template <typename ValueT>
class Read : public Operation<ValueT, StateAndSymbol<ValueT>, const Symbol<ValueT>*>
{
public:
	using Parent = Operation<ValueT, StateAndSymbol<ValueT>, const Symbol<ValueT>*>;

	using AutomatonType = Automaton<ValueT>;
	using GrammarType = Grammar<ValueT>;
	using SymbolType = Symbol<ValueT>;

	using StateAndSymbolType = StateAndSymbol<ValueT>;

	Read(const AutomatonType* automaton, const GrammarType* grammar) : Parent(automaton, grammar) {}
	Read(const Read&) = delete;
	Read(Read&&) noexcept = default;

	virtual void calculate() override
	{
		// Iterate over all states of LR automaton
		for (const auto& state : Parent::_automaton->get_states())
		{
			for (const auto& item : *state.get())
			{
				// We don't care about final items, only those in form A -> a <*> B b
				if (item->is_final())
					continue;

				// Symbol right to <*> needs to be nonterminal
				auto next_symbol = item->get_read_symbol();
				if (!next_symbol->is_nonterminal())
					continue;

				// Observe everything right of B, so in this case 'b' and calculate First()
				auto right_rest = item->get_right_side_without_read_symbol();
				auto symbols = Parent::_grammar->first(right_rest);

				// Insert operation result
				auto ss = StateAndSymbolType{state.get(), next_symbol};
				auto itr = Parent::_operation.find(ss);
				if (itr == Parent::_operation.end())
					Parent::_operation.emplace(std::move(ss), std::move(symbols));
				else
				{
					// TODO: std::vector + std::set_union or std::unordered_set::extract
					std::copy(symbols.begin(), symbols.end(), std::inserter(itr->second, itr->second.begin()));
				}
			}
		}
	}
};

} // namespace pog
