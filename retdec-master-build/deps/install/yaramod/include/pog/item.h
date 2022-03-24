#pragma once

#include <functional>
#include <unordered_set>

#include <pog/rule.h>
#include <pog/utils.h>

namespace pog {

template <typename ValueT>
class Item
{
public:
	using RuleType = Rule<ValueT>;
	using SymbolType = Symbol<ValueT>;

	Item(const RuleType* rule, std::size_t read_pos = 0)
		: _rule(rule), _read_pos(read_pos) {}
	Item(const Item&) = default;
	Item(Item&&) noexcept = default;

	const RuleType* get_rule() const { return _rule; }
	std::size_t get_read_pos() const { return _read_pos; }

	const SymbolType* get_previous_symbol() const
	{
		return _read_pos == 0 ? nullptr : _rule->get_rhs()[_read_pos - 1];
	}

	const SymbolType* get_read_symbol() const
	{
		return is_final() ? nullptr : _rule->get_rhs()[_read_pos];
	}

	std::vector<const SymbolType*> get_left_side_without_read_symbol()
	{
		if (_read_pos == 0)
			return {};

		// TODO: return just iterator range
		std::vector<const SymbolType*> result(_read_pos);
		std::copy(_rule->get_rhs().begin(), _rule->get_rhs().begin() + _read_pos, result.begin());
		return result;
	}

	std::vector<const SymbolType*> get_right_side_without_read_symbol()
	{
		if (is_final())
		{
			assert(false && "Shouldn't call get_right_side_without_read_symbol() on final item");
			return {};
		}

		auto rest_size = _rule->get_rhs().size() - _read_pos - 1;
		if (rest_size == 0)
			return {};

		// TODO: possibly just return iterator range?
		std::vector<const SymbolType*> result(rest_size);
		std::copy(_rule->get_rhs().begin() + _read_pos + 1, _rule->get_rhs().end(), result.begin());
		return result;
	}

	void step()
	{
		if (!is_final())
			_read_pos++;
	}

	void step_back()
	{
		if (_read_pos > 0)
			_read_pos--;
	}

	bool is_kernel() const
	{
		return _read_pos > 0 || _rule->is_start_rule();
	}

	bool is_final() const
	{
		return _read_pos == _rule->get_rhs().size();
	}

	bool is_accepting() const
	{
		return !is_final() && get_read_symbol()->is_end();
	}

	std::string to_string(std::string_view arrow = "->", std::string_view eps = "<eps>", std::string_view sep = "<*>") const
	{
		const auto& rhs = _rule->get_rhs();
		std::vector<std::string> left_of_read_pos(_read_pos);
		std::vector<std::string> right_of_read_pos(rhs.size() - _read_pos);
		std::transform(rhs.begin(), rhs.begin() + _read_pos, left_of_read_pos.begin(), [](const auto* sym) {
			return sym->get_name();
		});
		std::transform(rhs.begin() + _read_pos, rhs.end(), right_of_read_pos.begin(), [](const auto* sym) {
			return sym->get_name();
		});

		std::vector<std::string> parts;
		if (!left_of_read_pos.empty())
			parts.push_back(fmt::format("{}", fmt::join(left_of_read_pos.begin(), left_of_read_pos.end(), " ")));
		parts.push_back(std::string{sep});
		if (!right_of_read_pos.empty())
			parts.push_back(fmt::format("{}", fmt::join(right_of_read_pos.begin(), right_of_read_pos.end(), " ")));

		if (parts.size() == 1)
			parts.push_back(std::string{eps});

		return fmt::format("{} {} {}", _rule->get_lhs()->get_name(), arrow, fmt::join(parts.begin(), parts.end(), " "));
	}

	bool operator==(const Item& rhs) const
	{
		return get_rule()->get_index() == rhs.get_rule()->get_index() && get_read_pos() == rhs.get_read_pos();
	}

	bool operator!=(const Item& rhs) const
	{
		return !(*this == rhs);
	}

	bool operator<(const Item& rhs) const
	{
		return std::tuple{is_kernel() ? 0 : 1, _rule->get_index(), _read_pos} < std::tuple{rhs.is_kernel() ? 0 : 1, rhs._rule->get_index(), rhs._read_pos};
	}

private:
	const RuleType* _rule;
	std::size_t _read_pos;
};

} // namespace pog
