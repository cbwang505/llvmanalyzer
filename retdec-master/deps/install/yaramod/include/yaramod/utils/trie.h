/**
 * @file src/utils/trie.h
 * @brief Declaration of class Trie.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace yaramod {

template <typename T>
class TrieNode
{
public:
	/// @name Constructors
	/// @{
	TrieNode() = default;
	TrieNode(const TrieNode& trie) = default;
	TrieNode(TrieNode&& trie) = default;
	/// @}

	/// @name Getters
	/// @{
	/**
	 * Returns the subnode for the given key if it exists.
	 *
	 * @param key Key
	 *
	 * @return Subnode if it exists, otherwise @c nullptr.
	 */
	TrieNode* getSubnode(char key)
	{
		auto itr = _subnodes.find(key);
		return itr != _subnodes.end() ? itr->second : nullptr;
	}

	/**
	 * Returns the subnode for the given key if it exists.
	 *
	 * @param key Key
	 *
	 * @return Subnode if it exists, otherwise @c nullptr.
	 */
	const TrieNode* getSubnode(char key) const
	{
		auto itr = _subnodes.find(key);
		return itr != _subnodes.end() ? itr->second : nullptr;
	}

	/**
	 * Returns the value out of the node.
	 *
	 * @return Value.
	 */
	const std::optional<T>& getValue() const
	{
		return _value;
	}

	/**
	 * Returns whether the node has any subnode.
	 *
	 * @return @c true if it has, otherwise @c false.
	 */
	const std::map<char, TrieNode*> getSubnodes() const
	{
		return _subnodes;
	}

	/**
	 * Returns whether the node contains any subnode with value.
	 *
	 * @return @c true if it contains, otherwise @c false.
	 */
	bool hasAnySubnodeWithValue() const
	{
		bool result = _value.has_value();
		for (auto&& keySubnode : _subnodes)
		{
			auto subnode = keySubnode.second;
			result = result || subnode->hasAnySubnodeWithValue();
		}

		return result;
	}
	/// @}

	/// @name Setters
	/// @{
	/**
	 * Adds new subnode for the given key.
	 * Insertion is ignored if there is already node with the specified key.
	 *
	 * @param key Key
	 * @param subnode Subnode to insert
	 */
	void addSubnode(char key, TrieNode* subnode)
	{
		_subnodes.emplace(key, subnode);
	}

	/**
	 * Removes subnode for the specific key.
	 *
	 * @param key Key.
	 */
	void removeSubnode(char key)
	{
		_subnodes.erase(key);
	}

	/**
	 * Sets the value in the node.
	 *
	 * @param value Value
	 */
	template <typename U>
	void setValue(U&& value)
	{
		_value = std::forward<U>(value);
	}

	/**
	 * Unsets the value in the node.
	 */
	void unsetValue()
	{
		_value.reset();
	}
	/// @}

private:
	std::map<char, TrieNode*> _subnodes; ///< Subtries.
	std::optional<T> _value; ///< Value stored in node
};

/**
 * Class representing trie data structure (prefix tree) over data of type @c T.
 * Every node in the trie can either contain some data or be empty. Nodes are indexed
 * with characters from the string called key.
 */
template <typename T>
class Trie
{
public:
	using ValueType = T; ///< Type of data stored in nodes
	using NodeType = TrieNode<ValueType>; ///< Type of node in trie

	/// @name Constructors
	/// @{
	Trie() = default;
	Trie(const Trie& trie) = default;
	Trie(Trie&& trie) = default;
	/// @}

	/// @name Setter operations
	/// @{
	/**
	 * Inserts new value into the trie. If the value for the given key already exists,
	 * insertion fails. If the value can be moved, it is moved.
	 *
	 * @param key Key
	 * @param value Value to insert
	 *
	 * @return @c true if insertion succeeded, otherwise @c false.
	 */
	template <typename U>
	bool insert(const std::string& key, U&& value)
	{
		// No nodes, insert the root node
		if (_nodes.empty())
			_nodes.push_back(std::make_unique<NodeType>());

		// Iteratively descend down the trie
		auto node = _nodes.front().get();
		for (auto itr = key.begin(), end = key.end(); itr != end; ++itr)
		{
			// If node for the character doesn't exist, insert the new node
			auto childNode = node->getSubnode(*itr);
			if (childNode == nullptr)
			{
				_nodes.push_back(std::make_unique<NodeType>());
				childNode = _nodes.back().get();

				node->addSubnode(*itr, childNode);
			}

			node = childNode;
		}

		// If the value is defined in the node, insertion fails
		const auto& nodeValue = node->getValue();
		if (nodeValue)
			return false;

		node->setValue(std::forward<U>(value));
		_values.push_back(&node->getValue().value()); // Helps preserving order in which we inserted values
		return true;
	}

