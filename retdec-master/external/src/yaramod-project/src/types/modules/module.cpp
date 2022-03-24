/**
 * @file src/types/modules/module.cpp
 * @brief Implementation of class Module.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Constructor.
 *
 * @param name Name of the module
 */
Module::Module(const std::string& name, ImportFeatures needed_features)
	: _name(name)
	, _structure()
	, _needed_features(needed_features)
{
}

/**
 * Destructor.
 */
Module::~Module()
{
}

/**
 * Returns the name of the module.
 *
 * @return Module name.
 */
const std::string& Module::getName() const
{
	return _name;
}

/**
 * Returns the structure symbol of the module.
 *
 * @return Module structure symbol.
 */
const std::shared_ptr<StructureSymbol>& Module::getStructure() const
{
	return _structure;
}

/**
 * Returns the needed features of the module.
 *
 * @return Module Import Features.
 */
ImportFeatures Module::getFeatures() const
{
	return _needed_features;
}

/**
 * Returns whether the module is already initialized.
 *
 * @return @c true if initialized, otherwise @c false.
 */
bool Module::isInitialized() const
{
	return _structure != nullptr;
}

}
