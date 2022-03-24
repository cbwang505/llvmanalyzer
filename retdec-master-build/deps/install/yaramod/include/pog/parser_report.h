#pragma once

#include <variant>
#include <vector>

#include <pog/rule.h>
#include <pog/state.h>

namespace pog {

template <typename ValueT>
struct ShiftReduceConflict
{
	const State<ValueT>* state;
	const Symbol<ValueT>* symbol;
	const Rule<ValueT>* rule;

	std::string to_string(std::string_view arrow = "->", std::string_view eps = "<eps>") const
	{
		return fmt::format("Shift-reduce conflict of symbol \'{}\' and rule \'{}\' in state {}", symbol->get_name(), rule->to_string(arrow, eps), state->get_index());
	}
};

template <typename ValueT>
struct ReduceReduceConflict
{
	const State<ValueT>* state;
	const Rule<ValueT>* rule1;
	const Rule<ValueT>* rule2;

	std::string to_string(std::string_view arrow = "->", std::string_view eps = "<eps>") const
	{
		return fmt::format("Reduce-reduce conflict of rule \'{}\' and rule \'{}\' in state {}", rule1->to_string(arrow, eps), rule2->to_string(arrow, eps), state->get_index());
	}
};

template <typename ValueT>
using Issue = std::variant<ShiftReduceConflict<ValueT>, ReduceReduceConflict<ValueT>>;

template <typename ValueT>
class ParserReport
{
public:
	using IssueType = Issue<ValueT>;
	using RuleType = Rule<ValueT>;
	using StateType = State<ValueT>;
	using SymbolType = Symbol<ValueT>;

	using ReduceReduceConflictType = ReduceReduceConflict<ValueT>;
	using ShiftReduceConflictType = ShiftReduceConflict<ValueT>;

	bool ok() const { return _issues.empty(); }
	operator bool() const { return ok(); }

	std::size_t number_of_issues() const { return _issues.size(); }
	auto begin() { return _issues.begin(); }
	auto end() { return _issues.end(); }
	auto begin() const { return _issues.begin(); }
	auto end() const { return _issues.end(); }

	void add_shift_reduce_conflict(const StateType* state, const SymbolType* symbol, const RuleType* rule)
	{
		_issues.push_back(ShiftReduceConflictType{state, symbol, rule});
	}

	void add_reduce_reduce_conflict(const StateType* state, const RuleType* rule1, const RuleType* rule2)
	{
		_issues.push_back(ReduceReduceConflictType{state, rule1, rule2});
	}

	std::string to_string(std::string_view arrow = "->", std::string_view eps = "<eps>") const
	{
		std::vector<std::string> issues_str(_issues.size());
		std::transform(_issues.begin(), _issues.end(), issues_str.begin(), [&](const auto& issue) {
			return visit_with(issue,
				[&](const ShiftReduceConflictType& sr) { return sr.to_string(arrow, eps); },
				[&](const ReduceReduceConflictType& rr) { return rr.to_string(arrow, eps); }
			);
		});
		return fmt::format("{}", fmt::join(issues_str.begin(), issues_str.end(), "\n"));
	}

private:
	std::vector<IssueType> _issues;
};

} // namespace pog
