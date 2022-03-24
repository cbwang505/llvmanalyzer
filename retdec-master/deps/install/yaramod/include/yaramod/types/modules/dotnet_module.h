/**
 * @file src/types/modules/dotnet_module.h
 * @brief Declaration of DotnetModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c dotnet module.
 */
class DotnetModule : public Module
{
public:
	/// @name Constructors
	/// @{
	DotnetModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~DotnetModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
