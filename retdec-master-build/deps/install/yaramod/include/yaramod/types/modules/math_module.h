/**
 * @file src/types/modules/math_module.h
 * @brief Declaration of MathModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c math module.
 */
class MathModule : public Module
{
public:
	/// @name Constructors
	/// @{
	MathModule();
	/// @}

	/// @name Destructor
	/// @{
	virtual ~MathModule() override = default;
	/// @}

	/// @name Initialization method
	/// @{
	virtual bool initialize(ImportFeatures features) override;
	/// @}
};

}
