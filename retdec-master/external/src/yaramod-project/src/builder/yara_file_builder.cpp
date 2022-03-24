/**
 * @file src/builder/yara_file_builder.cpp
 * @brief Implementation of class YaraFileBuilder.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/builder/yara_file_builder.h"
#include "yaramod/parser/parser_driver.h"

namespace yaramod {

/**
 * Returns the built YARA file and resets the builder back to default state.
 *
 * @param recheck @c true if generated file should be rechecked by parser.
 * @param driver optional driver to be used to recheck. The driver will be RESET!
 *
 * @return Built YARA file.
 */
std::unique_ptr<YaraFile> YaraFileBuilder::get(bool recheck, ParserDriver* external_driver)
{
	auto yaraFile = std::make_unique<YaraFile>(std::move(_tokenStream), _import_features);
	for (const auto& module_token : _module_tokens)
		yaraFile->addImport(module_token.second, _modules_pool);
	yaraFile->addRules(_rules);

	_module_tokens.clear();
	_rules.clear();
	_tokenStream = std::make_shared<TokenStream>();

	std::stringstream ss;
	ss << yaraFile->getText();

	if (recheck)
	{
		// Recheck the file by parsing it again
		// We are not able to perform all semantic checks while building so we need to do this
		std::unique_ptr<ParserDriver> driver;
		if (external_driver)
		{
			try
			{
				external_driver->parse(ss);
			}
			catch (const ParserError& err)
			{
				std::stringstream ss;
				ss << "Error: Recheck failed: parser error, parsing \n'" << yaraFile->getText() << "'" << std::endl << err.what() << std::endl;
				throw YaraFileBuilderError(ss.str());
			}
		}
		else
		{
			ParserDriver driver(ImportFeatures::AllCurrent);
			try
			{
				driver.parse(ss);
			}
			catch (const ParserError& err)
			{
				std::stringstream ss;
				ss << "Error: Recheck failed: parser error, parsing \n'" << yaraFile->getText() << "'" << std::endl << err.what() << std::endl;
				throw YaraFileBuilderError(ss.str());
			}
		}
	}

	return yaraFile;
}

void YaraFileBuilder::insertImportIntoTokenStream(TokenIt before, const std::string& moduleName)
{
	_tokenStream->emplace(before, TokenType::IMPORT_KEYWORD, "import");
	auto moduleToken = _tokenStream->emplace(before, TokenType::IMPORT_MODULE, moduleName);
	_tokenStream->emplace(before, NEW_LINE, "\n");
	_module_tokens.insert(std::make_pair(moduleName, moduleToken));
}

/**
 * Adds module to YARA file.
 *
 * @param moduleName Module name.
 *
 * @return Builder.
 */
YaraFileBuilder& YaraFileBuilder::withModule(const std::string& moduleName)
{
	TokenIt moduleToken;
	if (_module_tokens.empty())
	{
		auto before = _tokenStream->begin();
		insertImportIntoTokenStream(before, moduleName);
		_newline_after_imports = _tokenStream->emplace(before, NEW_LINE, "\n");
	}
	else
	{
		auto close_module = _module_tokens.lower_bound(moduleName);
		if (close_module == _module_tokens.end())
 			insertImportIntoTokenStream(_newline_after_imports, moduleName);
		else if (close_module->first != moduleName)
		{
			auto before = _tokenStream->findBackwards(TokenType::IMPORT_KEYWORD, close_module->second);
			insertImportIntoTokenStream(before, moduleName);
		}
	}

	return *this;
}

/**
 * Adds rule to YARA file.
 *
 * @param rule Rule.
 *
 * @return Builder.
 */
YaraFileBuilder& YaraFileBuilder::withRule(Rule&& rule)
{
	withRule(std::make_unique<Rule>(std::move(rule)));
	return *this;
}

/**
 * Adds rule to YARA file.
 *
 * @param rule Rule.
 *
 * @return Builder.
 */
YaraFileBuilder& YaraFileBuilder::withRule(std::unique_ptr<Rule>&& rule)
{
	if (!_rules.empty())
		_tokenStream->emplace_back(NEW_LINE, "\n");

	_tokenStream->moveAppend(rule->getTokenStream());
	_tokenStream->emplace_back(NEW_LINE, "\n");

	_rules.emplace_back(std::move(rule));
	return *this;
}

/**
 * Adds rule to YARA file.
 *
 * @param rule Rule.
 *
 * @return Builder.
 */
YaraFileBuilder& YaraFileBuilder::withRule(const std::shared_ptr<Rule>& rule)
{
	if (!_rules.empty() || !_module_tokens.empty())
		_tokenStream->emplace_back(NEW_LINE, "\n");

	_tokenStream->moveAppend(rule->getTokenStream());
	_tokenStream->emplace_back(NEW_LINE, "\n");
	_rules.emplace_back(rule);
	return *this;
}

}
