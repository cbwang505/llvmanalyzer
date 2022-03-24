/**
 * @file src/types/yara_file.cpp
 * @brief Implementation of class YaraFile.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <iterator>

#include "yaramod/types/yara_file.h"
#include "yaramod/utils/utils.h"

namespace yaramod {

/**
 * Constructor.
 */
YaraFile::YaraFile(ImportFeatures features)
	: YaraFile(std::make_shared<TokenStream>(), features)
{
	if (_importFeatures & ImportFeatures::VirusTotalOnly)
		initializeVTSymbols();
}

YaraFile::YaraFile(const std::shared_ptr<TokenStream>& tokenStream, ImportFeatures features)
	: _tokenStream(std::move(tokenStream))
	, _imports()
	, _rules()
	, _importTable()
	, _ruleTable()
	, _importFeatures(features)
{
	if (_importFeatures & ImportFeatures::VirusTotalOnly)
		initializeVTSymbols();
}

YaraFile::YaraFile(YaraFile&& o) noexcept
	: _tokenStream(std::move(o._tokenStream))
	, _imports(std::move(o._imports))
	, _rules(std::move(o._rules))
	, _importTable(std::move(o._importTable))
	, _ruleTable(std::move(o._ruleTable))
	, _importFeatures(std::move(o._importFeatures))
	, _vtSymbols(std::move(o._vtSymbols))
{
}

YaraFile& YaraFile::operator=(YaraFile&& o) noexcept
{
	std::swap(_tokenStream, o._tokenStream);
	std::swap(_imports, o._imports);
	std::swap(_rules, o._rules);
	std::swap(_importTable, o._importTable);
	std::swap(_ruleTable, o._ruleTable);
	std::swap(_importFeatures, o._importFeatures);
	std::swap(_vtSymbols, o._vtSymbols);
	return *this;
}

/**
 * VirusTotal symbols
 */
