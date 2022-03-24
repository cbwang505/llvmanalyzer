#pragma once

#include <unordered_map>

#include <pog/action.h>
#include <pog/automaton.h>
#include <pog/errors.h>
#include <pog/grammar.h>
#include <pog/parser_report.h>
#include <pog/operations/lookahead.h>
#include <pog/state.h>
#include <pog/symbol.h>
#include <pog/types/state_and_rule.h>
#include <pog/types/state_and_symbol.h>

namespace pog {

template <typename ValueT>
class ParsingTable
{
public:
	using ActionType = Action<ValueT>;
	using ShiftActionType = Shift<ValueT>;
	using ReduceActionType = Reduce<ValueT>;

	using AutomatonType = Automaton<ValueT>;
	using GrammarType = Grammar<ValueT>;
	using RuleType = Rule<ValueT>;
	using StateType = State<ValueT>;
	using SymbolType = Symbol<ValueT>;

	using StateAndRuleType = StateAndRule<ValueT>;
	using StateAndSymbolType = StateAndSymbol<ValueT>;

	// TODO: Lookahead<> should be non-const but we need it for operator[]
	ParsingTable(const AutomatonType* automaton, const GrammarType* grammar, Lookahead<ValueT>& lookahead_op)
		: _automaton(automaton), _grammar(grammar), _lookahead_op(lookahead_op) {}

	void calculate(ParserReport<ValueT>& report)
	{
		for (const auto& state : _automaton->get_states())
		{
			if (state->is_accepting())
				add_accept(state.get(), _grammar->get_end_of_input_symbol());

			for (const auto& [sym, dest_state] : state->get_transitions())
				add_state_transition(report, state.get(), sym, dest_state);

			for (const auto& item : state->get_production_items())
			{
				for (const auto& sym : _lookahead_op[StateAndRuleType{state.get(), item->get_rule()}])
					add_reduction(report, state.get(), sym, item->get_rule());
			}
		}
	}

	void add_accept(const StateType* state, const SymbolType* symbol)
	{
		auto ss = StateAndSymbolType{state, symbol};
		auto itr = _action_table.find(ss);
		if (itr != _action_table.end())
			assert(false && "Conflict happened in placing accept but this shouldn't happen");

		_action_table.emplace(std::move(ss), Accept{});
	}

	void add_state_transition(ParserReport<ValueT>& report, const StateType* src_state, const SymbolType* symbol, const StateType* dest_state)
	{
		auto ss = StateAndSymbolType{src_state, symbol};
		if (symbol->is_terminal())
		{
			auto itr = _action_table.find(ss);
			if (itr != _action_table.end())
			{
				if (std::holds_alternative<ReduceActionType>(itr->second))
					report.add_shift_reduce_conflict(ss.state, ss.symbol, std::get<ReduceActionType>(itr->second).rule);
			}
			else
				_action_table.emplace(std::move(ss), ShiftActionType{dest_state});
		}
		else if (symbol->is_nonterminal())
		{
			auto itr = _goto_table.find(ss);
			if (itr != _goto_table.end())
				assert(false && "Conflict happened in filling GOTO table but this shouldn't happen");

			_goto_table.emplace(std::move(ss), dest_state);
		}
	}

	void add_reduction(ParserReport<ValueT>& report, const StateType* state, const SymbolType* symbol, const RuleType* rule)
	{
		auto ss = StateAndSymbolType{state, symbol};
		auto itr = _action_table.find(ss);
		if (itr != _action_table.end())
		{
			std::optional<Precedence> stack_prec;
			if (rule->has_precedence())
				stack_prec = rule->get_precedence();
			else if (auto op_symbol = rule->get_rightmost_terminal(); op_symbol && op_symbol->has_precedence())
				stack_prec = op_symbol->get_precedence();

			if (stack_prec.has_value() && symbol->has_precedence())
			{
				const auto& input_prec = symbol->get_precedence();
				// Stack symbol precedence is lower, keep shift in the table
				if (stack_prec < input_prec)
				{
					return;
				}
				// Stack symbol precedence is greater, prefer reduce
				else if (stack_prec > input_prec)
				{
					itr->second = ReduceActionType{rule};
					return;
				}
			}

			if (std::holds_alternative<ReduceActionType>(itr->second))
				report.add_reduce_reduce_conflict(ss.state, std::get<ReduceActionType>(itr->second).rule, rule);
			else if (std::holds_alternative<ShiftActionType>(itr->second))
				report.add_shift_reduce_conflict(ss.state, ss.symbol, rule);
		}
		else
			_action_table.emplace(std::move(ss), ReduceActionType{rule});
	}

	std::optional<ActionType> get_action(const StateType* state, const SymbolType* symbol) const
	{
		auto action_itr = _action_table.find(StateAndSymbolType{state, symbol});
		if (action_itr == _action_table.end())
			return std::nullopt;

		return action_itr->second;
	}

	std::optional<const StateType*> get_transition(const StateType* state, const SymbolType* symbol) const
	{
		auto goto_itr = _goto_table.find(StateAndSymbolType{state, symbol});
		if (goto_itr == _goto_table.end())
			return std::nullopt;

		return goto_itr->second;
	}

	std::vector<const SymbolType*> get_expected_symbols_from_state(const StateType* state) const
	{
		std::vector<const SymbolType*> result;
		for (const auto& sym : _grammar->get_symbols())
		{
			if (sym->is_nonterminal())
				continue;

			StateAndSymbolType ss{state, sym.get()};
			if (auto itr = _action_table.find(ss); itr != _action_table.end())
				result.push_back(sym.get());
		}

		return result;
	}

private:
	const AutomatonType* _automaton;
	const GrammarType* _grammar;
	std::unordered_map<StateAndSymbolType, ActionType> _action_table;
	std::unordered_map<StateAndSymbolType, const StateType*> _goto_table;
	Lookahead<ValueT>& _lookahead_op;
};

} // namespace pog
