#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <re2/re2.h>

#include <pog/symbol.h>

namespace pog {

template <typename ValueT>
class Token
{
public:
	using SymbolType = Symbol<ValueT>;
	using CallbackType = std::function<ValueT(std::string_view)>;

	template <typename StatesT>
	Token(std::uint32_t index, const std::string& pattern, StatesT&& active_in_states) : Token(index, pattern, std::forward<StatesT>(active_in_states), nullptr) {}

	template <typename StatesT>
	Token(std::uint32_t index, const std::string& pattern, StatesT&& active_in_states, const SymbolType* symbol)
		: _index(index), _pattern(pattern), _symbol(symbol), _regexp(std::make_unique<re2::RE2>(_pattern)), _action(),
			_enter_state(), _active_in_states(std::forward<StatesT>(active_in_states)) {}

	std::uint32_t get_index() const { return _index; }
	const std::string& get_pattern() const { return _pattern; }
	const SymbolType* get_symbol() const { return _symbol; }
	const re2::RE2* get_regexp() const { return _regexp.get(); }

	bool has_symbol() const { return _symbol != nullptr; }
	bool has_action() const { return static_cast<bool>(_action); }
	bool has_transition_to_state() const { return static_cast<bool>(_enter_state); }

	template <typename CallbackT>
	void set_action(CallbackT&& action)
	{
		_action = std::forward<CallbackT>(action);
	}

	template <typename... Args>
	ValueT perform_action(Args&&... args) const
	{
		return _action(std::forward<Args>(args)...);
	}

	void set_transition_to_state(const std::string& state)
	{
		_enter_state = state;
	}

	const std::string& get_transition_to_state() const
	{
		return _enter_state.value();
	}

	template <typename StrT>
	void add_active_in_state(StrT&& state)
	{
		_active_in_states.push_back(std::forward<StrT>(state));
	}

	const std::vector<std::string>& get_active_in_states() const
	{
		return _active_in_states;
	}

private:
	std::uint32_t _index;
	std::string _pattern;
	const SymbolType* _symbol;
	std::unique_ptr<re2::RE2> _regexp;
	CallbackType _action;
	std::optional<std::string> _enter_state;
	std::vector<std::string> _active_in_states;
};

} // namespace pog
