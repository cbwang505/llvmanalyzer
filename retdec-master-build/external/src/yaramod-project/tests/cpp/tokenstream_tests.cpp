/**
* @file tests/tokenstream_tests.cpp
* @brief Tests for the YARA literal.
* @copyright (c) 2019 Avast Software, licensed under the MIT license
*/

#include <gtest/gtest.h>
#include <iostream>

#include "yaramod/types/token_stream.h"
#include "yaramod/types/meta.h"

using namespace ::testing;

namespace yaramod {
namespace tests {

class TokenStreamTests : public Test {};

TEST_F(TokenStreamTests,
BoolLiteral) {
	auto l = Literal(true);
	ASSERT_TRUE(l.isBool());
	ASSERT_TRUE(l.getBool());
	ASSERT_EQ(l.getText(), "true");
	l = Literal(false);
	ASSERT_TRUE(l.isBool());
	ASSERT_FALSE(l.getBool());
	ASSERT_EQ(l.getText(), "false");
	l = Literal(true, "Yes");
	ASSERT_TRUE(l.isBool());
	ASSERT_TRUE(l.getBool());
	ASSERT_EQ(l.getText(), "Yes");
}

TEST_F(TokenStreamTests,
TokenStreamFindSimple) {
	TokenStream ts;

	auto found = ts.find(TokenType::META_KEY);
	ASSERT_EQ(found, ts.end());
	TokenIt key = ts.emplace_back(TokenType::META_KEY, "author");
	found = ts.find(TokenType::META_KEY);
	ASSERT_EQ(found, key);
}

TEST_F(TokenStreamTests,
TokenStreamFind) {
	TokenStream ts;
	TokenIt c1 = ts.emplace_back(TokenType::COMMENT, "/*c1*/");
	ts.emplace_back(TokenType::COMMENT, "/*c2*/");
	TokenIt k1 = ts.emplace_back(TokenType::META_KEY, "k1");
	TokenIt c3 = ts.emplace_back(TokenType::COMMENT, "/*c3*/");
	ts.emplace_back(TokenType::META_KEY, "k2");
	TokenIt c4 = ts.emplace_back(TokenType::COMMENT, "/*c4*/");
	ts.emplace_back(TokenType::COMMENT, "/*c5*/");
	ts.emplace_back(TokenType::META_KEY, "k3");

	ASSERT_EQ(ts.find(COMMENT), c1);
	ASSERT_EQ(ts.find(COMMENT, k1), c3);
	ASSERT_EQ(ts.find(COMMENT, c3), c3);
	ASSERT_EQ(ts.find(META_VALUE),			ts.end());
	ASSERT_EQ(ts.find(META_VALUE, c1),	  ts.end());
	ASSERT_EQ(ts.find(META_VALUE, c1, c4), c4		);
}

TEST_F(TokenStreamTests,
TokenStreamFindBackwardsSimple) {
	TokenStream ts;

	auto found = ts.findBackwards(TokenType::META_KEY);
	ASSERT_EQ(found, ts.begin());
	TokenIt key = ts.emplace_back(TokenType::META_KEY, "author");
	found = ts.findBackwards(TokenType::META_KEY);
	ASSERT_EQ(found, key);
}

TEST_F(TokenStreamTests,
TokenStreamFindBackwards) {
	TokenStream ts;
	TokenIt c1 = ts.emplace_back(TokenType::COMMENT, "/*c1*/");
	TokenIt c2 = ts.emplace_back(TokenType::COMMENT, "/*c2*/");
	TokenIt k1 = ts.emplace_back(TokenType::META_KEY, "k1");
	TokenIt c3 = ts.emplace_back(TokenType::COMMENT, "/*c3*/");
	TokenIt k2 = ts.emplace_back(TokenType::META_KEY, "k2");
	TokenIt c4 = ts.emplace_back(TokenType::COMMENT, "/*c4*/");
	TokenIt c5 = ts.emplace_back(TokenType::COMMENT, "/*c5*/");
	ts.emplace_back(TokenType::META_KEY, "k3");

	ASSERT_EQ(ts.findBackwards(COMMENT), c5);
	ASSERT_EQ(ts.findBackwards(COMMENT, k1), c2);
	ASSERT_EQ(ts.findBackwards(COMMENT, c3), c2);
	ASSERT_EQ(ts.findBackwards(META_VALUE),			ts.end());
	ASSERT_EQ(ts.findBackwards(META_VALUE, c1),	  c1);
	ASSERT_EQ(ts.findBackwards(META_KEY, c1, c4), k2		);
}

TEST_F(TokenStreamTests,
TokenStreamEmplaceBack) {
	TokenStream ts;

	TokenIt key = ts.emplace_back(TokenType::META_KEY, "author");
	ts.emplace_back(TokenType::EQ, "=");
	TokenIt value = ts.emplace_back(TokenType::META_VALUE, "Mr. Avastian");
	ASSERT_EQ(key->getPureText(), "author");
	ASSERT_EQ(value->getPureText(), "Mr. Avastian");
}

TEST_F(TokenStreamTests,
TokenStreamEmplace) {
	TokenStream ts;

	auto key = ts.emplace_back(TokenType::META_KEY, "author");
	ts.emplace_back(TokenType::EQ, "=");
	auto value = ts.emplace_back(TokenType::META_VALUE, "Mr. Avastian");
	auto comment = ts.emplace(value, TokenType::COMMENT, "/*comment about the author*/");
	ASSERT_EQ(key->getPureText(), "author");
	ASSERT_EQ(value->getPureText(), "Mr. Avastian");
	ASSERT_EQ(comment->getPureText(), "/*comment about the author*/");
}

TEST_F(TokenStreamTests,
TokenStreamPushBack) {
	TokenStream ts;
	Token t(TokenType::RULE_NAME, Literal("rule_name"));
	TokenIt name = ts.push_back(t);
	ASSERT_EQ(name->getPureText(), "rule_name");
}

TEST_F(TokenStreamTests,
TokenStreamErase) {
	TokenStream ts;
	TokenIt key = ts.emplace_back(TokenType::META_KEY, "author");
	ts.emplace_back(TokenType::META_KEY, "=");
	TokenIt comment1 = ts.emplace_back(TokenType::COMMENT, "/*comment before author name*/");
	TokenIt value = ts.emplace_back(TokenType::META_KEY, "author_name");
	TokenIt comment2 = ts.emplace_back(TokenType::COMMENT, "/*comment after author name*/");

	TokenIt behindErased = ts.erase(comment1);
	ASSERT_EQ(behindErased, value);
	behindErased = ts.erase(key, comment2);
	ASSERT_EQ(behindErased, comment2);
	ASSERT_EQ(comment2->getPureText(), "/*comment after author name*/");

	ts.clear();
	ASSERT_EQ(ts.size(), 0);
	ASSERT_TRUE(ts.empty());
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithDifferentTokenStreams) {
	TokenStream ts_a;
	[[maybe_unused]] auto a1 = ts_a.emplace_back(TokenType::META_KEY, "author");
	[[maybe_unused]] auto a2 = ts_a.emplace_back(TokenType::EQ, "=");
	[[maybe_unused]] auto a3 = ts_a.emplace_back(TokenType::COMMENT, "/*comment before author name*/");
	[[maybe_unused]] auto a4 = ts_a.emplace_back(TokenType::META_KEY, "author_name");
	[[maybe_unused]] auto a5 = ts_a.emplace_back(TokenType::COMMENT, "/*comment after author name*/");
	TokenStream ts_b;
	[[maybe_unused]] auto b1 = ts_b.emplace_back(TokenType::META_KEY, "AUTHOR");
	[[maybe_unused]] auto b2 = ts_b.emplace_back(TokenType::EQ, "=");
	[[maybe_unused]] auto b3 = ts_b.emplace_back(TokenType::COMMENT, "/*COMMENT BEFORE AUTHOR NAME*/");
	[[maybe_unused]] auto b4 = ts_b.emplace_back(TokenType::META_KEY, "AUTHOR_NAME");
	[[maybe_unused]] auto b5 = ts_b.emplace_back(TokenType::COMMENT, "/*COMMENT AFTER AUTHOR NAME*/");

	ts_a.swapTokens(a3, a5, &ts_b, b3, b5);
	EXPECT_EQ(R"(author = /*COMMENT BEFORE AUTHOR NAME*/
AUTHOR_NAME /*comment after author name*/
)", ts_a.getText());
	EXPECT_EQ(R"(AUTHOR = /*comment before author name*/
author_name /*COMMENT AFTER AUTHOR NAME*/
)", ts_b.getText());

	ts_a.clear();
	ASSERT_EQ(ts_a.size(), 0);
	ASSERT_TRUE(ts_a.empty());
	ts_b.clear();
	ASSERT_EQ(ts_b.size(), 0);
	ASSERT_TRUE(ts_b.empty());
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamInclusion1) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::META_KEY, "author");
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::EQ, "=");
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::COMMENT, "/*comment before author name*/");
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::META_KEY, "author_name");
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::COMMENT, "/*comment after author name*/");

	ts.swapTokens(a3, a5, &ts, a4, a5);
	EXPECT_EQ(R"(author =
author_name /*comment after author name*/
)", ts.getText());

	ts.clear();
	ASSERT_EQ(ts.size(), 0);
	ASSERT_TRUE(ts.empty());
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamInclusion2) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::INTEGER, 1.0, std::make_optional(std::string{"1"}));
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::INTEGER, 2.0, std::make_optional(std::string{"2"}));
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::INTEGER, 3.0, std::make_optional(std::string{"3"}));
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::INTEGER, 4.0, std::make_optional(std::string{"4"}));
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::INTEGER, 5.0, std::make_optional(std::string{"5"}));
	[[maybe_unused]] auto a6 = ts.emplace_back(TokenType::INTEGER, 6.0, std::make_optional(std::string{"6"}));

	ts.swapTokens(a2, a5, &ts, a3, a4);
	EXPECT_EQ(R"(1 3 5 6
)", ts.getText());

	ts.clear();
	ASSERT_EQ(ts.size(), 0);
	ASSERT_TRUE(ts.empty());
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamInclusionTouching1) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::CONDITION, "condition");
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::COLON_BEFORE_NEWLINE, ":");
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::BOOL_TRUE, "true");
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::AND, "and");
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::BOOL_FALSE, "false");

	ts.swapTokens(a3, ts.end(), &ts, a5, ts.end());
	EXPECT_EQ(R"(condition:
false
)", ts.getText());

	ts.clear();
	ASSERT_EQ(ts.size(), 0);
	ASSERT_TRUE(ts.empty());
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamInclusionTouching2) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::META_KEY, "author");
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::EQ, "=");
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::COMMENT, "/*comment before author name*/");
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::META_KEY, "author_name");
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::COMMENT, "/*comment after author name*/");

	ts.swapTokens(a3, a5, &ts, a3, a4);
	EXPECT_EQ(R"(author = /*comment before author name*/ /*comment after author name*/
)", ts.getText());

	ts.clear();
	ASSERT_EQ(ts.size(), 0);
	ASSERT_TRUE(ts.empty());
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamInclusionInvalid) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::INTEGER, 1.0, std::make_optional(std::string{"1"}));
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::INTEGER, 2.0, std::make_optional(std::string{"2"}));
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::INTEGER, 3.0, std::make_optional(std::string{"3"}));
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::INTEGER, 4.0, std::make_optional(std::string{"4"}));
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::INTEGER, 5.0, std::make_optional(std::string{"5"}));
	[[maybe_unused]] auto a6 = ts.emplace_back(TokenType::INTEGER, 6.0, std::make_optional(std::string{"6"}));

  try
	{
		ts.swapTokens(a3, a4, &ts, a2, a5);
		FAIL() << "TokenStream did not throw an exception.";
	}
	catch (const YaramodError& err)
	{
		EXPECT_EQ(R"(1 2 3 4 5 6
)", ts.getText());
		EXPECT_EQ("Error: Cannot swapTokens when ['3','4') is under ['2','5').", err.getErrorMessage());
	}
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamInclusionInvalid2) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::INTEGER, 1.0, std::make_optional(std::string{"1"}));
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::INTEGER, 2.0, std::make_optional(std::string{"2"}));
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::INTEGER, 3.0, std::make_optional(std::string{"3"}));
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::INTEGER, 4.0, std::make_optional(std::string{"4"}));
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::INTEGER, 5.0, std::make_optional(std::string{"5"}));
	[[maybe_unused]] auto a6 = ts.emplace_back(TokenType::INTEGER, 6.0, std::make_optional(std::string{"6"}));

  try
	{
		ts.swapTokens(a1, a3, &ts, a2, a5);
		FAIL() << "TokenStream did not throw an exception.";
	}
	catch (const YaramodError& err)
	{
		EXPECT_EQ(R"(1 2 3 4 5 6
)", ts.getText());
		EXPECT_EQ("Error: Cannot swapTokens when ['1','3') and ['2','5') intersect in proper subset of each of them.", err.getErrorMessage());
	}
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamDistinct) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::META_KEY, "author");
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::EQ, "=");
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::COMMENT, "/*comment before author name*/");
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::META_KEY, "author_name");
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::COMMENT, "/*comment after author name*/");

	ts.swapTokens(a3, a4, &ts, a5, ts.end());
	EXPECT_EQ(R"(author = /*comment after author name*/
author_name /*comment before author name*/
)", ts.getText());

	ts.clear();
	ASSERT_EQ(ts.size(), 0);
	ASSERT_TRUE(ts.empty());
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamDistinctTouching1) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::INTEGER, 1.0, std::make_optional(std::string{"1"}));
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::INTEGER, 2.0, std::make_optional(std::string{"2"}));
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::INTEGER, 3.0, std::make_optional(std::string{"3"}));
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::INTEGER, 4.0, std::make_optional(std::string{"4"}));
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::INTEGER, 5.0, std::make_optional(std::string{"5"}));
	[[maybe_unused]] auto a6 = ts.emplace_back(TokenType::INTEGER, 6.0, std::make_optional(std::string{"6"}));

	ts.swapTokens(a3, a4, &ts, a4, a5);
	EXPECT_EQ(R"(1 2 4 3 5 6
)", ts.getText());

	ts.clear();
	ASSERT_EQ(ts.size(), 0);
	ASSERT_TRUE(ts.empty());
}

