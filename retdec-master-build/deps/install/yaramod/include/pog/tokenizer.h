#pragma once

#include <memory>
#include <vector>

#include <fmt/format.h>
#include <re2/set.h>

#ifdef POG_DEBUG
#define POG_DEBUG_TOKENIZER 1
#endif

#ifdef POG_DEBUG_TOKENIZER
#define debug_tokenizer(...) fmt::print(stderr, "[tokenizer] {}\n", fmt::format(__VA_ARGS__))
#else
#define debug_tokenizer(...)
#endif

#include <pog/grammar.h>
#include <pog/token.h>

namespace pog {

template <typename ValueT>
struct TokenMatch
{
	TokenMatch(const Symbol<ValueT>* sym) : symbol(sym), value(), match_length(0) {}
	template <typename T>
	TokenMatch(const Symbol<ValueT>* sym, T&& v, std::size_t len) : symbol(sym), value(std::forward<T>(v)), match_length(len) {}
	TokenMatch(const TokenMatch&) = default;
	TokenMatch(TokenMatch&&) noexcept = default;

	TokenMatch& operator=(const TokenMatch&) = default;
	TokenMatch& operator=(TokenMatch&&) noexcept = default;

	const Symbol<ValueT>* symbol;
	ValueT value;
	std::size_t match_length;
};

struct InputStream
{
	std::unique_ptr<std::string> content;
	re2::StringPiece stream;
	bool at_end;
};

template <typename ValueT>
struct StateInfo
{
	std::string name;
	std::unique_ptr<re2::RE2::Set> re_set;
	std::vector<Token<ValueT>*> tokens;
};

template <typename ValueT>
class Tokenizer
{
public:
	using CallbackType = std::function<void(std::string_view)>;

	static constexpr std::string_view DefaultState = "@default";

	using GrammarType = Grammar<ValueT>;
	using StateInfoType = StateInfo<ValueT>;
	using SymbolType = Symbol<ValueT>;
	using TokenType = Token<ValueT>;
	using TokenMatchType = TokenMatch<ValueT>;

	Tokenizer(const GrammarType* grammar) : _grammar(grammar), _tokens(), _state_info(), _input_stack(), _current_state(nullptr), _global_action()
	{
		_current_state = get_or_make_state_info(std::string{DefaultState});
		add_token("$", nullptr, std::vector<std::string>{std::string{DefaultState}});
	}

	void prepare()
	{
		std::string error;

		for (const auto& token : _tokens)
		{
			for (const auto& state : token->get_active_in_states())
			{
				error.clear();
				auto* state_info = get_or_make_state_info(state);
				state_info->re_set->Add(token->get_pattern(), &error);
				state_info->tokens.push_back(token.get());
				assert(error.empty() && "Error when compiling token regexp");
			}
		}

		for (auto&& [name, info] : _state_info)
			info.re_set->Compile();
	}

	const std::vector<std::unique_ptr<TokenType>>& get_tokens() const
	{
		return _tokens;
	}

	TokenType* get_end_token() const
	{
		return _tokens[0].get();
	}

	TokenType* add_token(const std::string& pattern, const SymbolType* symbol, const std::vector<std::string>& states)
	{
		_tokens.push_back(std::make_unique<TokenType>(static_cast<std::uint32_t>(_tokens.size()), pattern, states, symbol));
		return _tokens.back().get();
	}

	void push_input_stream(std::istream& stream)
	{
		std::string input;
		std::vector<char> block(4096);
		while (stream.good())
		{
			stream.read(block.data(), block.size());
			input.append(std::string_view(block.data(), stream.gcount()));
		}

		_input_stack.emplace_back(InputStream{std::make_unique<std::string>(std::move(input)), re2::StringPiece{}, false});
		_input_stack.back().stream = re2::StringPiece{_input_stack.back().content->c_str()};
	}

	void pop_input_stream()
	{
		_input_stack.pop_back();
	}

	void clear_input_streams()
	{
		_input_stack.clear();
	}