void YaraFile::initializeVTSymbols()
{
	_vtSymbols = {
		// VirusTotal specific global variables
		std::make_shared<ValueSymbol>("new_file", Expression::Type::Bool),
		std::make_shared<ValueSymbol>("positives", Expression::Type::Int),
		std::make_shared<ValueSymbol>("signatures", Expression::Type::String),
		std::make_shared<ValueSymbol>("tags", Expression::Type::String),
		std::make_shared<ValueSymbol>("md5", Expression::Type::String),
		std::make_shared<ValueSymbol>("sha256", Expression::Type::String),
		std::make_shared<ValueSymbol>("imphash", Expression::Type::String),
		std::make_shared<ValueSymbol>("file_type", Expression::Type::String),
		std::make_shared<ValueSymbol>("file_name", Expression::Type::String),
		// VirusTotal specific global variables of antiviruses
		std::make_shared<ValueSymbol>("a_squared", Expression::Type::String),
		std::make_shared<ValueSymbol>("ad_aware", Expression::Type::String),
		std::make_shared<ValueSymbol>("aegislab", Expression::Type::String),
		std::make_shared<ValueSymbol>("agnitum", Expression::Type::String),
		std::make_shared<ValueSymbol>("ahnlab", Expression::Type::String),
		std::make_shared<ValueSymbol>("ahnlab_v3", Expression::Type::String),
		std::make_shared<ValueSymbol>("alibaba", Expression::Type::String),
		std::make_shared<ValueSymbol>("alyac", Expression::Type::String),
		std::make_shared<ValueSymbol>("antivir", Expression::Type::String),
		std::make_shared<ValueSymbol>("antivir7", Expression::Type::String),
		std::make_shared<ValueSymbol>("antiy_avl", Expression::Type::String),
		std::make_shared<ValueSymbol>("arcabit", Expression::Type::String),
		std::make_shared<ValueSymbol>("authentium", Expression::Type::String),
		std::make_shared<ValueSymbol>("avast", Expression::Type::String),
		std::make_shared<ValueSymbol>("avg", Expression::Type::String),
		std::make_shared<ValueSymbol>("avira", Expression::Type::String),
		std::make_shared<ValueSymbol>("avware", Expression::Type::String),
		std::make_shared<ValueSymbol>("baidu", Expression::Type::String),
		std::make_shared<ValueSymbol>("bitdefender", Expression::Type::String),
		std::make_shared<ValueSymbol>("bkav", Expression::Type::String),
		std::make_shared<ValueSymbol>("bytehero", Expression::Type::String),
		std::make_shared<ValueSymbol>("cat_quickheal", Expression::Type::String),
		std::make_shared<ValueSymbol>("clamav", Expression::Type::String),
		std::make_shared<ValueSymbol>("cmc", Expression::Type::String),
		std::make_shared<ValueSymbol>("commtouch", Expression::Type::String),
		std::make_shared<ValueSymbol>("comodo", Expression::Type::String),
		std::make_shared<ValueSymbol>("crowdstrike", Expression::Type::String),
		std::make_shared<ValueSymbol>("cyren", Expression::Type::String),
		std::make_shared<ValueSymbol>("drweb", Expression::Type::String),
		std::make_shared<ValueSymbol>("emsisoft", Expression::Type::String),
		std::make_shared<ValueSymbol>("esafe", Expression::Type::String),
		std::make_shared<ValueSymbol>("escan", Expression::Type::String),
		std::make_shared<ValueSymbol>("eset_nod32", Expression::Type::String),
		std::make_shared<ValueSymbol>("f_prot", Expression::Type::String),
		std::make_shared<ValueSymbol>("f_secure", Expression::Type::String),
		std::make_shared<ValueSymbol>("fortinet", Expression::Type::String),
		std::make_shared<ValueSymbol>("gdata", Expression::Type::String),
		std::make_shared<ValueSymbol>("ikarus", Expression::Type::String),
		std::make_shared<ValueSymbol>("invincea", Expression::Type::String),
		std::make_shared<ValueSymbol>("jiangmin", Expression::Type::String),
		std::make_shared<ValueSymbol>("k7antivirus", Expression::Type::String),
		std::make_shared<ValueSymbol>("k7gw", Expression::Type::String),
		std::make_shared<ValueSymbol>("kaspersky", Expression::Type::String),
		std::make_shared<ValueSymbol>("kingsoft", Expression::Type::String),
		std::make_shared<ValueSymbol>("malwarebytes", Expression::Type::String),
		std::make_shared<ValueSymbol>("mcafee", Expression::Type::String),
		std::make_shared<ValueSymbol>("mcafee_gw_edition", Expression::Type::String),
		std::make_shared<ValueSymbol>("microsoft", Expression::Type::String),
		std::make_shared<ValueSymbol>("microworld_escan", Expression::Type::String),
		std::make_shared<ValueSymbol>("nano_antivirus", Expression::Type::String),
		std::make_shared<ValueSymbol>("nod32", Expression::Type::String),
		std::make_shared<ValueSymbol>("norman", Expression::Type::String),
		std::make_shared<ValueSymbol>("nprotect", Expression::Type::String),
		std::make_shared<ValueSymbol>("panda", Expression::Type::String),
		std::make_shared<ValueSymbol>("pctools", Expression::Type::String),
		std::make_shared<ValueSymbol>("prevx", Expression::Type::String),
		std::make_shared<ValueSymbol>("prevx1", Expression::Type::String),
		std::make_shared<ValueSymbol>("qihoo_360", Expression::Type::String),
		std::make_shared<ValueSymbol>("rising", Expression::Type::String),
		std::make_shared<ValueSymbol>("sophos", Expression::Type::String),
		std::make_shared<ValueSymbol>("sunbelt", Expression::Type::String),
		std::make_shared<ValueSymbol>("superantispyware", Expression::Type::String),
		std::make_shared<ValueSymbol>("symantec", Expression::Type::String),
		std::make_shared<ValueSymbol>("tencent", Expression::Type::String),
		std::make_shared<ValueSymbol>("thehacker", Expression::Type::String),
		std::make_shared<ValueSymbol>("totaldefense", Expression::Type::String),
		std::make_shared<ValueSymbol>("trendmicro", Expression::Type::String),
		std::make_shared<ValueSymbol>("trendmicro_housecall", Expression::Type::String),
		std::make_shared<ValueSymbol>("vba32", Expression::Type::String),
		std::make_shared<ValueSymbol>("vipre", Expression::Type::String),
		std::make_shared<ValueSymbol>("virobot", Expression::Type::String),
		std::make_shared<ValueSymbol>("yandex", Expression::Type::String),
		std::make_shared<ValueSymbol>("zillya", Expression::Type::String),
		std::make_shared<ValueSymbol>("zoner", Expression::Type::String)
	};
}

/**
 * Returns the string representation of the whole YARA file.
 *
 * @return String representation.
 */
std::string YaraFile::getText() const
{
	if (!hasImports() && !hasRules())
		return std::string();

	std::ostringstream ss;
	for (const auto& module : getImports())
		ss << "import \"" << module->getName() << "\"\n";

	if (!hasRules())
		return ss.str();

	// If there are some imports, separate them with one new line from rules.
	if (hasImports())
		ss << '\n';

	for (const auto& rule : getRules())
		ss << rule->getText() << "\n\n";

	// Remove last "\n\n" from the text.
	return trim(ss.str());
}

std::string YaraFile::getTextFormatted(bool withIncludes) const
{
	return getTokenStream()->getText(withIncludes);
}

/**
 * Adds the import of the module to the YARA file. Module needs
 * to exist and be defined in @c types/modules folder.
 *
 * @param import Imported module name.
 *
 * @return @c true if module was found, @c false otherwise.
 */
