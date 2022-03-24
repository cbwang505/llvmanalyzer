#pragma once

#include <pog/grammar.h>
#include <pog/rule.h>

namespace pog {

template <typename ValueT>
class RuleBuilder
{
public:
	using GrammarType = Grammar<ValueT>;
	using RuleType = Rule<ValueT>;
	using SymbolType = Symbol<ValueT>;

	struct SymbolsAndAction
	{
		std::vector<std::string> symbols;
		typename RuleType::CallbackType action;
	};

	struct RightHandSide
	{
		std::vector<SymbolsAndAction> symbols_and_action;
		std::optional<Precedence> precedence;
	};

	RuleBuilder(GrammarType* grammar, const std::string& lhs) : _grammar(grammar), _lhs(lhs), _rhss() {}

	void done()
	{
		if (_rhss.empty())
			return;

		const auto* lhs_symbol = _grammar->add_symbol(SymbolKind::Nonterminal, _lhs);

		std::size_t rhs_counter = 0;
		for (auto&& rhs : _rhss)
		{
			assert(!rhs.symbols_and_action.empty() && "No symbols and action associated to right-hand side of the rule. This shouldn't happen");

			std::vector<const SymbolType*> rhs_symbols;
			for (std::size_t i = 0; i < rhs.symbols_and_action.size(); ++i)
			{
				auto&& symbols_and_action = rhs.symbols_and_action[i];

				std::transform(symbols_and_action.symbols.begin(), symbols_and_action.symbols.end(), std::back_inserter(rhs_symbols), [this](const auto& sym_name) {
					return _grammar->add_symbol(SymbolKind::Nonterminal, sym_name);
				});

				// There are multple actions (mid-rule actions) so we need to create new symbol and epsilon rule
				// for each midrule action. Midrule symbols will be inserted into the original rule.
				//
				// If you have rule A -> B C D and you want to perform action after B, then we'll create rules
				// A -> B X C D
				// X -> <eps>
				// where X -> <eps> will have assigned the midrule action.
				if (i < rhs.symbols_and_action.size() - 1)
				{
					// Create unique nonterminal for midrule action
					auto midsymbol = _grammar->add_symbol(
						SymbolKind::Nonterminal,
						fmt::format("_{}#{}.{}", _lhs, rhs_counter, i)
					);

					// Create rule to which midrule action can be assigned and set midrule size.
					// Midrule size is number of symbols preceding the midrule symbol. It represents how many
					// items from stack we need to borrow for action arguments.
					auto rule = _grammar->add_rule(midsymbol, std::vector<const SymbolType*>{}, std::move(symbols_and_action.action));
					rule->set_midrule(rhs_symbols.size());
					rhs_symbols.push_back(midsymbol);
				}
				// This is the last action so do not mark it as midrule
				else
				{
					auto rule = _grammar->add_rule(lhs_symbol, rhs_symbols, std::move(symbols_and_action.action));
					if (rule && rhs.precedence)
					{
						const auto& prec = rhs.precedence.value();
						rule->set_precedence(prec.level, prec.assoc);
					}
				}
			}

			rhs_counter++;
		}
	}

	template <typename... Args>
	RuleBuilder& production(Args&&... args)
	{
		_rhss.push_back(RightHandSide{
			std::vector<SymbolsAndAction>{
				SymbolsAndAction{
					std::vector<std::string>{},
					{}
				}
			},
			std::nullopt
		});
		_production(_rhss.back().symbols_and_action, std::forward<Args>(args)...);
		return *this;
	}

	RuleBuilder& precedence(std::uint32_t level, Associativity assoc)
	{
		_rhss.back().precedence = Precedence{level, assoc};
		return *this;
	}

private:
	void _production(std::vector<SymbolsAndAction>&) {}

	template <typename... Args>
	void _production(std::vector<SymbolsAndAction>& sa, const std::string& symbol, Args&&... args)
	{
		sa.back().symbols.push_back(symbol);
		_production(sa, std::forward<Args>(args)...);
	}

	template <typename... Args>
	void _production(std::vector<SymbolsAndAction>& sa, typename RuleType::CallbackType&& action, Args&&... args)
	{
		sa.back().action = std::move(action);
		// We have ran into action so create new record in symbols and actions vector
		// but only if it isn't the very last thing in the production
		if constexpr (sizeof...(args) > 0)
			sa.push_back(SymbolsAndAction{
				std::vector<std::string>{},
				{}
			});
		_production(sa, std::forward<Args>(args)...);
	}

	GrammarType* _grammar;
	std::string _lhs;
	std::vector<RightHandSide> _rhss;
};

} // namespace pog
