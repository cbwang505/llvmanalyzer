/**
 * @file src/builder/yara_rule_builder.h
 * @brief Declaration of class YaraRuleBuilder.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <string>

#include "yaramod/types/hex_string.h"
#include "yaramod/types/rule.h"
#include "yaramod/yaramod_error.h"

namespace yaramod {

/**
 * Represents error during building.
 */
class RuleBuilderError : public YaramodError
{
public:
	RuleBuilderError(const std::string& errorMsg) : YaramodError("YaraRuleBuilder error: " + errorMsg) {}
	RuleBuilderError(const RuleBuilderError&) = default;
};

/**
 * Class representing builder of YARA rules. You use this builder
 * to specify what you want in your YARA rule and then you can obtain
 * your YARA rule by calling method @c get. As soon as @c get is called,
 * builder resets to default state and does not contain any data from
 * the previous build process.
 */
class YaraRuleBuilder
{
public:
	/// @name Constructor
	/// @{
	YaraRuleBuilder();
	YaraRuleBuilder(const std::shared_ptr<TokenStream>& tokenStream);
	/// @}

	/// @name Build method
	/// @{
	std::unique_ptr<Rule> get();
	/// @}

	/// @name Building methods
	/// @{
	YaraRuleBuilder& withName(const std::string& name);
	YaraRuleBuilder& withModifier(Rule::Modifier mod);
	YaraRuleBuilder& withTag(const std::string& tag);
	YaraRuleBuilder& withComment(const std::string& comment, bool multiline = false);

	YaraRuleBuilder& withStringMeta(const std::string& key, const std::string& value);
	YaraRuleBuilder& withIntMeta(const std::string& key, std::int64_t value);
	YaraRuleBuilder& withUIntMeta(const std::string& key, std::uint64_t value);
	YaraRuleBuilder& withHexIntMeta(const std::string& key, std::uint64_t value);
	YaraRuleBuilder& withBoolMeta(const std::string& key, bool value);

	YaraRuleBuilder& withPlainString(const std::string& id, const std::string& value);
	YaraRuleBuilder& withHexString(const std::string& id, const std::shared_ptr<HexString>& hexString);
	YaraRuleBuilder& withRegexp(const std::string& id, const std::string& value, const std::string& suffixMods = std::string{});

	YaraRuleBuilder& withCondition(Expression::Ptr&& condition);
	YaraRuleBuilder& withCondition(const Expression::Ptr& condition);
	/// @}

	/// @name Method modifying last string
	/// @{
	YaraRuleBuilder& ascii();
	YaraRuleBuilder& wide();
	YaraRuleBuilder& nocase();
	YaraRuleBuilder& fullword();
	YaraRuleBuilder& private_();
	YaraRuleBuilder& xor_();
	YaraRuleBuilder& xor_(std::uint64_t key);
	YaraRuleBuilder& xor_(std::uint64_t low, std::uint64_t high);
	/// @}

private:
	void resetTokens();
	void initializeStrings();
	void createLastString();

	std::shared_ptr<TokenStream> _tokenStream; ///< Storage of all Tokens
	std::optional<TokenIt> _mod_private; ///< Private modifier
	std::optional<TokenIt> _mod_global; ///< Global modifier
	std::vector<TokenIt> _tags; ///< Tags
	std::vector<Meta> _metas; ///< Meta information
	std::shared_ptr<Rule::StringsTrie> _strings; ///< Strings
	Expression::Ptr _condition; ///< Condition expression

	TokenIt _rule_it; ///< iterator pointing at 'rule' token
	TokenIt _name_it; ///< iterator pointing at name token
	TokenIt _lcb; ///< iterator pointing at '{' token
	std::optional<TokenIt> _strings_it; ///< iterator pointing at 'strings' token
	TokenIt _condition_it; ///< iterator pointing at 'condition' token
	TokenIt _colon_it; ///< iterator pointing at ':' token
	TokenIt _rcb; ///< iterator pointing at '}' token

	std::shared_ptr<String> _lastString; ///< Points to the last defined string.
	std::vector<std::shared_ptr<StringModifier>> _stringMods; ///< String modifiers for last defined string.
	std::shared_ptr<TokenStream> _stringModsTokens; ///< Token stream for building string modifiers.
};

}
