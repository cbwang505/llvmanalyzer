#pragma once

#include <pog/digraph_algo.h>
#include <pog/operations/operation.h>
#include <pog/operations/read.h>
#include <pog/relations/includes.h>
#include <pog/types/state_and_symbol.h>

namespace pog {

/**
 * Follow operations maps (q,x) where q is state and x is symbol to set of symbols.
 *
 * Formal definition for what Follow() represents is
 *   Follow(q, A) = Read(q, A) union (union { Follow(p, B) | (q, A) includes (p, B) })
 * So Follow(q, A) represents Read(q, A) with union of all Follow sets of (p, B) such that
 * (q, A) is in include relation with (p, B).
 *
 * To put it simply by individual parts:
 * 1. Read(q, A) means what symbols can directly follow A in state q. See Read() operation for
 *    more information.
 * 2. Includes relation of (q, A) with (p, B) means that when we are about to read A in state q
 *    and everything after A can be empty string, meaning that what can follow B in state p can
 *    also follow A in state q.
 *
 * So Follow(q, A) represents what symbols can follow A while in state q. It is constructed from
 * generated follow using Read(q, A) and propagated follow using include relation.
 */
template <typename ValueT>
class Follow : public Operation<ValueT, StateAndSymbol<ValueT>, const Symbol<ValueT>*>
{
public:
	using Parent = Operation<ValueT, StateAndSymbol<ValueT>, const Symbol<ValueT>*>;

	using AutomatonType = Automaton<ValueT>;
	using GrammarType = Grammar<ValueT>;

	using StateAndSymbolType = StateAndSymbol<ValueT>;

	Follow(const AutomatonType* automaton, const GrammarType* grammar, const Includes<ValueT>& includes, Read<ValueT>& read_op)
		: Parent(automaton, grammar), _includes(includes), _read_op(read_op) {}
	Follow(const Follow&) = delete;
	Follow(Follow&&) noexcept = default;

	virtual void calculate() override
	{
		// We use digraph algorithm which calculates Follow() for us. See digraph_algo() for more information.
		digraph_algo<StateAndSymbolType>(_includes, _read_op, Parent::_operation);
	}

private:
	const Includes<ValueT>& _includes;
	Read<ValueT>& _read_op;
};

} // namespace pog
