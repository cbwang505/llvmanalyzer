/**
 * @file src/types/modules/androguard_module.h
 * @brief Declaration of AndroguardModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c androguard module.
 */
class AndroguardModule : public Module
{
public:
	/// @name Constructors
	/// @{
	AndroguardModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~AndroguardModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
