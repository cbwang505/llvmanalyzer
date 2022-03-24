#pragma once

#include <unordered_map>
#include <unordered_set>

#include <pog/automaton.h>
#include <pog/grammar.h>

namespace pog {

template <typename ValueT>
struct BacktrackingInfo
{
	const State<ValueT>* state;
	Item<ValueT> item;
};

template <typename ValueT, typename LeftT, typename RightT = LeftT>
class Relation
{
public:
	using AutomatonType = Automaton<ValueT>;
	using GrammarType = Grammar<ValueT>;

	Relation(const AutomatonType* automaton, const GrammarType* grammar) : _automaton(automaton), _grammar(grammar) {}
	Relation(const Relation&) = delete;
	Relation(Relation&&) noexcept = default;
	virtual ~Relation() = default;

	virtual void calculate() = 0;

	auto begin() { return _relation.begin(); }
	auto end() { return _relation.end(); }

	auto begin() const { return _relation.begin(); }
	auto end() const { return _relation.end(); }

	template <typename T>
	std::unordered_set<RightT>* find(const T& key)
	{
		auto itr = _relation.find(key);
		if (itr == _relation.end())
			return nullptr;

		return &itr->second;
	}

	template <typename T>
	const std::unordered_set<RightT>* find(const T& key) const
	{
		auto itr = _relation.find(key);
		if (itr == _relation.end())
			return nullptr;

		return &itr->second;
	}

protected:
	const AutomatonType* _automaton;
	const GrammarType* _grammar;
	std::unordered_map<LeftT, std::unordered_set<RightT>> _relation;
};

} // namespace pog
