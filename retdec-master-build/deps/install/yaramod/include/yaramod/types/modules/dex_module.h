/**
 * @file src/types/modules/dex_module.h
 * @brief Declaration of DexModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c dex module.
 */
class DexModule : public Module
{
public:
	/// @name Constructors
	/// @{
	DexModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~DexModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
