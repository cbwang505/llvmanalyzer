/**
 * @file src/types/location.h
 * @brief Declaration and Implementation of class Location.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>

namespace yaramod {

class Location
{
public:
	Location() : Location(std::string{}) {}
	Location(const std::string& filePath) : Location(filePath, 1, 0) {}
	Location(const std::string& filePath, std::size_t line, std::size_t column)
		: _filePath(filePath), _begin(line, column), _end(line, column) {}
	Location(const Location&) = default;
	Location(Location&&) noexcept = default;

	Location& operator=(const Location&) = default;
	Location& operator=(Location&&) noexcept = default;

	/// @name Modifying methods
	/// @{
	void addLine(std::size_t count = 1)
	{
		std::swap(_begin, _end);
		_end.first = _begin.first + count; // line
		_end.second = 0; // column
	}

	void addColumn(std::size_t count)
	{
		_begin.first = _end.first;
		_begin.second = _end.second;
		_end.second += count;
	}

	void reset()
	{
		_begin = {1, 0};
		_end = {1, 0};
	}
	/// @}

	/// @name Getters
	/// @{
	bool isUnnamed() const { return _filePath == "[stream]"; }
	const std::string& getFilePath() const { return _filePath; }
	std::pair<std::size_t, std::size_t> begin() const { return {_begin.first, _begin.second + 1}; }
	const std::pair<std::size_t, std::size_t>& end() const { return _end; }
	/// @}

	friend std::ostream& operator<<(std::ostream& os, const Location& location)
	{
		if (!location.isUnnamed())
			os << location.getFilePath() << ':';
		os << location.begin().first << '.' << location.begin().second;
		if (location.begin().second < location.end().second)
			os << '-' << location.end().second;
		return os;
	}

private:
	std::string _filePath;
	std::pair<std::size_t, std::size_t> _begin; // (line, column)
	std::pair<std::size_t, std::size_t> _end; // (line, column)
};

} //namespace yaramod
