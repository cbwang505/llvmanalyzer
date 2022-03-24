#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <pog/precedence.h>

namespace pog {

enum class SymbolKind
{
	End,
	Nonterminal,
	Terminal
};

template <typename ValueT>
class Symbol
{
public:
	Symbol(std::uint32_t index, SymbolKind kind, const std::string& name) : _index(index), _kind(kind), _name(name) {}

	std::uint32_t get_index() const { return _index; }
	const Precedence& get_precedence() const { return _precedence.value(); }
	const std::string& get_name() const { return _name; }
	const std::string& get_description() const { return _description.has_value() ? *_description : _name; }

	bool has_precedence() const { return static_cast<bool>(_precedence); }
	bool is_end() const { return _kind == SymbolKind::End; }
	bool is_nonterminal() const { return _kind == SymbolKind::Nonterminal; }
	bool is_terminal() const { return _kind == SymbolKind::Terminal; }

	void set_precedence(std::uint32_t level, Associativity assoc) { _precedence = Precedence{level, assoc}; }
	void set_description(const std::string& description) { _description = description; }

private:
	std::uint32_t _index;
	SymbolKind _kind;
	std::string _name;
	std::optional<std::string> _description;
	std::optional<Precedence> _precedence;
};


template <typename ValueT>
struct SymbolLess
{
	bool operator()(const Symbol<ValueT>* lhs, const Symbol<ValueT>* rhs) const
	{
		return lhs->get_index() < rhs->get_index();
	}
};

} // namespace pog
