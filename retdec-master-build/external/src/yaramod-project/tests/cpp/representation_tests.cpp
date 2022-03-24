/**
* @file tests/representation_tests.cpp
* @brief Tests for the YARA representation.
* @copyright (c) 2019 Avast Software, licensed under the MIT license
*/

#include <gtest/gtest.h>
#include <iostream>

#include "yaramod/types/meta.h"
#include "yaramod/types/regexp.h"
#include "yaramod/types/token_stream.h"

using namespace ::testing;

namespace yaramod {
namespace tests {

class RepresentationTests : public Test {};

TEST_F(RepresentationTests,
MetaConstruction) {
   TokenStream ts;
   TokenIt key = ts.emplace_back(TokenType::META_KEY, "author");
   TokenIt value = ts.emplace_back(TokenType::META_VALUE, "Mr. Avastian");
   Meta meta{key, value};

   ASSERT_EQ(meta.getKey(), "author");
   ASSERT_TRUE(meta.getValue().isString());
   ASSERT_EQ(meta.getValue().getString(), "Mr. Avastian");

   EXPECT_EQ(meta.getText(), "author = \"Mr. Avastian\"");
}

TEST_F(RepresentationTests,
MetaSetters) {
   TokenStream ts;
   TokenIt key = ts.emplace_back(TokenType::META_KEY, "author");
   TokenIt value = ts.emplace_back(TokenType::META_VALUE, "Mr. Avastian");
   Meta meta{key, value};
   meta.setKey("strain");
   meta.setValue(Literal("cryptic"));

   ASSERT_EQ(meta.getKey(), "strain");
   ASSERT_TRUE(meta.getValue().isString());
   ASSERT_EQ(meta.getValue().getString(), "cryptic");

   EXPECT_EQ(meta.getText(), "strain = \"cryptic\"");
}

TEST_F(RepresentationTests,
MetaCopyIsJustReference) {
   TokenStream ts;
   TokenIt key = ts.emplace_back(TokenType::META_KEY, "author");
   TokenIt value = ts.emplace_back(TokenType::META_VALUE, "Mr. Avastian");
   Meta meta{key, value};

   auto copy = meta;

   ASSERT_EQ(copy.getKey(), "author");
   ASSERT_TRUE(copy.getValue().isString());
   ASSERT_EQ(copy.getValue().getString(), "Mr. Avastian");

   EXPECT_EQ(copy.getText(), "author = \"Mr. Avastian\"");

   copy.setKey("strain");
   copy.setValue(Literal("cryptic"));

   ASSERT_EQ(meta.getKey(), "strain");
   ASSERT_TRUE(meta.getValue().isString());
   ASSERT_EQ(meta.getValue().getString(), "cryptic");

   EXPECT_EQ(meta.getText(), "strain = \"cryptic\"");
}

TEST_F(RepresentationTests,
RegexpConstruction) {
   auto ts = std::make_shared<TokenStream>();
   auto unit1 = std::make_shared<RegexpText>("abc");
   Regexp regexp1{ts, std::move(unit1)};
   EXPECT_EQ(regexp1.getText(), "/abc/");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"/", "a", "b", "c", "/"}));

   regexp1.setIdentifier("$s01");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"$s01", "=", "/", "a", "b", "c", "/"}));
   regexp1.setIdentifier("$s1");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"$s1", "=", "/", "a", "b", "c", "/"}));

   auto unit2 = std::make_shared<RegexpAnyChar>();
   Regexp regexp2{ts, std::move(unit2)};
   EXPECT_EQ(regexp2.getText(), "/./");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"$s1", "=", "/", "a", "b", "c", "/", "/", ".", "/" }));
   regexp2.setIdentifier("$s2");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"$s1", "=", "/", "a", "b", "c", "/", "$s2", "=", "/", ".", "/" }));

   auto unit3 = std::make_shared<RegexpEndOfLine>();
   Regexp regexp3{ts, std::move(unit3), "$s3"};
   EXPECT_EQ(regexp3.getText(), "/$/");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"$s1", "=", "/", "a", "b", "c", "/", "$s2", "=", "/", ".", "/" , "$s3", "=", "/", "$", "/" }));
}

