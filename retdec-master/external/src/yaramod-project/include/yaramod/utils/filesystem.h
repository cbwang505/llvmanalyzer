/**
 * @file src/utils/filesystem.h
 * @brief Declaration of filesystem functions.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <string>

namespace yaramod {

char pathSeparator();
bool pathIsRelative(const std::string& path);
std::string parentPath(const std::string& path);
std::string joinPaths(const std::string& first, const std::string& second);
std::string absolutePath(const std::string& path);

}
