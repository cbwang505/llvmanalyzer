/**
 * @file src/types/rule.cpp
 * @brief Implementation of class Rule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <algorithm>

#include "yaramod/types/plain_string.h"
#include "yaramod/types/rule.h"
#include "yaramod/types/symbols.h"

namespace yaramod {

/**
 * Default constructor.
 */
Rule::Rule()
	: _tokenStream(std::make_shared<TokenStream>())
{
	_name = _tokenStream->emplace_back(TokenType::RULE_NAME, "unknown");
}

/**
 * Constructor.
 *
 * @param tokenStream the TokenStream containing supplied tokens.
 * @param name Name of the rule as a token iterator.
 * @param mod_private Optional private modifier token iterator.
 * @param mod_global Optional global modifier token iterator.
 * @param metas Meta information.
 * @param strings Strings.
 * @param condition Condition expression.
 * @param tags Tags as token iterators.
 */
Rule::Rule(const std::shared_ptr<TokenStream>& tokenStream, TokenIt name, std::optional<TokenIt> mod_private, std::optional<TokenIt> mod_global, std::vector<Meta>&& metas, std::shared_ptr<StringsTrie>&& strings,
		Expression::Ptr&& condition, const std::vector<TokenIt>& tags)
	: _tokenStream(tokenStream)
	, _name(name)
	, _mod_private(mod_private)
	, _mod_global(mod_global)
	, _metas(std::move(metas))
	, _strings(std::move(strings))
	, _condition(std::move(condition))
	, _tags(tags)
	, _location({"[stream]", 0})
{
}

/**
 * Returns the string representation of the YARA rule.
 *
 * @return String representation.
 */
std::string Rule::getText() const
{
	auto indent = "\t\t";
	std::ostringstream ss;

	if (isPrivate())
		ss << "private ";
	if (isGlobal())
		ss << "global ";

	ss << "rule " << getName() << ' ';

	if (!_tags.empty())
	{
		const auto& tags = getTags();
		ss << ": ";
		std::for_each(tags.begin(), tags.end(),
				[&ss](const std::string& tag)
				{
					ss << tag << ' ';
				});
	}

	ss << "{\n";

	if (!getMetas().empty())
	{
		ss << "\tmeta:\n";
		std::for_each(getMetas().begin(), getMetas().end(),
				[&](const Meta& meta)
				{
					ss << indent << meta.getText() << '\n';
				});
	}

	auto strings = getStrings();
	if (!strings.empty())
	{
		ss << "\tstrings:\n";
		std::for_each(strings.begin(), strings.end(),
				[&](const auto& string)
				{
					ss << indent << string->getIdentifier() << " = " << string->getText() << '\n';
				});
	}
	ss << "\tcondition:\n" << indent << getCondition()->getText(indent) << "\n}";
	return ss.str();
}

/**
 * Returns the name of the YARA rule.
 *
 * @return Name.
 */
std::string Rule::getName() const
{
	return _name->getPureText();
}

/**
 * Returns the YARA rule modifier.
 *
 * @return Modifier.
 */
Rule::Modifier Rule::getModifier() const
{
	if (isPrivate() && isGlobal())
		return Rule::Modifier::PrivateGlobal;
	else if (isGlobal())
		return Rule::Modifier::Global;
	else if (isPrivate())
		return Rule::Modifier::Private;
	else
		return Rule::Modifier::None;
}

/**
 * Returns the meta information of the YARA rule.
 *
 * @return Meta information.
 */
std::vector<Meta>& Rule::getMetas()
{
	return _metas;
}

/**
 * Returns the meta information of the YARA rule.
 *
 * @return Meta information.
 */
const std::vector<Meta>& Rule::getMetas() const
{
	return _metas;
}

/**
 * Returns the strings of the YARA rule.
 *
 * @return Strings.
 */
std::vector<const String*> Rule::getStrings() const
{
	auto allValues = _strings->getAllValues();
	std::vector<const String*> result(allValues.size());
	std::transform(allValues.begin(), allValues.end(), result.begin(),
			[](const auto& string) {
				return string.get();
			});
	return result;
}

/**
 * Returns the strings of the YARA rule.
 *
 * @return Strings.
 */
const std::shared_ptr<Rule::StringsTrie>& Rule::getStringsTrie() const
{
	return _strings;
}

/**
 * Returns the condition expression of the YARA rule.
 *
 * @return Condition expression.
 */
const Expression::Ptr& Rule::getCondition() const
{
	return _condition;
}

/**
 * Returns the tags of the YARA rule.
 *
 * @return Tags.
 */
std::vector<std::string> Rule::getTags() const
{
	std::vector<std::string> output;
	output.reserve(_tags.size());
	for (const TokenIt& item : _tags)
		output.push_back(item->getPureText());
	return output;
}

