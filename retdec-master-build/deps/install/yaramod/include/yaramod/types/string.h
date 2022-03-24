/**
 * @file src/types/string.h
 * @brief Declaration of class String.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "yaramod/types/string_modifier.h"
#include "yaramod/types/token_stream.h"
#include "yaramod/yaramod_error.h"

namespace yaramod {

/**
 * Abstract class for representing string in YARA rules.
 * This class is subclassed into @c PlainString, @c HexString
 * and @c Regexp. It can be also subclassed to any further types
 * added in the future.
 */
class String
{
public:
	/// Type of string.
	enum class Type
	{
		Plain,
		Hex,
		Regexp
	};

	/// @name Constructors
	/// @{
	explicit String(Type type)
		: String(std::make_shared<TokenStream>(), type)
	{
	}

	explicit String(const std::shared_ptr<TokenStream>& ts, Type type)
		: _tokenStream(ts)
		, _type(type)
		, _mods()
	{
		assert(_tokenStream);
	}

	explicit String(const std::shared_ptr<TokenStream>& ts, Type type, const std::string& id)
		: _tokenStream(ts)
		, _type(type)
		, _mods()
	{
		assert(_tokenStream);
		_id = _tokenStream->emplace_back(STRING_KEY, id);
		_assignToken = _tokenStream->emplace_back(ASSIGN, "=");
	}

	explicit String(const std::shared_ptr<TokenStream>& ts, Type type, TokenIt id, TokenIt assignToken)
		: _tokenStream(ts)
		, _type(type)
		, _id(id)
		, _assignToken(assignToken)
		, _mods()
	{
		assert(_tokenStream);
	}

	virtual ~String() = default;
	/// @}

	/// @name String representation
	/// @{
	virtual std::string getText() const = 0;
	virtual std::string getPureText() const = 0;
	/// @}

	/// @name Getter methods
	/// @{
	Type getType() const { return _type; }
	const Literal* getIdentifierTokenIt() const
	{
		if (_id)
			return &(_id.value()->getLiteral());
		else
			return nullptr;
	}
	std::string getIdentifier() const
	{
		if (_id)
			return _id.value()->getPureText();
		else
			return std::string();
	}

	std::string getModifiersText() const
	{
		if (_mods.empty())
			return {};

		// ASCII is the only modifier, it that case don't write anything
		auto asciiItr = _mods.find(StringModifier::Type::Ascii);
		if (asciiItr != _mods.end() && _mods.size() == 1u)
			return {};

		auto orderedTypes = {
			StringModifier::Type::Ascii,
			StringModifier::Type::Wide,
			StringModifier::Type::Nocase,
			StringModifier::Type::Fullword,
			StringModifier::Type::Private,
			StringModifier::Type::Xor
		};

		std::string text;
		for (const auto& key : orderedTypes)
		{
			if (auto itr = _mods.find(key); itr != _mods.end())
				text += ' ' + itr->second->getText();
		}

		return text;
	}

	const std::shared_ptr<TokenStream>& getTokenStream() const { return _tokenStream; }

	virtual TokenIt getFirstTokenIt() const = 0;
	virtual TokenIt getLastTokenIt() const = 0;
	/// @}

	/// @name Setter methods
	/// @{
	void setIdentifier(std::string&& id)
	{
		if (_id)
			_id.value()->setValue(std::move(id));
		else
		{
			auto first = getFirstTokenIt();
			_id = _tokenStream->emplace(first, STRING_KEY, std::move(id));
			_assignToken = _tokenStream->emplace(first, ASSIGN, "=");
		}
	}

	void setIdentifier(const std::string& id)
	{
		if (_id)
			_id.value()->setValue(id);
		else
		{
			auto first = getFirstTokenIt();
			_id = _tokenStream->emplace(first, STRING_KEY, id);
			_assignToken = _tokenStream->emplace(first, ASSIGN, "=");
		}
	}

	void setIdentifier(TokenIt id, TokenIt assignToken)
	{
		setIdentifier(id);
		_assignToken = assignToken;
	}

	void setIdentifier(TokenIt id)
	{
		if (!id->isString())
			throw YaramodError("String class identifier type must be string");
		if (_id && _id.value() != id)
			_tokenStream->erase(_id.value());
		_id = id;
	}

	// use only when not care about the order of mods in tokenstream
	void setModifiers(const std::vector<std::shared_ptr<StringModifier>>& mods, bool avoidSingleAscii = false)
	{
		_mods.clear();

		bool hasSingleAsciiOrEmpty = true;
		for (const auto& mod : mods)
			hasSingleAsciiOrEmpty = hasSingleAsciiOrEmpty && mod->isAscii();

		if (!avoidSingleAscii || !hasSingleAsciiOrEmpty)
		{
			for (const auto& mod : mods)
				addModifier(mod);
		}
	}

	void setModifiersWithTokens(const std::vector<std::shared_ptr<StringModifier>>& mods, const std::shared_ptr<TokenStream>& modTokens, bool avoidSingleAscii = false)
	{
		_mods.clear();

		bool hasSingleAsciiOrEmpty = true;
		for (const auto& mod : mods)
			hasSingleAsciiOrEmpty = hasSingleAsciiOrEmpty && mod->isAscii();

		if (!avoidSingleAscii || !hasSingleAsciiOrEmpty)
		{
			for (const auto& mod : mods)
			{
				if (addModifier(mod))
				{
					auto tokens = mod->getTokenRange();
					std::advance(tokens.second, 1); // Range of modifiers are always [first, last] and not [first, last) which is expected by splice()
					_tokenStream->moveAppend(modTokens.get(), tokens.first, tokens.second);
				}
			}
		}
	}

	bool addModifier(const std::shared_ptr<StringModifier>& modifier)
	{
		// Modifier already present
		auto itr = _mods.find(modifier->getType());
		if (itr != _mods.end())
			return false;

		_mods[modifier->getType()] = modifier;
		return true;
	}
	/// @}

	/// @name Detection
	/// @{
	bool isPlain() const { return _type == Type::Plain; }
	bool isHex() const { return _type == Type::Hex; }
	bool isRegexp() const { return _type == Type::Regexp; }

	bool isAscii() const
	{
		if (_mods.empty())
			return true;

		auto asciiItr = _mods.find(StringModifier::Type::Ascii);
		auto wideItr = _mods.find(StringModifier::Type::Wide);
		return asciiItr != _mods.end() || wideItr == _mods.end();
	}

	bool isWide() const { return _mods.find(StringModifier::Type::Wide) != _mods.end(); }
	bool isNocase() const { return _mods.find(StringModifier::Type::Nocase) != _mods.end(); }
	bool isFullword() const { return _mods.find(StringModifier::Type::Fullword) != _mods.end(); }
	bool isPrivate() const { return _mods.find(StringModifier::Type::Private) != _mods.end(); }
	bool isXor() const { return _mods.find(StringModifier::Type::Xor) != _mods.end(); }
	/// @}

protected:
	std::shared_ptr<TokenStream> _tokenStream; ///< shared_pointer to the TokenStream in which the data is stored
	Type _type; ///< Type of string //no need to store type of string in tokenstream - we just store the '"' or '/' characters
	std::optional<TokenIt> _id; ///< Optional TokenIt pointing to identifier in strings section
	std::optional<TokenIt> _assignToken; ///< Optional TokenIt pointing to '=' following _id
	std::unordered_map<StringModifier::Type, std::shared_ptr<StringModifier>> _mods; ///< String modifiers
};

}
