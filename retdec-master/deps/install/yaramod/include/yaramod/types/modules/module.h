/**
 * @file src/types/modules/module.h
 * @brief Declaration of class Module.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <memory>
#include <string>

#include "yaramod/types/symbols.h"

namespace yaramod {

/**
 * Class representing features of module.
 * Every module has to provide its features to state what must hold for it to be loaded.
 */
enum ImportFeatures
{
	Basic = 0x01,          // 0001 - such module is always loaded
	AvastOnly = 0x02,      // 0010 - such module is loaded when Avast specified
	VirusTotalOnly = 0x04, // 0100 - such module is loaded when VirusTotal specified
	Deprecated = 0x08,     // 1000 - such module is deprecated
	Avast = Basic | AvastOnly,           // 0011 - specification which will load all basic and Avast-specific modules
	VirusTotal = Basic | VirusTotalOnly, // 0101 - specification which will load all basic and VirusTotal-specific modules
	AllCurrent = Avast | VirusTotal,     // 0111 - specification which will load all currently used modules
	Everything = AllCurrent | Deprecated // 1111 - specification which will load everything - even old deprecated modules
};

/**
 * Abstract class representing importable module. Every module
 * has to provide its structure in virtual method @c initialize.
 */
class Module
{
public:
	/// @name Constructors
	/// @{
	Module(const std::string& name, ImportFeatures features);
	/// @}

	/// @name Destructor
	/// @{
	virtual ~Module();
	/// @}

	/// @name Pure virtual initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) = 0;
	/// @}

	/// @name Getter methods
	/// @{
	const std::string& getName() const;
	const std::shared_ptr<StructureSymbol>& getStructure() const;
	ImportFeatures getFeatures() const;
	/// @}

	/// @name Detection methods
	/// @{
	bool isInitialized() const;
	/// @}

protected:
	std::string _name; ///< Name of the module
	std::shared_ptr<StructureSymbol> _structure; ///< Structure of the module
	ImportFeatures _needed_features; ///< Specifies when this module can be loaded:
};

}