/**
 * Returns the symbols of the YARA rule.
 *
 * @return Symbol.
 */
const std::shared_ptr<Symbol>& Rule::getSymbol() const
{
	return _name->getSymbol();
}

/**
 * Non-const version of @c getMetaWithName().
 */
Meta* Rule::getMetaWithName(const std::string& key)
{
	return const_cast<Meta*>(const_cast<const Rule*>(this)->getMetaWithName(key));
}

/**
 * Returns the meta with the given key if one exists.
 *
 * @param key Key of the meta.
 *
 * @return Pointer to meta if meta with the given key exists,
 *         @c nullptr otherwise.
 */
const Meta* Rule::getMetaWithName(const std::string& key) const
{
	for (auto& meta : _metas)
	{
		if (meta.getKey() == key)
			return &meta;
	}

	return nullptr;
}

/**
 * Returns the absolute path of a file in which this rule was located.
 * Returns "[stream]" in case this rule was parsed from input stream and not a file,
 * or if this file was created with `YaraRuleBuilder`.
 *
 * @return Location of the rule.
 */
const Rule::Location& Rule::getLocation() const
{
	return _location;
}


/**
 * Returns the first Token iterator associated with this Rule.
 *
 * @return First TokenIt of the rule.
 */
TokenIt Rule::getFirstTokenIt() const
{
	return _tokenStream->findBackwards(RULE, _name);
}

/**
 * Returns the last Token iterator associated with this Rule.
 *
 * @return Last TokenIt of the rule.
 */
TokenIt Rule::getLastTokenIt() const
{
	return _tokenStream->find(RULE_END, _name);
}

/**
 * Sets the name of the rule.
 *
 * @param name Name of the rule.
 */
void Rule::setName(const std::string& name)
{
	if (_name->isString())
		_name->setValue(name);
	else
	{
		assert(_name->isSymbol());
		_name->getSymbol()->setName(name);
	}
}

/**
 * Sets the metas of the rule.
 *
 * @param metas Metas to set.
 */
void Rule::setMetas(const std::vector<Meta>& metas)
{
	_metas = metas;
}

/**
 * Sets the tags of the rule.
 *
 * @param tags Tags to set.
 */
void Rule::setTags(const std::vector<std::string>& tags)
{
	TokenIt insert_before;
	if (_tags.empty())
	{
		insert_before = std::find_if(_name, _tokenStream->end(),
			[](const Token& t){ return t.getType() == NEW_LINE || t.getType() == RULE_BEGIN; }
			);
		assert(insert_before != _tokenStream->end() && "Called setTags on rule that does not contain '{'");
		_tokenStream->emplace(insert_before, TokenType::COLON, ":");
	}
	else
	{
		//delete all tags from tokenStream
		for (const TokenIt& it : _tags)
			insert_before = _tokenStream->erase(it);
	}
	_tags = std::vector<TokenIt>();
	// Insert new tags into TokenStream
	for (const auto& tag : tags)
	{
		TokenIt tagIt = _tokenStream->insert(insert_before, TokenType::TAG, Literal(tag));
		_tags.push_back(tagIt);
	}
}

/**
 * Sets the condition expression of the YARA rule.
 *
 * @param condition Condition expression.
 */
void Rule::setCondition(const Expression::Ptr& condition)
{
	_condition = condition;
}

/**
 * Sets the location of the rule.
 *
 * @param location Location to set.
 */
void Rule::setLocation(const std::string& filePath, std::uint64_t lineNumber)
{
	_location = { filePath, lineNumber };
}

void Rule::setModifier(const Modifier& modifier)
{
	bool deletePrivate = false;
	bool deleteGlobal = false;
	bool addPrivate = false;
	bool addGlobal = false;
	if (modifier == Modifier::None)
	{
		deleteGlobal = isGlobal();
		deletePrivate = isPrivate();
	}
	else if (modifier == Modifier::Private)
	{
		deleteGlobal = isGlobal();
		addPrivate = ! isPrivate();
	}
	else if (modifier == Modifier::Global)
	{
		deletePrivate = isPrivate();
		addGlobal = ! isGlobal();
	}
	else if (modifier == Modifier::PrivateGlobal)
	{
		addPrivate = ! isPrivate();
		addGlobal = ! isGlobal();
	}
	else
		assert(false && "Invalid rule modifier");

	if (deletePrivate)
	{
		_tokenStream->erase(_mod_private.value());
		_mod_private.reset();
	}
	if (deleteGlobal)
	{
		_tokenStream->erase(_mod_global.value());
		_mod_global.reset();
	}
	if (addPrivate || addGlobal)
	{
		TokenIt rule_token = _tokenStream->findBackwards(RULE, _name);
		if (addPrivate)
		{
			if (isGlobal())
				_mod_private = _tokenStream->emplace(_mod_global.value(), PRIVATE, "private");
			else
				_mod_private = _tokenStream->emplace(rule_token, PRIVATE, "private");
		}
		if (addGlobal)
			_mod_global = _tokenStream->emplace(rule_token, GLOBAL, "global");
	}
}

