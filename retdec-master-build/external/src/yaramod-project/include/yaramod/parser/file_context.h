/**
 * @file src/parser/file_context.h
 * @brief Declaration of class FileContext.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <memory>

#include "yaramod/types/location.h"
#include "yaramod/types/token_stream.h"

namespace yaramod {

/**
 * Represents the context of processing a file in parser.
 * Each file that is included creates its own file context.
 * Those file contexts are put into stack.
 */
class FileContext
{
public:
	FileContext(std::istream* stream) : FileContext("[stream]", stream) {}
	FileContext(const std::string& filePath, std::istream* stream) : _tokenStream(std::make_shared<TokenStream>()), _stream(stream), _location(filePath) {}
	FileContext(const std::string& filePath, std::istream* stream, const std::shared_ptr<TokenStream>& tokenStream) : _tokenStream(tokenStream), _stream(stream), _location(filePath) {}
	FileContext(const FileContext&) = delete;
	FileContext(FileContext&&) noexcept = default;

	bool isUnnamed() const { return _location.isUnnamed(); }

	const std::shared_ptr<TokenStream>& getTokenStream() const { return _tokenStream; }
	std::istream* getStream() const { return _stream; }
	Location& getLocation() { return _location; }
	const Location& getLocation() const { return _location; }

private:
	std::shared_ptr<TokenStream> _tokenStream;
	std::istream* _stream;
	Location _location;
};

} // namespace yaramod
