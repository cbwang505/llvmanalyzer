/**
 * @file src/python/yaramod_python.h
 * @brief Main header for yaramod python bindings.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <variant>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <yaramod/yaramod.h>

/**
 * Always include this file before anything else in `src/python` folder.
 * Otherwise, you'll get `template specialization after instantiation` errors.
 */

namespace pybind11 { namespace detail {

/**
 * Helper for type caster of `std::variant` to inspect values of variant.
 */
template <>
struct visit_helper<std::variant>
{
	template <typename... Args>
	static auto call(Args&&... args) -> decltype(std::visit(args...))
	{
		return std::visit(args...);
	}
};

/**
 * This type caster allows us to use `std::vector<const yaramod::String*>` with `return_value_policy:reference`.
 * Originally, return value policy influenced just the vector itself but not its content. This overrides the return value policy
 * of the content.
 */
template <>
struct type_caster<std::vector<const yaramod::String*>> : list_caster<std::vector<const yaramod::String*>, const yaramod::String*>
{
	static handle cast(const std::vector<const yaramod::String*>& src, return_value_policy, handle parent)
	{
		return list_caster<std::vector<const yaramod::String*>, const yaramod::String*>::cast(src, return_value_policy::reference, parent);
	}

	static handle cast(const std::vector<const yaramod::String*>* src, return_value_policy pol, handle parent)
	{
		return cast(*src, pol, parent);
	}
};

}}