/**
 * Returns whether the rule has global modifier set.
 *
 * @return @c true if is global, otherwise @c false.
 */
bool Rule::isGlobal() const
{
	return _mod_global.has_value();
}

/**
 * Returns whether the rule has private modifier set.
 *
 * @return @c true if is private, otherwise @c false.
 */
bool Rule::isPrivate() const
{
	return _mod_private.has_value();
}

/**
 * Adds meta with specified name and value.
 *
 * @param name Name of the meta.
 * @param value Value of the meta.
 */
void Rule::addMeta(const std::string& name, const Literal& value)
{
	// first we need to find a proper placing for the meta within the tokenstream:
	TokenIt insert_before;
	if (_metas.empty())
	{
		insert_before = _tokenStream->find(TokenType::RULE_BEGIN, _name);
		assert(insert_before != _tokenStream->end() && "Called addMeta on rule that does not contain '{'");
		++insert_before;
		_tokenStream->emplace(insert_before, TokenType::NEW_LINE, _tokenStream->getNewLineStyle());
		_tokenStream->emplace(insert_before, TokenType::META, "meta");
		_tokenStream->emplace(insert_before, TokenType::COLON, ":");
	}
	else
	{
		insert_before = _metas.back().getValueTokenIt();
		++insert_before;
	}
	_tokenStream->emplace(insert_before, TokenType::NEW_LINE, _tokenStream->getNewLineStyle());
	auto itKey = _tokenStream->emplace(insert_before, TokenType::META_KEY, name);
	_tokenStream->emplace(insert_before, TokenType::EQ, "=");
	auto itValue = _tokenStream->emplace(insert_before, TokenType::META_VALUE, value);

	_metas.emplace_back(itKey, itValue);
}

/**
 * Removes all metas with the provided name as key.
 *
 * @param name Name of the meta.
 */
void Rule::removeMetas(const std::string& name)
{
	bool metasWasEmpty = _metas.empty();
	std::vector<std::pair<TokenIt, TokenIt>> toDelete;
	for (auto it = _metas.begin(); it != _metas.end(); ++it)
	{
		if (it->getKey() == name)
		{
			auto deleteTo = ++it->getValueTokenIt();
			if (deleteTo->getType() == NEW_LINE)
				++deleteTo;
			toDelete.emplace_back(it->getKeyTokenIt(), deleteTo);
		}
	}
	auto newEnd = std::remove_if(_metas.begin(), _metas.end(), [name](const auto& meta) {
		return meta.getKey() == name;
	});
	_metas.erase(newEnd, _metas.end());
	for (const auto& deletePair : toDelete)
	{
		_tokenStream->erase(deletePair.first, deletePair.second);
	}

	if (_metas.empty() && !metasWasEmpty)
	{
		auto metaToken = _tokenStream->find(META, _name);
		TokenIt deleteTo;
		if (!_strings->empty())
			deleteTo = _tokenStream->find(STRINGS, metaToken);
		else
			deleteTo = _tokenStream->find(CONDITION, metaToken);
		_tokenStream->erase(metaToken, deleteTo);
	}
}

/**
 * Removes string with the given identifier.
 *
 * @param id Identifier of the string to remove.
 */
void Rule::removeString(const std::string& id)
{
	_strings->remove(id);
}

/**
 * Adds new tag at the end.
 *
 * @param tag Tag to add.
 */
void Rule::addTag(const std::string& tag)
{
	//find iterator behind tags in TokenStream
	TokenIt end = ++_tags.back();
	TokenIt newTagIt = _tokenStream->insert(end, TokenType::TAG, Literal(tag));
	_tags.push_back(newTagIt);
}

/**
 * Removes tag from the rule.
 *
 * @param tag Tag to remove.
 */
void Rule::removeTags(const std::string& tag)
{
	auto found = std::find_if(_tags.begin(), _tags.end(), [&tag](TokenIt it){ return it->getText() == tag; });
	if (found != _tags.end())
	{
		_tokenStream->erase(*found);
		_tags.erase(found);
	}
}

/**
 * Removes tag from the rule.
 *
 * @param type TokenType of the tag to remove.
 */
void Rule::removeTags(TokenType type)
{
	auto found = std::find_if(_tags.begin(), _tags.end(), [&type](TokenIt it){ return it->getType() == type; });
	if (found != _tags.end())
	{
		_tokenStream->erase(*found);
		_tags.erase(found);
	}
}

}