	/**
	 * Removes the value out of trie for the specified key.
	 *
	 * @param key Key.
	 *
	 * @return @c true if the removed node contained some value, otherwise @c false.
	 */
	bool remove(const std::string& key)
	{
		// No nodes, nothing to remove
		if (_nodes.empty())
			return false;

		// Iteratively descend down the trie
		auto node = _nodes.front().get();
		std::vector<std::pair<char, NodeType*>> path;
		for (auto itr = key.begin(), end = key.end(); itr != end; ++itr)
		{
			// If node for the character doesn't exist, just end
			auto childNode = node->getSubnode(*itr);
			if (childNode == nullptr)
				return false;

			node = childNode;
			path.emplace_back(*itr, node);
		}

		// If it has value, delete it
		const auto& nodeValue = node->getValue();
		if (!nodeValue)
			return false;

		auto valueItr = std::find(_values.begin(), _values.end(), &nodeValue.value());
		_values.erase(valueItr);
		node->unsetValue();

		// Find the sequence of nodes which we should delete
		auto delStartItr = std::find_if(path.begin(), path.end(),
				[](const auto& keyNode) {
					return !keyNode.second->hasAnySubnodeWithValue();
				});

		// We should delete from the first node so we just clear the whole trie
		if (delStartItr == path.begin())
		{
			clear();
			return true;
		}

		// We shouldn't delete anything because the current node is still valid prefix
		if (delStartItr == path.end())
			return true;

		// Move one node back and remove reference to the first node which is deleted
		(delStartItr - 1)->second->removeSubnode(delStartItr->first);

		// Filter out the nodes which should be deleted
		std::unordered_set<NodeType*> nodesToDelete;
		std::transform(delStartItr, path.end(), std::inserter(nodesToDelete, nodesToDelete.end()),
				[](const auto& keyNode) {
					return keyNode.second;
				});

		// Remove them
		auto endItr = std::remove_if(_nodes.begin(), _nodes.end(),
				[&nodesToDelete](auto&& node) {
					return nodesToDelete.find(node.get()) != nodesToDelete.end();
				});
		_nodes.erase(endItr, _nodes.end());

		return true;
	}

	/**
	 * Clears the whole trie.
	 */
	void clear()
	{
		_nodes.clear();
		_values.clear();
	}
	/// @}

	/// @name Getter operations
	/// @{
	/**
	 * Returns value from the trie for the given key if it exists.
	 *
	 * @param key Key
	 * @param value Value to return
	 *
	 * @return @c true if search succeeded, otherwise @c false.
	 */
	bool find(const std::string& key, ValueType& value) const
	{
		// No nodes, nothing to search for
		if (_nodes.empty())
			return false;

		// Iteratively descend down the trie
		auto node = _nodes.front().get();
		for (auto itr = key.begin(), end = key.end(); itr != end; ++itr)
		{
			// If node for the character doesn't exist, just end
			auto childNode = node->getSubnode(*itr);
			if (childNode == nullptr)
				return false;

			node = childNode;
		}

		// If the found node has value then copy it
		const auto& nodeValue = node->getValue();
		if (!nodeValue)
			return false;

		value = nodeValue.value();
		return true;
	}

	/**
	 * Returns whether the given key is the valid prefix in the trie. Put in other words,
	 * whether there exist node down the trie from the node given key represents with valid value.
	 *
	 * @param key Key
	 *
	 * @return @c true if search succeeded, otherwise @c false.
	 */
	bool isPrefix(const std::string& key) const
	{
		// No nodes, nothing to search for
		if (_nodes.empty())
			return false;

		// Iteratively descend down the trie
		const NodeType* node = _nodes.front().get();
		for (auto itr = key.begin(), end = key.end(); itr != end; ++itr)
		{
			// If node for the character doesn't exist, just end
			auto childNode = node->getSubnode(*itr);
			if (childNode == nullptr)
				return false;

			node = childNode;
		}

		return true;
	}

	/**
	 * Returns all set values from the whole trie in the order of insertion.
	 *
	 * @return Vector of values.
	 */
	std::vector<ValueType> getAllValues() const
	{
		std::vector<ValueType> result;
		for (auto valuePtr : _values)
			result.push_back(*valuePtr);
		return result;
	}

	/**
	 * Returns whether trie is empty or not.
	 *
	 * @return @c true if is empty, @c false otherwise.
	 */
	bool empty() const
	{
		return _nodes.empty();
	}
	/// @}

private:
	std::vector<std::unique_ptr<NodeType>> _nodes; ///< Nodes.
	std::vector<const ValueType*> _values; ///< Values.
};

}
