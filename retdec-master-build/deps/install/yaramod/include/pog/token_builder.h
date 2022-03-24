#pragma once

#include <pog/grammar.h>
#include <pog/token.h>
#include <pog/tokenizer.h>

namespace pog {

template <typename ValueT>
class TokenBuilder
{
public:
	using GrammarType = Grammar<ValueT>;
	using SymbolType = Symbol<ValueT>;
	using TokenType = Token<ValueT>;
	using TokenizerType = Tokenizer<ValueT>;

	TokenBuilder(GrammarType* grammar, TokenizerType* tokenizer) : _grammar(grammar), _tokenizer(tokenizer), _pattern("$"),
		_symbol_name(), _precedence(), _action(), _fullword(false), _end_token(true), _in_states{std::string{TokenizerType::DefaultState}}, _enter_state() {}

	TokenBuilder(GrammarType* grammar, TokenizerType* tokenizer, const std::string& pattern) : _grammar(grammar), _tokenizer(tokenizer), _pattern(pattern),
		_symbol_name(), _precedence(), _action(), _fullword(false), _end_token(false), _in_states{std::string{TokenizerType::DefaultState}}, _enter_state() {}

	void done()
	{
		TokenType* token;
		if (!_end_token)
		{
			auto* symbol = !_symbol_name.empty() ? _grammar->add_symbol(SymbolKind::Terminal, _symbol_name) : nullptr;
			token = _tokenizer->add_token(_fullword ? fmt::format("{}(\\b|$)", _pattern) : _pattern, symbol, std::move(_in_states));
			if (symbol && _precedence)
			{
				const auto& prec = _precedence.value();
				symbol->set_precedence(prec.level, prec.assoc);
			}

			if(symbol && _description.size() != 0)
			 	symbol->set_description(_description);

			if (_enter_state)
				token->set_transition_to_state(_enter_state.value());
		}
		else
		{
			token = _tokenizer->get_end_token();
			for (auto&& state : _in_states)
				token->add_active_in_state(std::move(state));
		}

		if (_action)
			token->set_action(std::move(_action));
	}

	TokenBuilder& symbol(const std::string& symbol_name)
	{
		_symbol_name = symbol_name;
		return *this;
	}

	TokenBuilder& precedence(std::uint32_t level, Associativity assoc)
	{
		_precedence = Precedence{level, assoc};
		return *this;
	}

	TokenBuilder& description(const std::string& text)
	{
		_description = text;
		return *this;
	}

	template <typename CallbackT>
	TokenBuilder& action(CallbackT&& action)
	{
		_action = std::forward<CallbackT>(action);
		return *this;
	}

	TokenBuilder& fullword()
	{
		_fullword = true;
		return *this;
	}

	template <typename... Args>
	TokenBuilder& states(Args&&... args)
	{
		_in_states = {std::forward<Args>(args)...};
		return *this;
	}

	TokenBuilder& enter_state(const std::string& state)
	{
		_enter_state = state;
		return *this;
	}

private:
	GrammarType* _grammar;
	TokenizerType* _tokenizer;
	std::string _description;
	std::string _pattern;
	std::string _symbol_name;
	std::optional<Precedence> _precedence;
	typename TokenType::CallbackType _action;
	bool _fullword;
	bool _end_token;
	std::vector<std::string> _in_states;
	std::optional<std::string> _enter_state;
};

} // namespace pog
