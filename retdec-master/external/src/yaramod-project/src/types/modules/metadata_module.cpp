/**
 * @file src/types/modules/metadata_module.cpp
 * @brief Implementation of MetadataModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/metadata_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
MetadataModule::MetadataModule() : Module("metadata", ImportFeatures::Basic)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool MetadataModule::initialize(ImportFeatures features)
{
	using Type = Expression::Type;
	auto metadataStruct = std::make_shared<StructureSymbol>("metadata");

	auto fileStruct = std::make_shared<StructureSymbol>("file");
	if (features & ImportFeatures::AvastOnly)
	{
		fileStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::String));
		fileStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::Regexp));
	}
	metadataStruct->addAttribute(fileStruct);

	auto detectionStruct = std::make_shared<StructureSymbol>("detection");
	if (features & ImportFeatures::AvastOnly)
	{
		detectionStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::Regexp));
		detectionStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::String, Type::Regexp));
	}
	metadataStruct->addAttribute(detectionStruct);

	_structure = metadataStruct;
	return true;
}

}
