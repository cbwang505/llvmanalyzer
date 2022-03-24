/**
 * @file src/types/modules/elf_module.h
 * @brief Declaration of ElfModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c elf module.
 */
class ElfModule : public Module
{
public:
	/// @name Constructors
	/// @{
	ElfModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~ElfModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
