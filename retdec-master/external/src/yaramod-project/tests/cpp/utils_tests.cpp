/**
* @file tests/utils_tests.cpp
* @brief Tests for the YARA utility functions.
* @copyright AVG Technologies s.r.o, All Rights Reserved
*/

#include <gtest/gtest.h>

#include "yaramod/utils/utils.h"

using namespace ::testing;

namespace yaramod {
namespace tests {

class UtilsTests : public Test {};

TEST_F(UtilsTests,
IsValidIdentifierWorks) {
	EXPECT_TRUE(isValidIdentifier("xyz"));
	EXPECT_TRUE(isValidIdentifier("abc1"));
	EXPECT_TRUE(isValidIdentifier("_xyz"));
	EXPECT_TRUE(isValidIdentifier("_1"));
	EXPECT_FALSE(isValidIdentifier(""));
	EXPECT_FALSE(isValidIdentifier("123"));
}

TEST_F(UtilsTests,
EscapeStringWorks) {
	EXPECT_EQ(R"(abc)", escapeString("abc"));
	EXPECT_EQ(R"(a\nc)", escapeString("a\nc"));
	EXPECT_EQ(R"(a\nc)", escapeString(R"(a
c)"));
	EXPECT_EQ(R"(a@c)", escapeString("a@c"));
	EXPECT_EQ(R"(a@c)", escapeString("a\x40""c"));
	EXPECT_EQ(R"(\n\t\\\"\x01)", escapeString("\n\t\\\"\x01"));
}

TEST_F(UtilsTests,
UnescapeStringWorks) {
	EXPECT_EQ("abc", unescapeString(R"(abc)"));
	EXPECT_EQ("a\nb", unescapeString(R"(a\nb)"));
	EXPECT_EQ("a\tb", unescapeString(R"(a\tb)"));
	EXPECT_EQ("a\"b", unescapeString(R"(a\"b)"));
	EXPECT_EQ(3, unescapeString(R"(a\"b)").length());
	EXPECT_EQ("a\\b", unescapeString(R"(a\\b)"));
	EXPECT_EQ(3, unescapeString(R"(a\\b)").length());
	EXPECT_EQ("a*b", unescapeString(R"(a\x2Ab)"));
	EXPECT_EQ("a@b", unescapeString(R"(a\x40b)"));
	EXPECT_EQ("a'b", unescapeString(R"(a\x27b)"));
	EXPECT_EQ("\x17", unescapeString(R"(\x17)"));
	EXPECT_EQ("a\x17", unescapeString(R"(a\x17)"));
	EXPECT_EQ(R"(a\x07\x07b)", escapeString(unescapeString(R"(a\x07\x07b)")));
	EXPECT_EQ("a\x07\x07b", unescapeString(escapeString("a\x07\x07b")));
	EXPECT_EQ("Here are \"\t\n\\\x01\xff", unescapeString(R"(Here are \"\t\n\\\x01\xff)"));
	EXPECT_EQ("\x17""b", unescapeString(R"(\x17b)"));
	EXPECT_EQ("a\x07\x07""b", unescapeString(R"(a\x07\x07b)"));
	EXPECT_EQ("\n\t\\\"\x01", unescapeString(R"(\n\t\\\"\x01)"));
}

}
}
