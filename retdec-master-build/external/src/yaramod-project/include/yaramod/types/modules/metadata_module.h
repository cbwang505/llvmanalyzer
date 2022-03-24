/**
 * @file src/types/modules/metadata_module.h
 * @brief Declaration of MetadataModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include "yaramod/types/modules/module.h"

namespace yaramod {

/**
 * Class representing @c metadata module.
 */
class MetadataModule : public Module
{
public:
    /// @name Constructors
    /// @{
    MetadataModule();
    /// @}

    /// @name Destructor
    /// @{
    virtual ~MetadataModule() override = default;
    /// @}

    /// @name Initialization method
    /// @{
    virtual bool initialize(ImportFeatures features) override;
    /// @}
};

}