	void global_action(CallbackType&& global_action)
	{
		_global_action = std::move(global_action);
	}

	std::optional<TokenMatchType> next_token()
	{
		bool repeat = true;
		while (repeat)
		{
			// We've emptied the stack so that means return end symbol to parser
			if (_input_stack.empty())
			{
				debug_tokenizer("Input stack empty - returing end of input");
				return _grammar->get_end_of_input_symbol();
			}

			auto& current_input = _input_stack.back();
			if (!current_input.at_end)
			{
				// Matched patterns doesn't have to be sorted (used to be in older re2 versions) but we shouldn't count on that
				std::vector<int> matched_patterns;
				_current_state->re_set->Match(current_input.stream, &matched_patterns);

				// Haven't matched anything, tokenization failure, we will get into endless loop
				if (matched_patterns.empty())
				{
					debug_tokenizer("Nothing matched on the current input");
					return std::nullopt;
				}

				re2::StringPiece submatch;
				const TokenType* best_match = nullptr;
				int longest_match = -1;
				for (auto pattern_index : matched_patterns)
				{
					_current_state->tokens[pattern_index]->get_regexp()->Match(current_input.stream, 0, current_input.stream.size(), re2::RE2::Anchor::ANCHOR_START, &submatch, 1);
					if (longest_match < static_cast<int>(submatch.size()))
					{
						best_match = _current_state->tokens[pattern_index];
						longest_match = static_cast<int>(submatch.size());
					}
					// In case of equal matches, index of tokens chooses which one is it (lower index has higher priority)
					else if (longest_match == static_cast<int>(submatch.size()))
					{
						if (!best_match || best_match->get_index() > pattern_index)
							best_match = _current_state->tokens[pattern_index];
					}
				}

				if (current_input.stream.size() == 0)
				{
					debug_tokenizer("Reached end of input");
					current_input.at_end = true;
				}

				if (best_match->has_transition_to_state())
				{
					enter_state(best_match->get_transition_to_state());
					debug_tokenizer("Entered state \'{}\'", best_match->get_transition_to_state());
				}

				std::string_view token_str{current_input.stream.data(), static_cast<std::size_t>(longest_match)};
				current_input.stream.remove_prefix(longest_match);
				debug_tokenizer("Matched \'{}\' with token \'{}\' (index {})", token_str, best_match->get_pattern(), best_match->get_index());

				if (_global_action)
					_global_action(token_str);

				ValueT value{};
				if (best_match->has_action())
					value = best_match->perform_action(token_str);

				if (!best_match->has_symbol())
					continue;

				return TokenMatchType{best_match->get_symbol(), std::move(value), static_cast<std::size_t>(longest_match)};
			}
			else
				debug_tokenizer("At the end of input");

			// There is still something on stack but we've reached the end and noone popped it so return end symbol to parser
			return _grammar->get_end_of_input_symbol();
		}

		return std::nullopt;
	}

	void enter_state(const std::string& state)
	{
		_current_state = get_state_info(state);
		assert(_current_state && "Transition to unknown state in tokenizer");
	}

private:
	StateInfoType* get_or_make_state_info(const std::string& name)
	{
		auto itr = _state_info.find(name);
		if (itr == _state_info.end())
			std::tie(itr, std::ignore) = _state_info.emplace(name, StateInfoType{
				name,
				std::make_unique<re2::RE2::Set>(re2::RE2::DefaultOptions, re2::RE2::Anchor::ANCHOR_START),
				std::vector<TokenType*>{}
			});
		return &itr->second;
	}

	StateInfoType* get_state_info(const std::string& name)
	{
		auto itr = _state_info.find(name);
		if (itr == _state_info.end())
			return nullptr;
		return &itr->second;
	}

	const GrammarType* _grammar;
	std::vector<std::unique_ptr<TokenType>> _tokens;

	std::unordered_map<std::string, StateInfoType> _state_info;
	std::vector<InputStream> _input_stack;
	StateInfoType* _current_state;
	CallbackType _global_action;
};

} // namespace pog
