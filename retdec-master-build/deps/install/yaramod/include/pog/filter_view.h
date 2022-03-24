#pragma once

#include <fmt/format.h>

#include <functional>

template <typename It, typename Filter>
class FilterView
{
public:
	using ValueType = typename std::iterator_traits<It>::value_type;

	class iterator
	{
	public:
		using difference_type = typename std::iterator_traits<It>::difference_type;
		using value_type = typename std::iterator_traits<It>::value_type;
		using reference = typename std::iterator_traits<It>::reference;
		using pointer = typename std::iterator_traits<It>::pointer;
		using iterator_category = std::forward_iterator_tag;

		iterator(const FilterView* parent) : _parent(parent), _itr(_parent->_begin) { _find_next(); }
		iterator(const FilterView* parent, const It& itr) : _parent(parent), _itr(itr) {}
		iterator(const iterator&) = default;
		iterator(iterator&&) noexcept = default;

		iterator& operator=(const iterator&) = default;
		iterator& operator=(iterator&&) noexcept = default;

		reference operator*() const { return *_itr; }
		pointer operator->() const { return &*_itr; }

		iterator& operator++()
		{
			++_itr;
			_find_next();
			return *this;
		}

		iterator operator++(int)
		{
			auto tmp = *this;
			++_itr;
			_find_next();
			return tmp;
		}

		bool operator==(const iterator& rhs) const { return _itr == rhs._itr; }
		bool operator!=(const iterator& rhs) const { return !(*this == rhs); }

	private:
		void _find_next()
		{
			while (_itr != _parent->_end && !_parent->_filter(*_itr))
				++_itr;
		}

		const FilterView* _parent;
		It _itr;
	};

	template <typename I, typename F>
	FilterView(I&& begin, I&& end, F&& filter)
		: _begin(std::forward<I>(begin)), _end(std::forward<I>(end)), _filter(std::forward<F>(filter)) {}

	FilterView(const FilterView&) = default;
	FilterView(FilterView&&) noexcept = default;

	FilterView& operator=(const FilterView&) = default;
	FilterView& operator=(FilterView&&) noexcept = default;

	auto begin() const { return iterator{this}; }
	auto end() const { return iterator{this, _end}; }

private:
	It _begin, _end;
	std::function<bool(const ValueType&)> _filter;
};

template <typename It, typename Filter>
FilterView(It&&, It&&, Filter&&) -> FilterView<It, Filter>;
