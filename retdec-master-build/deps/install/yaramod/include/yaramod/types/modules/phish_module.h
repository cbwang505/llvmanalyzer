/**
 * @file src/types/modules/phish_module.h
 * @brief Declaration of PhishModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c phish module.
 */
class PhishModule : public Module
{
public:
	/// @name Constructors
	/// @{
	PhishModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~PhishModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
