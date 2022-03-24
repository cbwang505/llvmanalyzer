/**
 * @file src/types/rule.h
 * @brief Declaration of class Rule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "yaramod/types/expression.h"
#include "yaramod/types/meta.h"
#include "yaramod/types/string.h"
#include "yaramod/types/symbol.h"
#include "yaramod/utils/trie.h"

namespace yaramod {

class YaraFileBuilder;

/**
 * Class representing YARA rule.
 */
class Rule
{
public:
	struct Location
	{
		std::string filePath;
		std::uint64_t lineNumber;
	};

	using StringsTrie = Trie<std::shared_ptr<String>>;

	/**
	 * Rule modifier for specifying if rule is
	 * either classic, global or private.
	 *
	 * @code
	 * (global|private)? rule RULE_NAME { ... }
	 * @endcode
	 */
	enum class Modifier
	{
		None,
		Global,
		Private,
		PrivateGlobal,
	};

	/// @name Constructors
	/// @{
	Rule();
	explicit Rule(const std::shared_ptr<TokenStream>& tokenStream, TokenIt name, std::optional<TokenIt> mod_private, std::optional<TokenIt> mod_global,
		std::vector<Meta>&& metas, std::shared_ptr<StringsTrie>&& strings, Expression::Ptr&& condition, const std::vector<TokenIt>& tags);

	Rule(Rule&& rule) = default;
	Rule(const Rule& rule) = default;
	Rule& operator=(Rule&& rule) = default;
	/// @}

	/// @name String representation
	/// @{
	std::string getText() const;
	/// @}

	/// @name Getter methods
	/// @{
	std::string getName() const;
	Rule::Modifier getModifier() const;
	std::vector<Meta>& getMetas();
	const std::vector<Meta>& getMetas() const;
	std::vector<const String*> getStrings() const;
	const std::shared_ptr<StringsTrie>& getStringsTrie() const;
	const Expression::Ptr& getCondition() const;
	std::vector<std::string> getTags() const;
	const std::shared_ptr<Symbol>& getSymbol() const;
	Meta* getMetaWithName(const std::string& key);
	const Meta* getMetaWithName(const std::string& key) const;
	const Location& getLocation() const;
	TokenStream* getTokenStream() const { return _tokenStream.get(); }
	TokenIt getFirstTokenIt() const;
	TokenIt getLastTokenIt() const;
	/// @}

	/// @name Setter methods
	/// @{
	void setName(const std::string& name);
	void setMetas(const std::vector<Meta>& metas);
	void setTags(const std::vector<std::string>& tags);
	void setCondition(const Expression::Ptr& condition);
	void setLocation(const std::string& filePath, std::uint64_t lineNumber);
	void setModifier(const Modifier& modifier);
	/// @}

	/// @name Detection methods
	/// {
	bool isGlobal() const;
	bool isPrivate() const;
	/// }

	/// @name Manipulation methods
	/// @{
	void addMeta(const std::string& name, const Literal& value);
	void removeMetas(const std::string& name);
	void removeString(const std::string& id);
	void addTag(const std::string& tag);
	void removeTags(const std::string& tag);
	/// @}

private:
	void removeTags(TokenType type);

	std::shared_ptr<TokenStream> _tokenStream; ///< tokenStream containing all the data in this Rule
	TokenIt _name; ///< Name
	std::optional<TokenIt> _mod_private; ///< Private modifier
	std::optional<TokenIt> _mod_global; ///< Global modifier
	std::vector<Meta> _metas; ///< Meta information
	std::shared_ptr<StringsTrie> _strings; ///< Strings
	Expression::Ptr _condition; ///< Condition expression
	std::vector<TokenIt> _tags; ///< Tags
	Location _location; ///< Which file was this rule included from
};

}
