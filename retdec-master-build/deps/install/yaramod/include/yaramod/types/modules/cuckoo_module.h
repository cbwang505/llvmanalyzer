/**
 * @file src/types/modules/cuckoo_module.h
 * @brief Declaration of CuckooModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c cuckoo module.
 */
class CuckooModule : public Module
{
public:
	/// @name Constructors
	/// @{
	CuckooModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~CuckooModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