TEST_F(TokenStreamTests,
TokenStreamSwapTokensWithinTheSameTokenStreamDistinctTouching2) {
	TokenStream ts;
	[[maybe_unused]] auto a1 = ts.emplace_back(TokenType::INTEGER, 1.0, std::make_optional(std::string{"1"}));
	[[maybe_unused]] auto a2 = ts.emplace_back(TokenType::INTEGER, 2.0, std::make_optional(std::string{"2"}));
	[[maybe_unused]] auto a3 = ts.emplace_back(TokenType::INTEGER, 3.0, std::make_optional(std::string{"3"}));
	[[maybe_unused]] auto a4 = ts.emplace_back(TokenType::INTEGER, 4.0, std::make_optional(std::string{"4"}));
	[[maybe_unused]] auto a5 = ts.emplace_back(TokenType::INTEGER, 5.0, std::make_optional(std::string{"5"}));
	[[maybe_unused]] auto a6 = ts.emplace_back(TokenType::INTEGER, 6.0, std::make_optional(std::string{"6"}));

	ts.swapTokens(a4, a5, &ts, a3, a4);
	EXPECT_EQ(R"(1 2 4 3 5 6
)", ts.getText());

	ts.clear();
	ASSERT_EQ(ts.size(), 0);
	ASSERT_TRUE(ts.empty());
}

}
}
