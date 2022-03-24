/**
 * @file src/builder/yara_file_builder.h
 * @brief Declaration of class YaraFileBuilder.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "yaramod/parser/parser_driver.h"
#include "yaramod/types/yara_file.h"

namespace yaramod {

/**
 * Represents error during parsing.
 */
class YaraFileBuilderError : public YaramodError
{
public:
	YaraFileBuilderError(const std::string& errorMsg) : YaramodError("YaraExpressionBuilder error: " + errorMsg) {}
	YaraFileBuilderError(const YaraFileBuilderError&) = default;
};

/**
 * Class representing builder of YARA files. You use this builder
 * to specify what you want in your YARA file and then you can obtain
 * your YARA file by calling method @c get. As soon as @c get is called,
 * builder resets to default state and does not contain any data from
 * the previous build process.
 */
class YaraFileBuilder
{
public:
	/// @name Constructors
	/// @{
	YaraFileBuilder(ImportFeatures features = ImportFeatures::AllCurrent)
		: _tokenStream(std::make_shared<TokenStream>())
		, _import_features(features)
	{
	}
	/// @}

	/// @name Build method
	/// @{
	std::unique_ptr<YaraFile> get(bool recheck = false, ParserDriver* external_driver = nullptr);
	/// @}

	/// @name Building methods
	/// @{
	YaraFileBuilder& withModule(const std::string& moduleName);
	YaraFileBuilder& withRule(Rule&& rule);
	YaraFileBuilder& withRule(std::unique_ptr<Rule>&& rule);
	YaraFileBuilder& withRule(const std::shared_ptr<Rule>& rule);
	/// @}

protected:
	void insertImportIntoTokenStream(TokenIt before, const std::string& moduleName);

private:
	std::shared_ptr<TokenStream> _tokenStream; ///< Tokens storage
	std::map<std::string, TokenIt> _module_tokens; ///< Modules
	TokenIt _newline_after_imports; ///< Always stands behind newline after last import in the TokenStream
	ImportFeatures _import_features; ///< Determines which modules should be possible to load
	ModulesPool _modules_pool; ///< Storage of used modules
	std::vector<std::shared_ptr<Rule>> _rules; ///< Rules
};

}
