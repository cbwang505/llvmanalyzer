#pragma once

#include <numeric>
#include <unordered_set>

#include <pog/filter_view.h>
#include <pog/item.h>
#include <pog/utils.h>

namespace pog {

template <typename ValueT>
class State
{
public:
	using ItemType = Item<ValueT>;
	using SymbolType = Symbol<ValueT>;

	State() : _index(std::numeric_limits<decltype(_index)>::max()) {}
	State(std::uint32_t index) : _index(index) {}

	std::uint32_t get_index() const { return _index; }
	void set_index(std::uint32_t index) { _index = index; }

	std::size_t size() const { return _items.size(); }
	auto begin() const { return _items.begin(); }
	auto end() const { return _items.end(); }

	template <typename T>
	std::pair<ItemType*, bool> add_item(T&& item)
	{
		auto itr = std::lower_bound(_items.begin(), _items.end(), item, [](const auto& left, const auto& needle) {
			return *left.get() < needle;
		});

		if (itr == _items.end() || *itr->get() != item)
		{
			auto new_itr = _items.insert(itr, std::make_unique<ItemType>(std::forward<T>(item)));
			return {new_itr->get(), true};
		}
		else
			return {itr->get(), false};
	}

	void add_transition(const SymbolType* symbol, const State* state)
	{
		_transitions.emplace(symbol, state);
	}

	void add_back_transition(const SymbolType* symbol, const State* state)
	{
		auto itr = _back_transitions.find(symbol);
		if (itr == _back_transitions.end())
		{
			_back_transitions.emplace(symbol, std::vector<const State*>{state});
			return;
		}

		auto state_itr = std::lower_bound(itr->second.begin(), itr->second.end(), state->get_index(), [](const auto& left, const auto& needle) {
			return left->get_index() < needle;
		});

		if (state_itr == itr->second.end() || (*state_itr)->get_index() != state->get_index())
			itr->second.insert(state_itr, state);
	}

	bool is_accepting() const
	{
		return std::count_if(_items.begin(), _items.end(), [](const auto& item) {
				return item->is_accepting();
			}) == 1;
	}

	std::string to_string(std::string_view arrow = "->", std::string_view eps = "<eps>", std::string_view sep = "<*>", const std::string& newline = "\n") const
	{
		std::vector<std::string> item_strings(_items.size());
		std::transform(_items.begin(), _items.end(), item_strings.begin(), [&](const auto& item) {
			return item->to_string(arrow, eps, sep);
		});
		return fmt::format("{}", fmt::join(item_strings.begin(), item_strings.end(), newline));
	}

	std::vector<const ItemType*> get_production_items() const
	{
		std::vector<const ItemType*> result;
		transform_if(_items.begin(), _items.end(), std::back_inserter(result),
			[](const auto& item) {
				return item->is_final();
			},
			[](const auto& item) {
				return item.get();
			}
		);
		return result;
	}

	auto get_kernel() const
	{
		return FilterView{_items.begin(), _items.end(), [](const auto& item) {
			return item->is_kernel();
		}};
	}

	bool contains(const ItemType& item) const
	{
		auto itr = std::lower_bound(_items.begin(), _items.end(), item, [](const auto& left, const auto& needle) {
			return *left.get() < needle;
		});
		return itr != _items.end() && *itr->get() == item;
	}

	bool operator==(const State& rhs) const
	{
		auto lhs_kernel = get_kernel();
		auto rhs_kernel = rhs.get_kernel();
		return std::equal(lhs_kernel.begin(), lhs_kernel.end(), rhs_kernel.begin(), rhs_kernel.end(), [](const auto& left, const auto& right) {
			return *left.get() == *right.get();
		});
	}

	bool operator !=(const State& rhs) const
	{
		return !(*this == rhs);
	}

	const std::map<const SymbolType*, const State*, SymbolLess<ValueT>>& get_transitions() const { return _transitions; }
	const std::map<const SymbolType*, std::vector<const State*>, SymbolLess<ValueT>>& get_back_transitions() const { return _back_transitions; }

private:
	std::uint32_t _index;
	std::vector<std::unique_ptr<ItemType>> _items;
	std::map<const SymbolType*, const State*, SymbolLess<ValueT>> _transitions;
	std::map<const SymbolType*, std::vector<const State*>, SymbolLess<ValueT>> _back_transitions;
};

template <typename ValueT>
struct StateKernelHash
{
	std::size_t operator()(const State<ValueT>* state) const
	{
		std::size_t kernel_hash = 0;
		for (const auto& item : state->get_kernel())
			hash_combine(kernel_hash, item->get_rule()->get_index(), item->get_read_pos());
		return kernel_hash;
	}
};

template <typename ValueT>
struct StateKernelEquals
{
	bool operator()(const State<ValueT>* state1, const State<ValueT>* state2) const
	{
		return *state1 == *state2;
	}
};

} // namespace pog
