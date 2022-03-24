/**
 * @file src/types/modules/hash_module.h
 * @brief Declaration of HashModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c hash module.
 */
class HashModule : public Module
{
public:
	/// @name Constructors
	/// @{
	HashModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~HashModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
