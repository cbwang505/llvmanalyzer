/**
* @file tests/builder_tests.cpp
* @brief Tests for the YARA builder.
* @copyright AVG Technologies s.r.o, All Rights Reserved
*/

#include <gtest/gtest.h>

#include "yaramod/builder/yara_expression_builder.h"
#include "yaramod/builder/yara_file_builder.h"
#include "yaramod/builder/yara_hex_string_builder.h"
#include "yaramod/builder/yara_rule_builder.h"

using namespace ::testing;

namespace yaramod {
namespace tests {

class BuilderTests : public Test {
public:
	BuilderTests() : driver() {}

	yaramod::ParserDriver driver;
};

TEST_F(BuilderTests,
EmptyFileWorks) {
	YaraFileBuilder newFile;
	auto yaraFile = newFile.get();

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ("", yaraFile->getText());
}

TEST_F(BuilderTests,
PureImportsWorks) {
	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withModule("elf")
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "elf"
import "pe"
)", yaraFile->getText());
}

TEST_F(BuilderTests,
PureImportsComplicateWorks) {
	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withModule("cuckoo")
		.withModule("elf")
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "cuckoo"
import "elf"
import "pe"
)", yaraFile->getText());

	yaraFile = newFile
		.withModule("cuckoo")
		.withModule("pe")
		.withModule("elf")
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "cuckoo"
import "elf"
import "pe"
)", yaraFile->getText());
}

