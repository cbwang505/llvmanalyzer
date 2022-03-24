#pragma once

#include <unordered_map>
#include <unordered_set>

#include <pog/automaton.h>
#include <pog/grammar.h>

namespace pog {

template <typename ValueT, typename ArgT, typename ResT>
class Operation
{
public:
	using AutomatonType = Automaton<ValueT>;
	using GrammarType = Grammar<ValueT>;

	Operation(const AutomatonType* automaton, const GrammarType* grammar) : _automaton(automaton), _grammar(grammar) {}
	Operation(const Operation&) = delete;
	Operation(Operation&&) noexcept = default;
	virtual ~Operation() = default;

	virtual void calculate() = 0;

	auto& operator[](const ArgT& key) { return _operation[key]; }
	auto& operator[](ArgT& key) { return _operation[key]; }

	template <typename T>
	std::unordered_set<ResT>* find(const T& key)
	{
		auto itr = _operation.find(key);
		if (itr == _operation.end())
			return nullptr;

		return &itr->second;
	}

	template <typename T>
	const std::unordered_set<ResT>* find(const T& key) const
	{
		auto itr = _operation.find(key);
		if (itr == _operation.end())
			return nullptr;

		return &itr->second;
	}

protected:
	const AutomatonType* _automaton;
	const GrammarType* _grammar;
	std::unordered_map<ArgT, std::unordered_set<ResT>> _operation;
};

} // namespace pog