TEST_F(RepresentationTests,
RegexpSetters) {
   auto ts = std::make_shared<TokenStream>();

   auto unit2 = std::make_shared<RegexpAnyChar>();
   Regexp regexp2{ts, std::move(unit2)};
   EXPECT_EQ(regexp2.getText(), "/./");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({ "/", ".", "/" }));
   regexp2.setIdentifier("$s2");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({ "$s2", "=", "/", ".", "/" }));

   auto unit3 = std::make_shared<RegexpEndOfLine>();
   Regexp regexp3{ts, std::move(unit3), "$s3"};
   EXPECT_EQ(regexp3.getText(), "/$/");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({ "$s2", "=", "/", ".", "/", "$s3", "=", "/", "$", "/" }));

   regexp2.setSuffixModifiers("modifiers 2");
   regexp3.setSuffixModifiers("modifiers 3");
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({ "$s2", "=", "/", ".", "/", "modifiers 2", "$s3", "=", "/", "$", "/", "modifiers 3" }));


}

TEST_F(RepresentationTests,
RegexpConcat) {
   std::vector<std::shared_ptr<RegexpUnit>> v;

   v.push_back(std::make_shared<RegexpWordChar>());
   v.push_back(std::make_shared<RegexpNonWordChar>());
   v.push_back(std::make_shared<RegexpSpace>());
   v.push_back(std::make_shared<RegexpNonSpace>());
   v.push_back(std::make_shared<RegexpDigit>());
   v.push_back(std::make_shared<RegexpNonDigit>());
   v.push_back(std::make_shared<RegexpWordBoundary>());
   v.push_back(std::make_shared<RegexpNonWordBoundary>());
   v.push_back(std::make_shared<RegexpStartOfLine>());

   RegexpConcat concat(std::move(v));
   EXPECT_EQ(concat.getText(), "\\w\\W\\s\\S\\d\\D\\b\\B^");

   auto ts = concat.getTokenStream();
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({ "\\w", "\\W", "\\s", "\\S", "\\d", "\\D", "\\b", "\\B", "^" }));
}

TEST_F(RepresentationTests,
RegexpGroup) {
   auto unit = std::make_shared<RegexpStartOfLine>();
   RegexpGroup group{std::move(unit)};
   EXPECT_EQ(group.getText(), "(^)");

   auto ts = group.getTokenStream();
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({ "(", "^", ")" }));
}

TEST_F(RepresentationTests,
RegexpOr) {
   auto unit1 = std::make_shared<RegexpText>("abc");
   auto unit2 = std::make_shared<RegexpAnyChar>();
   RegexpOr regexp_or{std::move(unit1), std::move(unit2)};
   EXPECT_EQ(regexp_or.getText(), "abc|.");

   auto ts = regexp_or.getTokenStream();
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({ "a", "b", "c", "|", "." }));
}

TEST_F(RepresentationTests,
RegexpRange) {
   auto unit = std::make_shared<RegexpText>("abc");
   std::optional<std::uint64_t> left(100);
   std::optional<std::uint64_t> right(200);
   auto pair = std::make_pair(left, right);
   bool greedy = false;

   RegexpRange range(std::move(unit), std::move(pair) , greedy);
   EXPECT_EQ(range.getText(), "abc{100,200}?");

   auto ts = range.getTokenStream();
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"a", "b", "c", "{", "100", ",", "200", "}", "?"}));
}

TEST_F(RepresentationTests,
RegexpOptional) {
   auto unit = std::make_shared<RegexpText>("abc");
   bool greedy = true;
   RegexpOptional regexp_optional{std::move(unit), greedy};
   EXPECT_EQ(regexp_optional.getText(), "abc?");

   auto ts = regexp_optional.getTokenStream();
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"a", "b", "c", "?", ""}));
}

TEST_F(RepresentationTests,
RegexpPositiveIteration) {
   auto unit = std::make_shared<RegexpText>("abc");
   bool greedy = false;
   RegexpPositiveIteration regexp_iter{std::move(unit), greedy};
   EXPECT_EQ(regexp_iter.getText(), "abc+?");

   auto ts = regexp_iter.getTokenStream();
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"a", "b", "c", "+", "?"}));
}

TEST_F(RepresentationTests,
RegexpIteration) {
   auto unit = std::make_shared<RegexpText>("abc");
   bool greedy = false;
   RegexpIteration regexp_iter{std::move(unit), greedy};
   EXPECT_EQ(regexp_iter.getText(), "abc*?");

   auto ts = regexp_iter.getTokenStream();
   EXPECT_EQ(ts->getTokensAsText(), std::vector<std::string>({"a", "b", "c", "*", "?"}));
}


}
}
