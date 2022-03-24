/**
 * @file src/utils/filesystem.cpp
 * @brief Definition of filesystem functions.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <algorithm>
#include <cstring>

#ifdef YARAMOD_OS_WINDOWS
#include <windows.h>
#include <shlwapi.h>
#else
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#endif

#include "yaramod/utils/filesystem.h"
#include "yaramod/utils/utils.h"

namespace yaramod {

namespace detail {

std::string normalizePath(std::string path)
{
	std::replace(path.begin(), path.end(), '/', pathSeparator());
	return endsWith(path, pathSeparator()) ? path.substr(0, path.length() - 1) : path;
}

#ifdef YARAMOD_OS_WINDOWS
char pathSeparator()
{
	return '\\';
}

bool pathIsRelative(const std::string& path)
{
	return PathIsRelative(path.c_str());
}

std::string parentPath(const std::string& path)
{
	char parentPathStr[MAX_PATH] = { '\0' };
	strncpy_s(parentPathStr, path.c_str(), MAX_PATH - 1);
	PathRemoveFileSpec(parentPathStr);
	return parentPathStr;
}

std::string absolutePath(const std::string& path)
{
	char absolutePathStr[MAX_PATH] = { '\0' };
	if (GetFullPathName(path.c_str(), MAX_PATH, absolutePathStr, nullptr) == 0)
		return {};

	return absolutePathStr;
}
#elif YARAMOD_OS_LINUX
char pathSeparator()
{
	return '/';
}

bool pathIsRelative(const std::string& path)
{
	return !startsWith(path, pathSeparator());
}

std::string parentPath(const std::string& path)
{
	char* copyPathStr = new char[path.length() + 1];
	strcpy(copyPathStr, path.c_str());

	char* parentPathStr = dirname(copyPathStr);

	auto result = std::string{parentPathStr};
	delete[] copyPathStr;

	return result;
}

std::string absolutePath(const std::string& path)
{
	char absolutePathStr[PATH_MAX] = { '\0' };
	if (realpath(path.c_str(), absolutePathStr) == nullptr)
		return {};

	return absolutePathStr;
}
#endif

}

/**
 * Returns system specific separator used in paths.
 *
 * @return Separator.
 */
char pathSeparator()
{
	return detail::pathSeparator();
}

/**
 * Checks whether provided path is relative.
 *
 * @param path Path.
 *
 * @return @c true if relative, otherwise @c false.
 */
bool pathIsRelative(const std::string& path)
{
	return detail::pathIsRelative(detail::normalizePath(path));
}

/**
 * Returns the parent path of the provided path.
 *
 * @param path Path.
 *
 * @return Parent path.
 */
std::string parentPath(const std::string& path)
{
	return detail::parentPath(detail::normalizePath(path));
}

/**
 * Joins two paths together using the correct system specific separator.
 *
 * @param first First part of the path.
 * @param second Second part of the path.
 *
 * @return Joined path.
 */
std::string joinPaths(const std::string& first, const std::string& second)
{
	return detail::normalizePath(first) + pathSeparator() + detail::normalizePath(second);
}

/**
 * Returns absolute path of the provided path. Does nothing if path is already absolute.
 *
 * @param path Path to convert.
 *
 * @return Absolute path.
 */
std::string absolutePath(const std::string& path)
{
	return detail::absolutePath(path);
}

}
