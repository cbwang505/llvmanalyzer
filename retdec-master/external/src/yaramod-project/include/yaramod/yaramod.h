/**
 * @file src/yaramod.h
 * @brief Declaration of yaramod interface.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define YARAMOD_VERSION_MAJOR 3
#define YARAMOD_VERSION_MINOR 7
#define YARAMOD_VERSION_PATCH 0
#define YARAMOD_VERSION_ADDEND ""

#define YARAMOD_VERSION STR(YARAMOD_VERSION_MAJOR) "." STR(YARAMOD_VERSION_MINOR) "." STR(YARAMOD_VERSION_PATCH) YARAMOD_VERSION_ADDEND

#define YARA_SYNTAX_VERSION "3.11"

#include <memory>

#include "yaramod/builder/yara_file_builder.h"
#include "yaramod/parser/parser_driver.h"
#include "yaramod/types/yara_file.h"

namespace yaramod {

class Yaramod
{
public:
	/*
	 * @param ParserMode
	 * Regular -- regular YARA parser
	 * IncludeGuarded -- protection against inclusion of the same file multiple times
	 *
	 * @param features determines iff we want to use aditional Avast-specific symbols or VirusTotal-specific symbols in the imported modules
	 */
	Yaramod(ImportFeatures features = ImportFeatures::AllCurrent) : _driver(features) {}
	/**
	 * Parses file at given path.
	 *
	 * @param filePath Path to the file.
	 * @param parserMode Parsing mode.
	 *
	 * @return Valid @c YaraFile instance if parsing succeeded, otherwise @c nullptr.
	 */
	std::unique_ptr<YaraFile> parseFile(const std::string& filePath, ParserMode parserMode = ParserMode::Regular);
	/**
	 * Parses input stream.
	 *
	 * @param inputStream Input stream.
	 * @param parserMode Parsing mode.
	 *
	 * @return Valid @c YaraFile instance if parsing succeeded, otherwise @c nullptr.
	 */
	std::unique_ptr<YaraFile> parseStream(std::istream& inputStream, ParserMode parserMode = ParserMode::Regular);

	const YaraFile& getParsedFile() const;

private:
	ParserDriver _driver;
};

}
