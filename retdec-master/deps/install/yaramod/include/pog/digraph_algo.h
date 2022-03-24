#pragma once

#include <deque>
#include <unordered_map>

namespace pog {

namespace detail {

template <typename NodeT, typename R, typename BaseF, typename F>
void digraph_traverse(const NodeT& x, std::deque<NodeT>& stack, std::unordered_map<NodeT, std::size_t>& depths, const R& rel, BaseF& base_f, F& f)
{
	stack.push_back(x); // push x
	std::size_t current_depth = stack.size(); // d <- depth of stack
	depths.insert_or_assign(x, current_depth); // N[x] <- d
	f.insert_or_assign(x, base_f[x]); // F(x) <- F'(x)

	auto rel_with = rel.find(x);
	if (rel_with)
	{
		for (const auto& y : *rel_with) // for each y such that xRy
		{
			auto include_itr = depths.find(y);
			if (include_itr == depths.end()) // if N[y] == 0
				digraph_traverse(y, stack, depths, rel, base_f, f); // recursive call Traverse(y)

			include_itr = depths.find(y); // possible iterator invalidation
			include_itr->second = std::min(depths[x], include_itr->second); // N[y] <- min(N[x], N[y])
			auto& fx = f[x];
			auto& fy = f[y];
			std::copy(fy.begin(), fy.end(), std::inserter(fx, fx.begin())); // F(x) <- F(x) union F(y)
		}
	}

	if (depths[x] == current_depth) // if N[x] == d
	{
		auto top_x = std::move(stack.back());
		stack.pop_back();
		depths[top_x] = std::numeric_limits<std::size_t>::max(); // N(top of stack) <- Infinity
		if (top_x != x)
			f[top_x] = f[x]; // F(top of stack) <- F(x)

		while (top_x != x) // while top of stack != x
		{
			top_x = std::move(stack.back());
			stack.pop_back();
			depths[top_x] = std::numeric_limits<std::size_t>::max(); // N(top of stack) <- Infinity
			if (top_x != x)
				f[top_x] = f[x]; // F(top of stack) <- F(x)
		}
	}
}

} // namespace detail

/**
 * Digraph algorithm for finding SCCs (Strongly Connected Components). It is used for
 * computation of function F(x) using base function F'(x) over directed graph. It first
 * computes F'(x) as F(x) for each node x and then perform unions of F(x) over edges
 * of directed graph. Finding SCC is a crucial part to not get into infinite loops and properly
 * propagate F(x) in looped relations.
 *
 * You can specify custom relation R which specifies edges of the directed graph, base function
 * F'(x) which needs to be already precomunted. The output is operation F(x) which will
 * be computed along the way.
 *
 * TODO: base_f should not be non-const but we require operator[] right now
 */
template <typename NodeT, typename R, typename BaseF, typename F>
void digraph_algo(const R& rel, BaseF& base_f, F& f)
{
	std::unordered_map<NodeT, std::size_t> depths;
	std::deque<NodeT> stack;
	for (const auto& x : rel)
	{
		detail::digraph_traverse(x.first, stack, depths, rel, base_f, f);
	}
}

} // namespace pog
