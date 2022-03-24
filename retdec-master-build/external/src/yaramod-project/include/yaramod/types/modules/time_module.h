/**
 * @file src/types/modules/time_module.h
 * @brief Declaration of TimeModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c time module.
 */
class TimeModule : public Module
{
public:
	/// @name Constructors
	/// @{
	TimeModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~TimeModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