TEST_F(BuilderTests,
UnnamedRuleWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule.get();
	assert(rule);
	EXPECT_EQ(R"(rule unknown {
	condition:
		true
})", rule->getText());

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule unknown {
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(rule unknown
{
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
UnnamedRuleWithImportsWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.withModule("cuckoo")
		.withModule("pe")
		.withModule("cuckoo")
		.withModule("elf")
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "cuckoo"
import "elf"
import "pe"

rule unknown {
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(import "cuckoo"
import "elf"
import "pe"

rule unknown
{
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
UnnamedRuleWithImportsWorks2) {
	YaraRuleBuilder newRule;
	auto rule = newRule.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("cuckoo")
		.withModule("pe")
		.withRule(std::move(rule))
		.withModule("cuckoo")
		.withModule("elf")
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "cuckoo"
import "elf"
import "pe"

rule unknown {
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(import "cuckoo"
import "elf"
import "pe"

rule unknown
{
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithCustomNameWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_custom_name")
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_custom_name {
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_custom_name
{
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithMetasWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_metas")
		.withStringMeta("string_meta", "string value")
		.withIntMeta("int_meta", 42)
		.withHexIntMeta("hex_int_meta", 0x42)
		.withBoolMeta("bool_meta", false)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_metas {
	meta:
		string_meta = "string value"
		int_meta = 42
		hex_int_meta = 0x42
		bool_meta = false
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_metas
{
	meta:
		string_meta = "string value"
		int_meta = 42
		hex_int_meta = 0x42
		bool_meta = false
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithEmptyStringMetaValueWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_metas")
		.withStringMeta("string_meta", "")
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_metas {
	meta:
		string_meta = ""
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_metas
{
	meta:
		string_meta = ""
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithTagsWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_tags")
		.withTag("Tag1")
		.withTag("Tag2")
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_tags : Tag1 Tag2 {
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_tags : Tag1 Tag2
{
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithModifierWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_modifier")
		.withModifier(Rule::Modifier::Global)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(global rule rule_with_modifier {
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(global rule rule_with_modifier
{
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithOnelineCommentWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_oneline_comment")
		.withModifier(Rule::Modifier::Global)
		.withComment("comment", false)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(false);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(global rule rule_with_oneline_comment {
	condition:
		true
})", yaraFile->getText());

	std::string expected = R"(// comment
global rule rule_with_oneline_comment
{
	condition:
		true
}
)";
	EXPECT_EQ(expected, yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithMultilineCommentWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_multiline_comment")
		.withModifier(Rule::Modifier::Global)
		.withComment("comment", true)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(false);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(global rule rule_with_multiline_comment {
	condition:
		true
})", yaraFile->getText());

	std::string expected = R"(/* comment */
global rule rule_with_multiline_comment
{
	condition:
		true
}
)";
	EXPECT_EQ(expected, yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithPlainStringWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_plain_string")
		.withPlainString("$1", "This is plain string.").ascii().wide()
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_plain_string {
	strings:
		$1 = "This is plain string." ascii wide
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_plain_string
{
	strings:
		$1 = "This is plain string." ascii wide
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithPlainStringPureWideWorks) {
	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_plain_string")
		.withPlainString("$1", "This is plain string.").wide()
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_plain_string {
	strings:
		$1 = "This is plain string." wide
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_plain_string
{
	strings:
		$1 = "This is plain string." wide
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
MultipleRulesWorks) {
	YaraRuleBuilder newRule;
	auto rule1 = newRule
		.withName("rule_1")
		.withTag("Tag1")
		.withUIntMeta("id", 1)
		.withPlainString("$1", "This is plain string 1.")
		.get();
	auto rule2 = newRule
		.withName("rule_2")
		.withTag("Tag2")
		.withUIntMeta("id", 2)
		.withPlainString("$2", "This is plain string 2.")
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule1))
		.withRule(std::move(rule2))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_1 : Tag1 {
	meta:
		id = 1
	strings:
		$1 = "This is plain string 1."
	condition:
		true
}

rule rule_2 : Tag2 {
	meta:
		id = 2
	strings:
		$2 = "This is plain string 2."
	condition:
		true
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_1 : Tag1
{
	meta:
		id = 1
	strings:
		$1 = "This is plain string 1."
	condition:
		true
}

rule rule_2 : Tag2
{
	meta:
		id = 2
	strings:
		$2 = "This is plain string 2."
	condition:
		true
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithCustomConditionWorks) {
	auto cond = matchAt("$1", entrypoint()).get();
	EXPECT_EQ("$1", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("entrypoint", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_custom_condition")
		.withPlainString("$1", "Hello World!")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_custom_condition {
	strings:
		$1 = "Hello World!"
	condition:
		$1 at entrypoint
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_custom_condition
{
	strings:
		$1 = "Hello World!"
	condition:
		$1 at entrypoint
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithConditionWithSymbolsWorks) {
	auto cond = forLoop(any(), "n", set({intVal(1), intVal(2), intVal(3)}), matchAt("$1", paren(entrypoint() + id("i")))).get();
	EXPECT_EQ("for", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());
	auto forExp = std::static_pointer_cast<ForIntExpression>(cond);
	EXPECT_EQ(forExp->getId(), "n");
	forExp->setId("i");
	EXPECT_EQ(forExp->getId(), "i");
	EXPECT_EQ(cond->getText(), "for any i in (1, 2, 3) : ( $1 at (entrypoint + i) )");

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_condition_with_symbols")
		.withPlainString("$1", "Hello World!")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_condition_with_symbols {
	strings:
		$1 = "Hello World!"
	condition:
		for any i in (1, 2, 3) : ( $1 at (entrypoint + i) )
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_condition_with_symbols
{
	strings:
		$1 = "Hello World!"
	condition:
		for any i in (1, 2, 3) : ( $1 at (entrypoint + i) )
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithHexStringWorks) {
	auto cond = stringRef("$1").get();
	EXPECT_EQ("$1", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("$1", cond->getLastTokenIt()->getPureText());

	auto alt1 = YaraHexStringBuilder().add(0xBB).add(0xCC);
	auto alt2 = YaraHexStringBuilder().add(0xDD, 0xEE);
	auto alt3 = YaraHexStringBuilder().add(0xFF, 0xF1);
	auto alt4 = YaraHexStringBuilder(std::vector<std::uint8_t>{ 0xFE, 0xED, 0xDC });

	YaraHexStringBuilder newHexStr;
	auto hexStr = newHexStr
		.add(0x11, 0x22, wildcard(), wildcardHigh(0xA), wildcardLow(0xB))
		.add(jumpVarying(), jumpFixed(5), jumpVaryingRange(3), jumpRange(3, 5))
		.add(alt(alt(alt1, alt2), alt3, alt4))
		.add(0x99)
		.get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_hex_string")
		.withHexString("$1", hexStr)
		.withCondition(cond)
		.get();

	ASSERT_NE(nullptr, rule);

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_hex_string {
	strings:
		$1 = { 11 22 ?? ?A B? [-] [5] [3-] [3-5] ( ( BB CC | DD EE ) | FF F1 | FE ED DC ) 99 }
	condition:
		$1
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_hex_string
{
	strings:
		$1 = { 11 22 ?? ?A B? [-] [5] [3-] [3-5] ( ( BB CC | DD EE ) | FF F1 | FE ED DC ) 99 }
	condition:
		$1
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithStringForConditionWorks) {
	auto cond = forLoop(any(), set({stringRef("$1"), stringRef("$2")}), matchAt("$", entrypoint())).get();
	EXPECT_EQ("for", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_string_for_condition")
		.withPlainString("$1", "Hello World!")
		.withPlainString("$2", "Ahoj Svet!")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_string_for_condition {
	strings:
		$1 = "Hello World!"
		$2 = "Ahoj Svet!"
	condition:
		for any of ($1, $2) : ( $ at entrypoint )
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_string_for_condition
{
	strings:
		$1 = "Hello World!"
		$2 = "Ahoj Svet!"
	condition:
		for any of ($1, $2) : ( $ at entrypoint )
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithOfWorks) {
	auto cond = of(all(), them()).get();
	EXPECT_EQ("all", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("them", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_of")
		.withPlainString("$1", "Hello World!")
		.withPlainString("$2", "Ahoj Svet!")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_of {
	strings:
		$1 = "Hello World!"
		$2 = "Ahoj Svet!"
	condition:
		all of them
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_of
{
	strings:
		$1 = "Hello World!"
		$2 = "Ahoj Svet!"
	condition:
		all of them
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithRangeWorks) {
	auto cond = matchInRange("$1", range(intVal(0), filesize())).get();
	EXPECT_EQ("$1", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_range")
		.withPlainString("$1", "Hello World!")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_range {
	strings:
		$1 = "Hello World!"
	condition:
		$1 in (0 .. filesize)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_range
{
	strings:
		$1 = "Hello World!"
	condition:
		$1 in (0 .. filesize)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithStructureWorks) {
	auto cond = (id("pe").access("number_of_sections") > intVal(1)).get();
	EXPECT_EQ("pe", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("1", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_range")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "pe"

rule rule_with_range {
	condition:
		pe.number_of_sections > 1
})", yaraFile->getText());

	EXPECT_EQ(R"(import "pe"

rule rule_with_range
{
	condition:
		pe.number_of_sections > 1
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithConditionWithOnelineComment) {
	auto cond = (id("pe").comment("Number of sections needs to exceed 1", false).access("number_of_sections") > intVal(1)).get();
	EXPECT_EQ("pe", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("1", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_range")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "pe"

rule rule_with_range {
	condition:
		pe.number_of_sections > 1
})", yaraFile->getText());

	EXPECT_EQ(R"(import "pe"

rule rule_with_range
{
	condition:
		// Number of sections needs to exceed 1
		pe.number_of_sections > 1
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithConditionWithMultilineComment) {
	auto cond = (id("pe").comment("Number of sections needs to exceed 1,\n\t\tbecause one is simply not enough.", true).access("number_of_sections") > intVal(1)).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_range")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "pe"

rule rule_with_range {
	condition:
		pe.number_of_sections > 1
})", yaraFile->getText());

	EXPECT_EQ(R"(import "pe"

rule rule_with_range
{
	condition:
		/* Number of sections needs to exceed 1,
		because one is simply not enough. */
		pe.number_of_sections > 1
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithArrayAndStructureWorks) {
	auto cond = id("pe").access("sections")[intVal(0)].access("name").contains(stringVal("text"))
		.get();
	EXPECT_EQ("pe", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("text", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_array_and_structure")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "pe"

rule rule_with_array_and_structure {
	condition:
		pe.sections[0].name contains "text"
})", yaraFile->getText());

	EXPECT_EQ(R"(import "pe"

rule rule_with_array_and_structure
{
	condition:
		pe.sections[0].name contains "text"
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithFunctionCallWorks) {
	auto cond = id("pe").access("exports")(stringVal("ExitProcess"))
		.get();
	EXPECT_EQ("pe", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_function_call")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "pe"

rule rule_with_function_call {
	condition:
		pe.exports("ExitProcess")
})", yaraFile->getText());

	EXPECT_EQ(R"(import "pe"

rule rule_with_function_call
{
	condition:
		pe.exports("ExitProcess")
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithIntFunctionWorks) {
	auto cond = (intVal(0).readUInt16(IntFunctionEndianness::Little) == hexIntVal(0x5A4D))
		.get();
	EXPECT_EQ("uint16", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("0x5a4d", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_int_function")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_int_function {
	condition:
		uint16(0) == 0x5a4d
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_int_function
{
	condition:
		uint16(0) == 0x5a4d
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithArithmeticOperationsWorks) {
	auto cond = (paren(entrypoint() + intVal(100) * intVal(3)) < paren(filesize() - intVal(100) / intVal(2)))
		.get();
	EXPECT_EQ("(", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_arithmetic_operations")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_arithmetic_operations {
	condition:
		(entrypoint + 100 * 3) < (filesize - 100 \ 2)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_arithmetic_operations
{
	condition:
		(entrypoint + 100 * 3) < (filesize - 100 \ 2)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithArithmeticOperationsWithDoubleValuesWorks) {
	auto cond = (paren(entrypoint() + doubleVal(2.71828) * intVal(10)) < paren(filesize() - doubleVal(1.61803) / doubleVal(3.14159)))
		.get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_arithmetic_operations_with_double_values")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_arithmetic_operations_with_double_values {
	condition:
		(entrypoint + 2.71828 * 10) < (filesize - 1.61803 \ 3.14159)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_arithmetic_operations_with_double_values
{
	condition:
		(entrypoint + 2.71828 * 10) < (filesize - 1.61803 \ 3.14159)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithBitwiseOperationsWorks) {
	auto cond = (id("pe").access("characteristics") & paren(id("pe").access("DLL") | id("pe").access("RELOCS_STRIPPED")))
		.get();
	EXPECT_EQ("pe", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_bitwise_operations")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "pe"

rule rule_with_bitwise_operations {
	condition:
		pe.characteristics & (pe.DLL | pe.RELOCS_STRIPPED)
})", yaraFile->getText());

	EXPECT_EQ(R"(import "pe"

rule rule_with_bitwise_operations
{
	condition:
		pe.characteristics & (pe.DLL | pe.RELOCS_STRIPPED)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithLogicOperationsWorks) {
	auto cond = (id("pe").access("is_32bit")() && paren((id("pe").access("is_dll")() || paren(id("pe").access("number_of_sections") > intVal(3)))))
		.get();
	EXPECT_EQ("pe", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_logic_operations")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "pe"

rule rule_with_logic_operations {
	condition:
		pe.is_32bit() and (pe.is_dll() or (pe.number_of_sections > 3))
})", yaraFile->getText());

	EXPECT_EQ(R"(import "pe"

rule rule_with_logic_operations
{
	condition:
		pe.is_32bit() and
		(
			pe.is_dll() or
			(pe.number_of_sections > 3)
		)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithIntMultpliersWorks) {
	auto cond = (intVal(100, IntMultiplier::Kilobytes) <= filesize() && filesize() <= intVal(1, IntMultiplier::Megabytes))
		.get();
	EXPECT_EQ("100KB", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("1MB", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_int_multipliers")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_int_multipliers {
	condition:
		100KB <= filesize and filesize <= 1MB
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_int_multipliers
{
	condition:
		100KB <= filesize and
		filesize <= 1MB
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithStringOperatorsWorks) {
	auto cond = (matchCount("$1") > intVal(0) && matchLength("$1") > intVal(1) && matchOffset("$1") > intVal(100))
		.get();
	EXPECT_EQ("#1", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("100", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_string_operators")
		.withPlainString("$1", "Hello World!")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_string_operators {
	strings:
		$1 = "Hello World!"
	condition:
		#1 > 0 and !1 > 1 and @1 > 100
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_string_operators
{
	strings:
		$1 = "Hello World!"
	condition:
		#1 > 0 and
		!1 > 1 and
		@1 > 100
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithRegexpWorks) {
	auto cond = stringRef("$1").get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_regexp")
		.withRegexp("$1", R"(md5: [0-9a-zA-Z]{32})", "i")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_regexp {
	strings:
		$1 = /md5: [0-9a-zA-Z]{32}/i
	condition:
		$1
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_regexp
{
	strings:
		$1 = /md5: [0-9a-zA-Z]{32}/i
	condition:
		$1
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithRegexpInConditionWorks) {
	auto cond = (id("pe").access("sections")[intVal(0)].access("name").matches(regexp(R"(\.(text|data))", "i")))
		.get();
	EXPECT_EQ("pe", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ("i", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_regexp_in_condition")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(import "pe"

rule rule_with_regexp_in_condition {
	condition:
		pe.sections[0].name matches /\.(text|data)/i
})", yaraFile->getText());

	EXPECT_EQ(R"(import "pe"

rule rule_with_regexp_in_condition
{
	condition:
		pe.sections[0].name matches /\.(text|data)/i
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithConjunctionInConditionWorks) {
	std::vector<YaraExpressionBuilder> terms = { stringRef("$1"), paren(matchOffset("$1") < intVal(100)), paren(entrypoint() == intVal(100)) };
	auto cond = conjunction(terms).get();
	EXPECT_EQ("$1", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_conjunction")
		.withPlainString("$1", "Hello")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_conjunction {
	strings:
		$1 = "Hello"
	condition:
		$1 and (@1 < 100) and (entrypoint == 100)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_conjunction
{
	strings:
		$1 = "Hello"
	condition:
		$1 and
		(@1 < 100) and
		(entrypoint == 100)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithDisjunctionInConditionWorks) {
	std::vector<YaraExpressionBuilder> terms = { stringRef("$1"), stringRef("$2"), paren(entrypoint() == intVal(100)) };
	auto cond = disjunction(terms).get();
	EXPECT_EQ("$1", cond->getFirstTokenIt()->getPureText());
	EXPECT_EQ(")", cond->getLastTokenIt()->getPureText());

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_disjunction")
		.withPlainString("$1", "Hello")
		.withPlainString("$2", "World")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_disjunction {
	strings:
		$1 = "Hello"
		$2 = "World"
	condition:
		$1 or $2 or (entrypoint == 100)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_disjunction
{
	strings:
		$1 = "Hello"
		$2 = "World"
	condition:
		$1 or
		$2 or
		(entrypoint == 100)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithConjunctionWithLinebreaksInConditionWorks) {
	std::vector<YaraExpressionBuilder> terms = { stringRef("$1"), paren(matchOffset("$1") < intVal(100)), paren(entrypoint() == intVal(100)) };
	auto cond = conjunction(terms, true).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_conjunction_with_linebreaks")
		.withPlainString("$1", "Hello")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_conjunction_with_linebreaks {
	strings:
		$1 = "Hello"
	condition:
		$1 and
		(@1 < 100) and
		(entrypoint == 100)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_conjunction_with_linebreaks
{
	strings:
		$1 = "Hello"
	condition:
		$1 and
		(@1 < 100) and
		(entrypoint == 100)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithDisjunctionWithLinebreaksInConditionWorks) {
	std::vector<YaraExpressionBuilder> terms = { stringRef("$1"), stringRef("$2"), paren(entrypoint() == intVal(100)) };
	auto cond = disjunction(terms, true).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_disjunction_with_linebreaks")
		.withPlainString("$1", "Hello")
		.withPlainString("$2", "World")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_disjunction_with_linebreaks {
	strings:
		$1 = "Hello"
		$2 = "World"
	condition:
		$1 or
		$2 or
		(entrypoint == 100)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_disjunction_with_linebreaks
{
	strings:
		$1 = "Hello"
		$2 = "World"
	condition:
		$1 or
		$2 or
		(entrypoint == 100)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithCommentedConjunctionInConditionWorks) {
	std::vector<std::pair<YaraExpressionBuilder, std::string>> terms;
	terms.emplace_back(std::make_pair(stringRef("$1"), "comment1"));
	terms.emplace_back(std::make_pair(paren(matchOffset("$1") < intVal(100)), "comment2"));
	terms.emplace_back(std::make_pair(paren(entrypoint() == intVal(100)), "comment3"));

	auto cond = conjunction(terms).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_commented_conjunction")
		.withPlainString("$1", "Hello")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_commented_conjunction {
	strings:
		$1 = "Hello"
	condition:
		$1 and
		(@1 < 100) and
		(entrypoint == 100)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_commented_conjunction
{
	strings:
		$1 = "Hello"
	condition:
		/* comment1 */
		$1 and
		/* comment2 */
		(@1 < 100) and
		/* comment3 */
		(entrypoint == 100)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithCommentedDisjunctionInConditionWorks) {
	std::vector<std::pair<YaraExpressionBuilder, std::string>> terms;
	terms.emplace_back(std::make_pair(stringRef("$1"), "Hello must be present"));
	terms.emplace_back(std::make_pair(stringRef("$2"), "World must be present"));
	terms.emplace_back(std::make_pair(paren(entrypoint() == intVal(100)), "entrypoint is 100"));

	auto cond = conjunction({boolVal(true), paren(disjunction(terms))}).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_commented_disjunction")
		.withPlainString("$1", "Hello")
		.withPlainString("$2", "World")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_commented_disjunction {
	strings:
		$1 = "Hello"
		$2 = "World"
	condition:
		true and ($1 or
		$2 or
		(entrypoint == 100))
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_commented_disjunction
{
	strings:
		$1 = "Hello"
		$2 = "World"
	condition:
		true and
		(
			/* Hello must be present */
			$1 or
			/* World must be present */
			$2 or
			/* entrypoint is 100 */
			(entrypoint == 100)
		)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithParenthesesWithLinebreaksInConditionWorks) {
	auto cond = (stringRef("$1") && paren(stringRef("$2") || stringRef("$3"), true)).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_parentheses_with_linebreaks")
		.withPlainString("$1", "Hello")
		.withPlainString("$2", "Cruel")
		.withPlainString("$3", "World")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_parentheses_with_linebreaks {
	strings:
		$1 = "Hello"
		$2 = "Cruel"
		$3 = "World"
	condition:
		$1 and (
			$2 or $3
		)
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_parentheses_with_linebreaks
{
	strings:
		$1 = "Hello"
		$2 = "Cruel"
		$3 = "World"
	condition:
		$1 and
		(
			$2 or
			$3
		)
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithEscapedSequencesWorks) {
	auto cond = (id("pe").access("rich_signature").access("clear_data") == stringVal("DanS\"\t\n\\\x01\xff")).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_double_quotes")
		.withStringMeta("str_meta", "Double \"\t\n\\\x01\xff quotes")
		.withPlainString("$str", "Double \"\t\n\\\x01\xff quotes")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withModule("pe")
		.withRule(std::move(rule))
		.get(true, &driver);

	ASSERT_NE(nullptr, yaraFile);

	std::string expected = R"(import "pe"

rule rule_with_double_quotes {
	meta:
		str_meta = "Double \"\t\n\\\x01\xff quotes"
	strings:
		$str = "Double \"\t\n\\\x01\xff quotes"
	condition:
		pe.rich_signature.clear_data == "DanS\"\t\n\\\x01\xff"
})";
	EXPECT_EQ(expected, yaraFile->getText());

	std::string expected_with_newline = R"(import "pe"

rule rule_with_double_quotes
{
	meta:
		str_meta = "Double \"\t\n\\\x01\xff quotes"
	strings:
		$str = "Double \"\t\n\\\x01\xff quotes"
	condition:
		pe.rich_signature.clear_data == "DanS\"\t\n\\\x01\xff"
}
)";
	EXPECT_EQ(expected_with_newline, yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithStringsWithDifferentKindsOfModifiers) {
	auto cond = of(all(), them()).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("rule_with_strings_with_different_kinds_of_modifiers")
		.withPlainString("$1", "Hello").ascii()
		.withPlainString("$2", "Cruel").wide()
		.withPlainString("$3", "World").ascii().wide().private_()
		.withHexString("$4", YaraHexStringBuilder{std::vector<std::uint8_t>{0x01, 0x02, 0x03, 0x04}}.get())
		.withRegexp("$5", "abc", "i").wide().nocase()
		.withPlainString("$6", "Bye").nocase().xor_()
		.withPlainString("$7", "Bye").fullword().xor_()
		.withPlainString("$8", "Bye").xor_(12)
		.withPlainString("$9", "Bye").xor_(1, 255)
		.withPlainString("$10", "Bye").xor_(128).xor_(1, 255)
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule rule_with_strings_with_different_kinds_of_modifiers {
	strings:
		$1 = "Hello"
		$2 = "Cruel" wide
		$3 = "World" ascii wide private
		$4 = { 01 02 03 04 }
		$5 = /abc/i wide nocase
		$6 = "Bye" nocase xor
		$7 = "Bye" fullword xor
		$8 = "Bye" xor(12)
		$9 = "Bye" xor(1-255)
		$10 = "Bye" xor(128)
	condition:
		all of them
})", yaraFile->getText());

	EXPECT_EQ(R"(rule rule_with_strings_with_different_kinds_of_modifiers
{
	strings:
		$1 = "Hello"
		$2 = "Cruel" wide
		$3 = "World" ascii wide private
		$4 = { 01 02 03 04 }
		$5 = /abc/i wide nocase
		$6 = "Bye" nocase xor
		$7 = "Bye" fullword xor
		$8 = "Bye" xor(12)
		$9 = "Bye" xor(1-255)
		$10 = "Bye" xor(128)
	condition:
		all of them
}
)", yaraFile->getTextFormatted());
}

TEST_F(BuilderTests,
RuleWithXorStringModifierOutOfBounds) {
	auto cond = of(all(), them()).get();

	try
	{
		YaraRuleBuilder newRule;
		auto rule = newRule
			.withName("rule_with_xor_string_modifier_out_of_bounds")
			.withPlainString("$1", "Hello").xor_(256)
			.withCondition(cond)
			.get();

		YaraFileBuilder newFile;
		auto yaraFile = newFile
			.withRule(std::move(rule))
			.get(true);

		FAIL() << "Builder did not throw an exception.";
	}
	catch (const YaramodError& err)
	{
		EXPECT_EQ("Error: XOR string modifier key is out of allowed range", err.getErrorMessage());
	}
}

TEST_F(BuilderTests,
RuleWithXorStringModifierOutOfBoundsLowerKey) {
	auto cond = of(all(), them()).get();

	try
	{
		YaraRuleBuilder newRule;
		auto rule = newRule
			.withName("rule_with_xor_string_modifier_out_of_bounds")
			.withPlainString("$1", "Hello").xor_(256, 256)
			.withCondition(cond)
			.get();

		YaraFileBuilder newFile;
		auto yaraFile = newFile
			.withRule(std::move(rule))
			.get(true);

		FAIL() << "Builder did not throw an exception.";
	}
	catch (const YaramodError& err)
	{
		EXPECT_EQ("Error: XOR string modifier key is out of allowed range", err.getErrorMessage());
	}
}

TEST_F(BuilderTests,
RuleWithXorStringModifierOutOfBoundsHigherKey) {
	auto cond = of(all(), them()).get();

	try
	{
		YaraRuleBuilder newRule;
		auto rule = newRule
			.withName("rule_with_xor_string_modifier_out_of_bounds")
			.withPlainString("$1", "Hello").xor_(1, 256)
			.withCondition(cond)
			.get();

		YaraFileBuilder newFile;
		auto yaraFile = newFile
			.withRule(std::move(rule))
			.get(true);

		FAIL() << "Builder did not throw an exception.";
	}
	catch (const YaramodError& err)
	{
		EXPECT_EQ("Error: XOR string modifier key is out of allowed range", err.getErrorMessage());
	}
}

TEST_F(BuilderTests,
RuleWithXorStringModifierLowerBoundGreaterThanHigherBound) {
	auto cond = of(all(), them()).get();

	try
	{
		YaraRuleBuilder newRule;
		auto rule = newRule
			.withName("rule_with_xor_string_modifier_lower_bound_greater_than_higher_bound")
			.withPlainString("$1", "Hello").xor_(2, 1)
			.withCondition(cond)
			.get();

		YaraFileBuilder newFile;
		auto yaraFile = newFile
			.withRule(std::move(rule))
			.get(true);

		FAIL() << "Builder did not throw an exception.";
	}
	catch (const YaramodError& err)
	{
		EXPECT_EQ("Error: XOR string modifier has lower bound of key greater then higher bound", err.getErrorMessage());
	}
}

TEST_F(BuilderTests,
ConjunctionWithSingleTerm) {
	auto cond = conjunction({boolVal(false)}).get();

	YaraRuleBuilder newRule;
	auto rule = newRule
		.withName("conjunction_with_single_term")
		.withCondition(cond)
		.get();

	YaraFileBuilder newFile;
	auto yaraFile = newFile
		.withRule(std::move(rule))
		.get(true);

	ASSERT_NE(nullptr, yaraFile);
	EXPECT_EQ(R"(rule conjunction_with_single_term {
	condition:
		false
})", yaraFile->getText());

	EXPECT_EQ(R"(rule conjunction_with_single_term
{
	condition:
		false
}
)", yaraFile->getTextFormatted());
}

}
}
