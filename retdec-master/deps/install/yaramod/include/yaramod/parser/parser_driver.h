/**
 * @file src/parser/parser_driver.h
 * @brief Declaration of class ParserDriver.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <climits>
#include <fstream>
#include <memory>
#include <unordered_map>

#define FMT_HEADER_ONLY 1

// Uncomment for debugging
// #define POG_DEBUG 1

#include <pog/pog.h>

#include "yaramod/parser/file_context.h"
#include "yaramod/parser/value.h"
#include "yaramod/types/expressions.h"
#include "yaramod/types/meta.h"
#include "yaramod/types/rule.h"
#include "yaramod/types/symbol.h"
#include "yaramod/types/token_stream.h"
#include "yaramod/types/yara_file.h"
#include "yaramod/utils/trie.h"
#include "yaramod/yaramod_error.h"

namespace yaramod {

/**
 * Specifies different parsing modes.
 */
enum class ParserMode
{
	Regular, ///< In this mode, parser behaves like regular YARA parser
	IncludeGuarded ///< Parser provides protection against inclusion of the same file multiple times
};

/**
 * Result of including a file.
 */
enum IncludeResult
{
	Error, ///< Failed to include a file
	AlreadyIncluded, ///< File was already included before
	Included ///< File was successfully included
};

/**
 * Represents error during parsing.
 */
class ParserError : public YaramodError
{
public:
	ParserError(const std::string& errorMsg)
		: YaramodError(errorMsg)
	{
	}
	ParserError(const ParserError&) = default;
};

/**
 * Class representing handler of pog parser.
 * It also serves as context storage for parsing.
 *
 * ParserDriver driver;
 * driver.parse(input, ParserMode::Regular);
 * result = driver.getParsedFile();
 * driver.reset();
 */
class ParserDriver
{
public:
	/// @name Constructors
	/// @{
	ParserDriver(ImportFeatures features = ImportFeatures::AllCurrent);
	/// @}

	/// @name Destructor
	/// @{
	~ParserDriver() = default;
	/// @}

	/// @name Getter methods
	/// @{
	YaraFile&& getParsedFile();
	const YaraFile& getParsedFile() const;
	/// @}

	/// @name Parsing methods
	/// @{
	bool parse(std::istream& stream, ParserMode mode = ParserMode::Regular);
	bool parse(const std::string& filePath, ParserMode mode = ParserMode::Regular);
	/// @}

	/// @name Detection methods
	/// @{
	bool isValid() const;
	/// @}

	/// @name Methods for handling comments
	/// @{
	void addComment(TokenIt comment);
	/// @}

protected:
	/// @name Method for parsing
	/// @{
	void initialize();
	bool prepareParser(ParserMode parserMode);
	bool parseImpl();
	void reset(ParserMode parserMode);
	/// @}

	/// @name Methods for handling includes
	/// @{
	bool includeFile(const std::string& includePath, const std::shared_ptr<TokenStream>& tokenStream);
	IncludeResult includeFileImpl(const std::string& includePath, std::optional<std::shared_ptr<TokenStream>> = std::nullopt);
	bool isAlreadyIncluded(const std::string& includePath);
	bool includeEnd();
	/// @}

	/// @name Methods for handling rules
	/// @{
	bool ruleExists(const std::string& name) const;
	void addRule(Rule&& rule);
	void addRule(std::unique_ptr<Rule>&& rule);
	/// @}

	/// @name Methods for handling strings
	/// @{
	bool stringExists(const std::string& id) const;
	const Literal* findStringDefinition(const std::string& id) const;
	void setCurrentStrings(const std::shared_ptr<Rule::StringsTrie>& currentStrings);
	bool sectionStrings() const { return _sectionStrings; };
	void sectionStrings(bool new_value) { _sectionStrings = new_value; };
	/// @}

	/// @name Methods for parser maintainance
	/// @{
	void defineTokens();
	void defineGrammar();
	template <typename... Args> TokenIt emplace_back(Args&&... args);
	void enter_state(const std::string& state);
	void push_input_stream(std::istream& input) { _parser.push_input_stream(input); }
	/// @}

	/// @name Methods for handling for loops
	/// @{
	bool isInStringLoop() const;
	void stringLoopEnter();
	void stringLoopLeave();
	/// @}

	/// @name Methods for handling symbols
	/// @{
	std::shared_ptr<Symbol> findSymbol(const std::string& name) const;
	bool addLocalSymbol(const std::shared_ptr<Symbol>& symbol);
	void removeLocalSymbol(const std::string& name);
	/// @}

	/// @name Methods for handling anonymous strings
	/// @{
	bool isAnonymousStringId(const std::string& stringId) const;
	std::string generateAnonymousStringPseudoId();
	/// @}

	/// @name Methods for handling file contexts
	/// @{
	const std::shared_ptr<TokenStream>& currentTokenStream() const { return currentFileContext()->getTokenStream(); }
	FileContext* currentFileContext() { return &_fileContexts.back(); }
	const FileContext* currentFileContext() const { return &_fileContexts.back(); }
	void popFileContext() { _fileContexts.pop_back(); }
	/// @}

private:
	std::string _strLiteral; ///< Currently processed string literal.
	std::string _indent; ///< Variable storing current indentation
	std::string _comment; ///< For incremental construction of parsed comments
	std::string _regexpClass; ///< Currently processed regular expression class.
	pog::Parser<Value> _parser; ///< used pog parser
	
	bool _sectionStrings = false; ///< flag used to determine if we parse section after 'strings:'
	bool _escapedContent = false; ///< flag used to determine if a currently parsed literal contains hexadecimal byte (such byte must be unescaped in getPureText())

	ParserMode _mode; ///< Parser mode.

	ImportFeatures _import_features; ///< Used to determine whether to include Avast-specific or VirusTotal-specific symbols or to skip them
	ModulesPool _modules; ///< Storage of all modules used by this ParserDriver

	std::vector<FileContext> _fileContexts;
	std::vector<TokenIt> _comments; ///< Tokens of parsed comments

	std::vector<std::shared_ptr<std::istream>> _includedFiles; ///< Stack of included files
	std::unordered_set<std::string> _includedFilesCache; ///< Cache of already included files

	bool _valid; ///< Validity
	YaraFile _file; ///< Parsed file

	std::weak_ptr<Rule::StringsTrie> _currentStrings; ///< Context storage of current strings trie
	bool _stringLoop; ///< Context storage of for loop indicator
	std::unordered_map<std::string, std::shared_ptr<Symbol>> _localSymbols; ///< Context storage of local symbols

	Location _lastRuleLocation; ///< Holds the filename and line number where the last parsed rule starts
	std::shared_ptr<TokenStream> _lastRuleTokenStream; ///< Holds token stream at the point of where last parsed rule starts
	std::uint64_t _anonStringCounter; ///< Internal counter for generating pseudo identifiers of anonymous strings
	Location _errorLocation; ///< Last known location before error in parsing happened.
};

} // namespace yaramod
