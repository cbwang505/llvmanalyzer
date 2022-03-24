/**
 * @file src/types/yara_file.h
 * @brief Declaration of class YaraFile.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <vector>

#include "yaramod/types/modules/modules_pool.h"
#include "yaramod/types/rule.h"

namespace yaramod {

/**
 * Class representing YARA file with all the imports and
 * rules it contains.
 */
class YaraFile
{
public:
	/// @name Constructors
	/// @{
	YaraFile(ImportFeatures features = ImportFeatures::AllCurrent);
	YaraFile(const std::shared_ptr<TokenStream>& tokenStream, ImportFeatures features = ImportFeatures::AllCurrent);
	YaraFile(YaraFile&&) noexcept;

	YaraFile& operator=(YaraFile&&) noexcept;
	/// @}

	/// @name String representation
	/// @{
	std::string getText() const;
	std::string getTextFormatted(bool withIncludes = false) const;
	/// @}

	/// @name Addition methods
	/// @{
	bool addImport(TokenIt import, ModulesPool& modules);
	void addRule(Rule&& rule);
	void addRule(std::unique_ptr<Rule>&& rule);
	void addRule(const std::shared_ptr<Rule>& rule);
	void addRules(const std::vector<std::shared_ptr<Rule>>& rules);
	bool addImports(const std::vector<TokenIt>& imports, ModulesPool& modules);
	void insertRule(std::size_t position, std::unique_ptr<Rule>&& rule);
	void insertRule(std::size_t position, const std::shared_ptr<Rule>& rule);
	/// @}

	/// @name Getter methods
	/// @{
	const std::vector<std::shared_ptr<Module>>& getImports() const;
	const std::vector<std::shared_ptr<Rule>>& getRules() const;
	TokenStream* getTokenStream() const;
	/// @}

	/// @name Removing methods
	/// @{
	template <typename Fn>
	void removeImports(Fn&& fn)
	{
		auto itr = std::stable_partition(_imports.begin(), _imports.end(), [&](const auto& i) { return !fn(i); });
		for (auto rem_itr = itr; rem_itr != _imports.end(); ++rem_itr)
			_importTable.erase(_importTable.find((*rem_itr)->getName()));
		_imports.erase(itr, _imports.end());
	}

	template <typename Fn>
	void removeRules(Fn&& fn)
	{
		auto itr = std::stable_partition(_rules.begin(), _rules.end(), [&](const auto& i) { return !fn(i); });
		for (auto rem_itr = itr; rem_itr != _rules.end(); ++rem_itr)
		{
			_ruleTable.erase(_ruleTable.find((*rem_itr)->getName()));
			auto behind = _tokenStream->erase((*rem_itr)->getFirstTokenIt(), std::next((*rem_itr)->getLastTokenIt()));
			while (behind != _tokenStream->end() && behind->getType() == NEW_LINE)
				behind = _tokenStream->erase(behind);
		}
		_rules.erase(itr, _rules.end());
	}
	/// @}

	/// @name Symbol methods
	/// @{
	std::shared_ptr<Symbol> findSymbol(const std::string& name) const;
	/// @}

	/// @name Detection methods
	/// @{
	bool hasImports() const;
	bool hasRules() const;
	bool hasRule(const std::string& name) const;
	/// @}

private:
	void initializeVTSymbols();

	std::shared_ptr<TokenStream> _tokenStream; ///< tokenStream containing all the data in this Rule
	std::vector<std::shared_ptr<Module>> _imports; ///< Imported modules
	std::vector<std::shared_ptr<Rule>> _rules; ///< Rules

	std::unordered_map<std::string, Module*> _importTable;
	std::unordered_map<std::string, Rule*> _ruleTable;

	ImportFeatures _importFeatures; ///< Determines which symbols are needed
	std::vector<std::shared_ptr<Symbol>> _vtSymbols; ///< Virust Total symbols
};

}
