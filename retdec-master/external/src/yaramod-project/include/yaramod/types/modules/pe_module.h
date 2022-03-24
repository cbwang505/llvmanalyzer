/**
 * @file src/types/modules/pe_module.h
 * @brief Declaration of PeModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c pe module.
 */
class PeModule : public Module
{
public:
	/// @name Constructors
	/// @{
	PeModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~PeModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
