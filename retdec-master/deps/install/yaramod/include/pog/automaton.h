#pragma once

#include <deque>
#include <memory>
#include <vector>

#include <pog/grammar.h>
#include <pog/state.h>
#include <pog/types/state_and_symbol.h>

namespace pog {

template <typename ValueT>
class Automaton
{
public:
	using GrammarType = Grammar<ValueT>;
	using ItemType = Item<ValueT>;
	using StateType = State<ValueT>;
	using SymbolType = Symbol<ValueT>;

	using StateAndSymbolType = StateAndSymbol<ValueT>;

	Automaton(const GrammarType* grammar) : _grammar(grammar), _states(), _state_to_index() {}

	const std::vector<std::unique_ptr<StateType>>& get_states() const { return _states; }

	const StateType* get_state(std::size_t index) const
	{
		assert(index < _states.size() && "Accessing state index out of bounds");
		return _states[index].get();
	}

	template <typename StateT>
	std::pair<StateType*, bool> add_state(StateT&& state)
	{
		auto itr = _state_to_index.find(&state);
		if (itr != _state_to_index.end())
			return {_states[itr->second].get(), false};

		_states.push_back(std::make_unique<StateType>(std::forward<StateT>(state)));
		_state_to_index.emplace(_states.back().get(), _states.size() - 1);
		return {_states.back().get(), true};
	}

	void closure(StateType& state)
	{
		std::deque<const ItemType*> to_process;
		for (const auto& item : state)
			to_process.push_back(item.get());

		while (!to_process.empty())
		{
			const auto* current_item = to_process.front();
			to_process.pop_front();

			const auto* next_symbol = current_item->get_read_symbol();
			auto rules = _grammar->get_rules_of_symbol(next_symbol);
			for (const auto* rule : rules)
			{
				auto new_item = Item{rule};
				auto result = state.add_item(std::move(new_item));
				if (result.second)
					to_process.push_back(result.first);
			}
		}
	}

	void construct_states()
	{
		StateType initial_state;
		initial_state.add_item(ItemType{_grammar->get_start_rule()});
		initial_state.set_index(0);
		closure(initial_state);
		auto result = add_state(std::move(initial_state));

		std::deque<StateType*> to_process{result.first};
		while (!to_process.empty())
		{
			auto* state = to_process.front();
			to_process.pop_front();

			std::map<const SymbolType*, StateType, SymbolLess<ValueT>> prepared_states;
			for (const auto& item : *state)
			{
				if (item->is_final())
					continue;

				auto next_sym = item->get_read_symbol();
				if (next_sym->is_end())
					continue;

				auto new_item = Item{*item};
				new_item.step();

				auto itr = prepared_states.find(next_sym);
				if (itr == prepared_states.end())
					std::tie(itr, std::ignore) = prepared_states.emplace(next_sym, StateType{});
				itr->second.add_item(std::move(new_item));
			}

			for (auto&& [symbol, prepared_state] : prepared_states)
			{
				prepared_state.set_index(static_cast<std::uint32_t>(_states.size()));
				auto result = add_state(std::move(prepared_state));
				auto* target_state = result.first;
				if (result.second)
				{
					// We calculate closure only if it's new state introduced in the automaton.
					// States can be compared only with their kernel items so it's better to just do it
					// once for each state.
					closure(*target_state);
					to_process.push_back(target_state);
				}
				state->add_transition(symbol, target_state);
				target_state->add_back_transition(symbol, state);
			}
		}
	}

	std::string generate_graph() const
	{
		std::vector<std::string> states_str(_states.size());
		std::transform(_states.begin(), _states.end(), states_str.begin(), [](const auto& state) {
			std::vector<std::string> items_str(state->size());
			std::transform(state->begin(), state->end(), items_str.begin(), [](const auto& item) {
				return item->to_string("→", "ε", "•");
			});
			return fmt::format("{} [label=\"{}\\l\", xlabel=\"{}\"]", state->get_index(), fmt::join(items_str.begin(), items_str.end(), "\\l"), state->get_index());
		});
		std::vector<std::string> edges_str;
		for (const auto& state : _states)
		{
			for (const auto& [sym, dest] : state->get_transitions())
			{
				edges_str.push_back(fmt::format("{} -> {} [label=\"{}\"]", state->get_index(), dest->get_index(), sym->get_name()));
			}
		}
		return fmt::format(R"(digraph Automaton {{
node [shape=rect];

{}

{}
}})",
			fmt::join(states_str.begin(), states_str.end(), "\n"),
			fmt::join(edges_str.begin(), edges_str.end(), "\n")
		);
	}

private:
	const GrammarType* _grammar;
	std::vector<std::unique_ptr<StateType>> _states;
	std::unordered_map<const StateType*, std::size_t, StateKernelHash<ValueT>, StateKernelEquals<ValueT>> _state_to_index;
};

} // namespace pog
