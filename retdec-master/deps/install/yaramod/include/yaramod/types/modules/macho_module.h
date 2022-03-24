/**
 * @file src/types/modules/macho_module.h
 * @brief Declaration of MachoModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c macho module.
 */
class MachoModule : public Module
{
public:
	/// @name Constructors
	/// @{
	MachoModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~MachoModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
