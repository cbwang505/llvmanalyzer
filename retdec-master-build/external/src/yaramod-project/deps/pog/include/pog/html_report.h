#pragma once

#include <fstream>

#include <pog/parser.h>

namespace pog {

template <typename ValueT>
class HtmlReport
{
public:
	using ParserType = Parser<ValueT>;

	using ShiftActionType = Shift<ValueT>;
	using ReduceActionType = Reduce<ValueT>;

	HtmlReport(const ParserType& parser) : _parser(parser) {}

	void save(const std::string& file_path)
	{
		static const std::string html_page_template = R"(<!doctype html>
<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
		<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
		<link href="https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css" rel="stylesheet" integrity="sha384-wvfXpqpZZVQGK6TAh5PVlGOfQNHSoD2xbE+QkPxCAFlNEevoEH3Sl0sibVcOQVnN" crossorigin="anonymous">
		<style>
			#parsing-table {{
				width: auto;
			}}
			#parsing-table td, #parsing-table th {{
				min-width: 2em;
				max-width: 6em;
				word-wrap: break-word;
				text-align: center;
			}}
			.state-table {{
				width: auto;
			}}
			.state-table td, .state-table th {{
				word-wrap: break-word;
				text-align: center;
			}}
		</style>
	</head>
	<body>
		<nav class="navbar navbar-dark bg-dark">
			<span class="navbar-brand">pog report</span>
		</nav>
		<main class="container">
			{issues}
			{parsing_table}
			{states}
			{automaton}
			<div class="pt-3 row justify-content-center">
				<small>Generated: {generated_at}</small>
			</div>
		</div>
		<script src="https://code.jquery.com/jquery-3.4.1.min.js" integrity="sha256-CSXorXvZcTkaix6Yvo6HppcZGetbYMGWSFlBw8HfCJo=" crossorigin="anonymous"></script>
		<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js" integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q" crossorigin="anonymous"></script>
		<script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js" integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl" crossorigin="anonymous"></script>
		<script src="https://cdnjs.cloudflare.com/ajax/libs/clipboard.js/2.0.4/clipboard.min.js"></script>
		<script type="text/javascript">
$(document).ready(function() {{
	$('[data-toggle="tooltip"]').tooltip();
}});

var clipboard = new ClipboardJS('.btn');
		</script>
	</body>
</html>)";

		using namespace fmt;

		std::ofstream file(file_path, std::ios::out | std::ios::trunc);
		if (file.is_open())
			file << fmt::format(html_page_template,
				"issues"_a = build_issues(),
				"parsing_table"_a = build_parsing_table(),
				"states"_a = build_states(),
				"automaton"_a = generate_automaton_graph(),
				"generated_at"_a = current_time("%Y-%m-%d %H:%M:%S %Z")
			);
		file.close();
	}