bool YaraFile::addImport(TokenIt import, ModulesPool& modules)
{
	auto module = modules.load(import->getPureText(), _importFeatures);
	if (!module)
		return false;

	// We don't want duplicates.
	if (_importTable.find(import->getPureText()) != _importTable.end())
		return true;

	_imports.push_back(std::move(module));
	_importTable.emplace(_imports.back()->getName(), _imports.back().get());
	return true;
}

/**
 * Adds the rule to the YARA file.
 *
 * @param rule Rule to add.
 */
void YaraFile::addRule(Rule&& rule)
{
	addRule(std::make_unique<Rule>(std::move(rule)));
}

/**
 * Adds the rule to the YARA file.
 *
 * @param rule Rule to add.
 */
void YaraFile::addRule(std::unique_ptr<Rule>&& rule)
{
	_rules.emplace_back(std::move(rule));
	_ruleTable.emplace(_rules.back()->getName(), _rules.back().get());
}

/**
 * Adds the rule to the YARA file.
 *
 * @param rule Rule to add.
 */
void YaraFile::addRule(const std::shared_ptr<Rule>& rule)
{
	_rules.emplace_back(rule);
	_ruleTable.emplace(rule->getName(), _rules.back().get());
}

/**
 * Adds the rules to the YARA file.
 *
 * @param rules Rules to add.
 */
void YaraFile::addRules(const std::vector<std::shared_ptr<Rule>>& rules)
{
	for (const auto& rule : rules)
		addRule(rule);
}

/**
 * Adds the imports of the modules to the YARA file. Modules need
 * to exist and be defined in @c types/modules folder.
 *
 * @param imports Imported modules names.
 *
 * @return @c true if modules were found, @c false otherwise.
 */
bool YaraFile::addImports(const std::vector<TokenIt>& imports, ModulesPool& modules)
{
	for (const TokenIt& module : imports)
	{
		if (!addImport(module, modules))
			return false;
	}

	return true;
}

/**
 * Insert single rule at the specified position to the YARA file.
 *
 * @param position Position to insert rule at.
 * @param rule Rule to insert.
 */
void YaraFile::insertRule(std::size_t position, std::unique_ptr<Rule>&& rule)
{
	position = std::min(position, _rules.size());
	_rules.insert(_rules.begin() + position, std::move(rule));
	_ruleTable.emplace(_rules[position]->getName(), _rules[position].get());
}

/**
 * Insert single rule at the specified position to the YARA file.
 *
 * @param position Position to insert rule at.
 * @param rule Rule to insert.
 */
void YaraFile::insertRule(std::size_t position, const std::shared_ptr<Rule>& rule)
{
	position = std::min(position, _rules.size());
	_rules.insert(_rules.begin() + position, rule);
	_ruleTable.emplace(_rules[position]->getName(), _rules[position].get());
}

/**
 * Returns all imported modules from the YARA file in order they were added.
 *
 * @return All imported modules.
 */
const std::vector<std::shared_ptr<Module>>& YaraFile::getImports() const
{
	return _imports;
}

/**
 * Returns the whole tokenStream of this file.
 *
 * @return _tokenStream.
 */
TokenStream* YaraFile::getTokenStream() const
{
	return _tokenStream.get();
}

/**
 * Returns all rules from the YARA file in order they were added.
 *
 * @return All rules.
 */
const std::vector<std::shared_ptr<Rule>>& YaraFile::getRules() const
{
	return _rules;
}

/**
 * Finds the symbol in the YARA file. Symbol is either rule name or module identifier.
 *
 * @param name Name of the symbol to search for.
 *
 * @return Returns valid symbol if it was found, @c nullptr otherwise.
 */
std::shared_ptr<Symbol> YaraFile::findSymbol(const std::string& name) const
{
	// @todo Should rules have priority over imported modules?
	if (auto itr = _ruleTable.find(name); itr != _ruleTable.end())
		return itr->second->getSymbol();

	if (auto itr = _importTable.find(name); itr != _importTable.end())
		return itr->second->getStructure();

	for (const auto& vtSymbol : _vtSymbols)
	{
		if (vtSymbol->getName() == name)
			return vtSymbol;
	}

	return nullptr;
}

/**
 * Returns whether the YARA file contains any imported modules.
 *
 * @return @c true if it contains, otherwise @c false.
 */
bool YaraFile::hasImports() const
{
	return !_imports.empty();
}

/**
 * Returns whether the YARA file contains any rules.
 *
 * @return @c true if it contains, otherwise @c false.
 */
bool YaraFile::hasRules() const
{
	return !_rules.empty();
}

/**
 * Returns whether the YARA file contains specified rule.
 *
 * @return @c true if it contains, otherwise @c false.
 */
bool YaraFile::hasRule(const std::string& name) const
{
	return _ruleTable.find(name) != _ruleTable.end();
}

}
