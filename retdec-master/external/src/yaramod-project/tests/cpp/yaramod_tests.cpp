/**
* @file tests/yaramod_tests.cpp
* @brief Tests for the YARA literal.
* @copyright (c) 2019 Avast Software, licensed under the MIT license
*/

#include <gtest/gtest.h>
#include <iostream>

#include "yaramod/types/plain_string.h"
#include "yaramod/yaramod.h"

using namespace ::testing;

namespace yaramod {
namespace tests {

class YaramodTests : public Test {};

TEST_F(YaramodTests,
MultipleParsePhasesWithSingleYaramod) {
	yaramod::Yaramod ymod;
	std::string input_text = R"(
rule empty_rule
{
	condition:
		true
}
)";
	std::stringstream input;
	input << input_text;
	auto yarafile = ymod.parseStream(input);

	ASSERT_EQ(1u, yarafile->getRules().size());
	const auto& rule = yarafile->getRules()[0];
	EXPECT_EQ("empty_rule", rule->getName());
	EXPECT_EQ(Rule::Modifier::None, rule->getModifier());
	EXPECT_EQ(0u, rule->getMetas().size());
	EXPECT_TRUE(rule->getStrings().empty());

	EXPECT_EQ(input_text, yarafile->getTokenStream()->getText());

	input_text = R"(
rule rule_1 {
	strings:
		$1 = "String from Rule 1"
	condition:
		true
}

rule rule_2
{
	strings:
		$1 = "String from Rule 2"
	condition:
		true
}

rule rule_3 {
	strings:
		$1 = "String from Rule 3"
	condition:
		true
}
)";
	input.clear();
	input << input_text;
	yarafile = ymod.parseStream(input);

	ASSERT_EQ(3u, yarafile->getRules().size());

	std::uint64_t ruleId = 1;
	for (const auto& rule : yarafile->getRules())
	{
		std::ostringstream stream;
		stream << "rule_" << ruleId;

		EXPECT_EQ(stream.str(), rule->getName());

		auto strings = rule->getStrings();
		ASSERT_EQ(1u, strings.size());

		auto str = strings[0];
		stream.str(std::string());
		stream.clear();
		stream << "String from Rule " << ruleId;

		ASSERT_TRUE(str->isPlain());
		EXPECT_EQ("$1", str->getIdentifier());
		EXPECT_EQ('"' + stream.str() + '"', str->getText());
		EXPECT_TRUE(static_cast<const PlainString*>(str)->isAscii());

		ruleId++;
	}

	std::string expected = R"(
rule rule_1
{
	strings:
		$1 = "String from Rule 1"
	condition:
		true
}

rule rule_2
{
	strings:
		$1 = "String from Rule 2"
	condition:
		true
}

rule rule_3
{
	strings:
		$1 = "String from Rule 3"
	condition:
		true
}
)";

	EXPECT_EQ(expected, yarafile->getTokenStream()->getText());
}

TEST_F(YaramodTests,
AddMeta) {
	yaramod::Yaramod ymod;

	std::stringstream input;
	input << R"(
	rule rule_with_added_metas {
		condition:
			true
	})";
	auto yarafile = ymod.parseStream(input);

	ASSERT_EQ(1u, yarafile->getRules().size());
	const auto& rule = yarafile->getRules()[0];
	
	uint64_t u = 42;
	rule->addMeta("int_meta", Literal(u));
	rule->addMeta("bool_meta", Literal(false));
	EXPECT_EQ(R"(
rule rule_with_added_metas
{
	meta:
		int_meta = 42
		bool_meta = false
	condition:
		true
}
)", yarafile->getTextFormatted());
}

TEST_F(YaramodTests,
SetMeta) {
	yaramod::Yaramod ymod;

	std::stringstream input;
	input << R"(
	rule rule_with_added_metas {
		meta:
			int_meta = 42
			bool_meta = false
		condition:
			true
	})";
	auto yarafile = ymod.parseStream(input);

	ASSERT_EQ(1u, yarafile->getRules().size());
	const auto& rule = yarafile->getRules()[0];
	
	auto meta = rule->getMetaWithName("int_meta");
	meta->setValue(Literal("forty two"));
	EXPECT_EQ(R"(
rule rule_with_added_metas
{
	meta:
		int_meta = "forty two"
		bool_meta = false
	condition:
		true
}
)", yarafile->getTextFormatted());
}

}
}