private:
	std::string build_issues()
	{
		using namespace fmt;

		if (_parser._report)
			return std::string{};

		std::vector<std::string> issues(_parser._report.number_of_issues());
		std::transform(_parser._report.begin(), _parser._report.end(), issues.begin(), [](const auto& issue) {
			return fmt::format("<li><span>{}</span></li>", visit_with(issue,
				[&](const ShiftReduceConflict<ValueT>& sr) { return sr.to_string("→", "ε"); },
				[&](const ReduceReduceConflict<ValueT>& rr) { return rr.to_string("→", "ε"); }
			));
		});

		return fmt::format(
			R"(<div class="pt-3 row justify-content-center">
				<div class="col-12 alert alert-danger">
					<h3 class="font-weight-bold">Issues</h3>
					<ul>
						{issues}
					</ul>
				</div>
			</div>)",
			"issues"_a = fmt::join(issues.begin(), issues.end(), "")
		);
	}

	std::string build_parsing_table()
	{
		using namespace fmt;

		auto terminal_symbols = _parser._grammar.get_terminal_symbols();
		auto nonterminal_symbols = _parser._grammar.get_nonterminal_symbols();

		std::vector<std::string> symbol_headers(terminal_symbols.size() + nonterminal_symbols.size());
		std::transform(terminal_symbols.begin(), terminal_symbols.end(), symbol_headers.begin(), [](const auto& s) {
			return fmt::format("<th>{}</th>", s->get_name());
		});
		std::transform(nonterminal_symbols.begin(), nonterminal_symbols.end(), symbol_headers.begin() + terminal_symbols.size(), [](const auto& s) {
			return fmt::format("<th>{}</th>", s->get_name());
		});

		std::vector<std::string> rows(_parser._automaton.get_states().size());
		for (const auto& state : _parser._automaton.get_states())
		{
			std::vector<std::string> row;
			row.push_back(fmt::format(
				"<tr><td data-toggle=\"tooltip\" data-placement=\"bottom\" title=\"{}\" data-html=\"true\">{}</td>",
				state->to_string("→", "ε", "•", "<br/>"),
				state->get_index()
			));
			for (const auto& sym : terminal_symbols)
			{
				auto action = _parser._parsing_table.get_action(state.get(), sym);
				if (!action)
				{
					row.push_back("<td></td>");
					continue;
				}

				row.push_back(visit_with(action.value(),
					[](const ShiftActionType& shift) {
						return fmt::format(
							"<td data-toggle=\"tooltip\" data-placement=\"bottom\" title=\"{state_str}\" data-html=\"true\"><a href=\"#state{state_id}\">s{state_id}</a></td>",
							"state_str"_a = shift.state->to_string("→", "ε", "•", "<br/>"),
							"state_id"_a = shift.state->get_index()
						);
					},
					[](const ReduceActionType& reduce) {
						return fmt::format(
							"<td data-toggle=\"tooltip\" data-placement=\"bottom\" title=\"{}\">r{}</td>",
							reduce.rule->to_string("→", "ε"),
							reduce.rule->get_index()
						);
					},
					[](const Accept&) -> std::string { return "<td><span class=\"fa fa-check-square\" style=\"color: green\"/></td>"; }
				));
			}

			for (const auto& sym : nonterminal_symbols)
			{
				auto go_to = _parser._parsing_table.get_transition(state.get(), sym);
				if (!go_to)
				{
					row.push_back("<td></td>");
					continue;
				}

				row.push_back(fmt::format(
					"<td data-toggle=\"tooltip\" data-placement=\"bottom\" title=\"{state_str}\" data-html=\"true\"><a href=\"#state{state_id}\">{state_id}</a></td>",
					"state_str"_a = go_to.value()->to_string("→", "ε", "•", "<br/>"),
					"state_id"_a = go_to.value()->get_index()
				));
			}

			row.push_back("</tr>");
			rows.push_back(fmt::format("{}", fmt::join(row.begin(), row.end(), "")));
		}

		return fmt::format(
			R"(<div class="pt-3 row justify-content-center">
				<h3 class="col text-center">Parsing Table</h3>
				<div class="w-100"></div>
				<div class="col">
					<table id="parsing-table" class="table table-bordered table-sm ml-auto mr-auto">
						<thead class="thead-dark">
							<tr>
								<th rowspan="2">State</th>
								<th colspan="{number_of_terminals}">Action</th>
								<th colspan="{number_of_nonterminals}">Goto</th>
							</tr>
							<tr>
								{symbols}
							</tr>
						</thead>
						<tbody>
							{rows}
						</tbody>
					</table>
				</div>
			</div>)",
			"number_of_terminals"_a = terminal_symbols.size(),
			"number_of_nonterminals"_a = nonterminal_symbols.size(),
			"symbols"_a = fmt::join(symbol_headers.begin(), symbol_headers.end(), ""),
			"rows"_a = fmt::join(rows.begin(), rows.end(), "")
		);
	}

	std::string build_states()
	{
		using namespace fmt;

		static const std::string single_state_template =
			R"(<div class="col-4">
				<table class="state-table table table-bordered table-sm ml-auto mr-auto" id="state{id}">
					<thead class="thead-dark">
						<tr><th>State {id}</th></tr>
					</thead>
					<tbody>
						{rows}
					<tbody>
				</table>
			</div>)";

		std::vector<std::string> states;
		for (const auto& state : _parser._automaton.get_states())
		{
			std::vector<std::string> cols(state->size());
			std::transform(state->begin(), state->end(), cols.begin(), [](const auto& item) {
				return fmt::format("<tr><td>{}</td></tr>", item->to_string("→", "ε", "•"));
			});
			states.push_back(fmt::format(
				single_state_template,
				"id"_a = state->get_index(),
				"rows"_a = fmt::join(cols.begin(), cols.end(), "")
			));
		}

		return fmt::format(
			R"(<div class="row justify-content-around">
				<h3 class="col text-center">States</h3>
				<div class="w-100"></div>
				{states}
			</div>)",
			"states"_a = fmt::join(states.begin(), states.end(), "")
		);
	}

	std::string generate_automaton_graph()
	{
		using namespace fmt;

		return fmt::format(R"(<div class="row justify-content-center">
				<h3 class="col text-center">Automaton (graphviz)</h3>
				<div class="w-100"></div>
				<textarea id="automaton" class="textarea form-control text-monospace" style="height: 20em; resize: none; font-size: 12px;" readonly>{automaton}</textarea>
				<div class="w-100"></div>
				<div class="col pt-2 text-center">
					<button class="btn btn-dark" data-clipboard-target="#automaton">
						Copy
					</button>
				</div>
			</div>)",
			"automaton"_a = _parser._automaton.generate_graph()
		);
	}

	const ParserType& _parser;
};

template <typename ValueT>
HtmlReport(const Parser<ValueT>&) -> HtmlReport<ValueT>;

} // namespace pog
