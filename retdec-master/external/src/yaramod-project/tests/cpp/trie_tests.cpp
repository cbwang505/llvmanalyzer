/**
* @file tests/trie_tests.cpp
* @brief Tests for the trie.
* @copyright AVG Technologies s.r.o, All Rights Reserved
*/

#include <gtest/gtest.h>

#include "yaramod/utils/trie.h"

using namespace ::testing;
using namespace std::string_literals;

namespace yaramod {
namespace tests {

class TrieTests : public Test {};

TEST_F(TrieTests,
InsertAndFindWorks) {
	Trie<std::string> trie;

	const std::string expected = "value";
	std::string inserted;

	ASSERT_TRUE(trie.insert("abc", expected));
	EXPECT_TRUE(trie.find("abc", inserted));
	EXPECT_EQ(expected, inserted);
}

TEST_F(TrieTests,
InsertFailsOnDuplicateInsertWorks) {
	Trie<std::string> trie;

	ASSERT_TRUE(trie.insert("abc", "value"s));
	EXPECT_FALSE(trie.insert("abc", "value2"s));
}

TEST_F(TrieTests,
FindFailsIfKeyNotPresentWorks) {
	Trie<std::string> trie;

	std::string dummy;
	EXPECT_FALSE(trie.find("abc", dummy));
}

TEST_F(TrieTests,
IsPrefixWorks) {
	Trie<std::string> trie;

	ASSERT_TRUE(trie.insert("abc", "value1"s));
	ASSERT_TRUE(trie.insert("abd", "value2"s));

	EXPECT_TRUE(trie.isPrefix("ab"));
	EXPECT_TRUE(trie.isPrefix("abc"));
	EXPECT_TRUE(trie.isPrefix("abd"));
}

TEST_F(TrieTests,
IsPrefixFailsIfKeyNotPresentWorks) {
	Trie<std::string> trie;

	ASSERT_TRUE(trie.insert("abc", "value1"s));
	ASSERT_TRUE(trie.insert("abd", "value2"s));

	EXPECT_FALSE(trie.isPrefix("ad"));
	EXPECT_FALSE(trie.isPrefix("cd"));
}

TEST_F(TrieTests,
GetAllValuesWorks) {
	Trie<std::string> trie;

	ASSERT_TRUE(trie.insert("ab",  "value1"s));
	ASSERT_TRUE(trie.insert("abc", "value2"s));
	ASSERT_TRUE(trie.insert("abd", "value3"s));
	ASSERT_TRUE(trie.insert("bcd", "value4"s));

	const std::vector<std::string> expected = { "value1", "value2", "value3", "value4" };
	EXPECT_EQ(expected, trie.getAllValues());
}

TEST_F(TrieTests,
GetAllValuesPreservesInsertionOrderWorks) {
	Trie<std::string> trie;

	ASSERT_TRUE(trie.insert("bcd", "value1"s));
	ASSERT_TRUE(trie.insert("abd", "value2"s));
	ASSERT_TRUE(trie.insert("abc", "value3"s));
	ASSERT_TRUE(trie.insert("ab",  "value4"s));

	const std::vector<std::string> expected = { "value1", "value2", "value3", "value4" };
	EXPECT_EQ(expected, trie.getAllValues());
}

TEST_F(TrieTests,
EmptyWorks) {
	Trie<std::string> trie;

	EXPECT_TRUE(trie.empty());
	EXPECT_TRUE(trie.insert("ab", "value"s));
	EXPECT_FALSE(trie.empty());
}

TEST_F(TrieTests,
ClearWorks) {
	Trie<std::string> trie;

	EXPECT_TRUE(trie.empty());
	EXPECT_TRUE(trie.insert("ab", "value"s));
	ASSERT_FALSE(trie.empty());

	trie.clear();
	EXPECT_TRUE(trie.empty());
}

TEST_F(TrieTests,
RemoveWorks) {
	Trie<std::string> trie;

	EXPECT_TRUE(trie.empty());
	EXPECT_TRUE(trie.insert("a", "value1"s));
	EXPECT_TRUE(trie.insert("abc", "value2"s));
	EXPECT_TRUE(trie.insert("abd", "value3"s));
	ASSERT_FALSE(trie.empty());

	trie.remove("abc");
	const std::vector<std::string> expected = { "value1", "value3" };
	EXPECT_EQ(expected, trie.getAllValues());
	EXPECT_FALSE(trie.isPrefix("abc"));
}

TEST_F(TrieTests,
RemoveWithMultiDeleteWorks) {
	Trie<std::string> trie;

	EXPECT_TRUE(trie.empty());
	EXPECT_TRUE(trie.insert("a", "value1"s));
	EXPECT_TRUE(trie.insert("abc", "value2"s));
	ASSERT_FALSE(trie.empty());

	trie.remove("abc");
	const std::vector<std::string> expected = { "value1" };
	EXPECT_EQ(expected, trie.getAllValues());
	EXPECT_FALSE(trie.isPrefix("abc"));
}

TEST_F(TrieTests,
RemoveWithParentRemovedWorks) {
	Trie<std::string> trie;

	EXPECT_TRUE(trie.empty());
	EXPECT_TRUE(trie.insert("a", "value1"s));
	EXPECT_TRUE(trie.insert("abc", "value2"s));
	ASSERT_FALSE(trie.empty());

	trie.remove("a");
	const std::vector<std::string> expected = { "value2" };
	EXPECT_EQ(expected, trie.getAllValues());
	EXPECT_TRUE(trie.isPrefix("a"));
	EXPECT_TRUE(trie.isPrefix("abc"));
}

TEST_F(TrieTests,
RemoveClearsWorks) {
	Trie<std::string> trie;

	EXPECT_TRUE(trie.empty());
	EXPECT_TRUE(trie.insert("abc", "value1"s));
	ASSERT_FALSE(trie.empty());

	trie.remove("abc");
	EXPECT_FALSE(trie.isPrefix("abc"));
	EXPECT_TRUE(trie.empty());
}

}
}
