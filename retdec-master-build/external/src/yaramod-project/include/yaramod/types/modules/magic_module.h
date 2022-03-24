/**
 * @file src/types/modules/magic_module.h
 * @brief Declaration of MagicModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c magic module.
 */
class MagicModule : public Module
{
public:
	/// @name Constructors
	/// @{
	MagicModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~MagicModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
