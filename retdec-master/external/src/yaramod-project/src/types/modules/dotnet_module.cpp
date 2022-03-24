/**
 * @file src/types/modules/dotnet_module.cpp
 * @brief Implementation of DotnetModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/dotnet_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
DotnetModule::DotnetModule() : Module("dotnet", ImportFeatures::Basic)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool DotnetModule::initialize(ImportFeatures/* features*/)
{
	using Type = Expression::Type;

	auto dotnetStruct = std::make_shared<StructureSymbol>("dotnet");

	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("version", Type::String));
	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("module_name", Type::String));

	auto streamsStruct = std::make_shared<StructureSymbol>("streams");
	streamsStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	streamsStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	streamsStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	dotnetStruct->addAttribute(std::make_shared<ArraySymbol>("streams", streamsStruct));

	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_streams", Type::Int));

	dotnetStruct->addAttribute(std::make_shared<ArraySymbol>("guids", Type::String));
	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_guids", Type::Int));

	auto resourcesStruct = std::make_shared<StructureSymbol>("resources");
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("length", Type::Int));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	dotnetStruct->addAttribute(std::make_shared<ArraySymbol>("resources", resourcesStruct));

	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_resources", Type::Int));

	auto assemblyRefStruct = std::make_shared<StructureSymbol>("assembly_refs");
	auto versionRefStruct = std::make_shared<StructureSymbol>("version");
	versionRefStruct->addAttribute(std::make_shared<ValueSymbol>("major", Type::Int));
	versionRefStruct->addAttribute(std::make_shared<ValueSymbol>("minor", Type::Int));
	versionRefStruct->addAttribute(std::make_shared<ValueSymbol>("build_number", Type::Int));
	versionRefStruct->addAttribute(std::make_shared<ValueSymbol>("revision_number", Type::Int));
	assemblyRefStruct->addAttribute(versionRefStruct);
	assemblyRefStruct->addAttribute(std::make_shared<ValueSymbol>("public_key_or_token", Type::String));
	assemblyRefStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	dotnetStruct->addAttribute(std::make_shared<ArraySymbol>("assembly_refs", assemblyRefStruct));

	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_assembly_refs", Type::Int));

	auto assemblyStruct = std::make_shared<StructureSymbol>("assembly");
	auto versionStruct = std::make_shared<StructureSymbol>("version");
	versionStruct->addAttribute(std::make_shared<ValueSymbol>("major", Type::Int));
	versionStruct->addAttribute(std::make_shared<ValueSymbol>("minor", Type::Int));
	versionStruct->addAttribute(std::make_shared<ValueSymbol>("build_number", Type::Int));
	versionStruct->addAttribute(std::make_shared<ValueSymbol>("revision_number", Type::Int));
	assemblyStruct->addAttribute(versionStruct);
	assemblyStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	assemblyStruct->addAttribute(std::make_shared<ValueSymbol>("culture", Type::String));
	dotnetStruct->addAttribute(assemblyStruct);

	dotnetStruct->addAttribute(std::make_shared<ArraySymbol>("modulerefs", Type::String));
	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_modulerefs", Type::Int));

	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_user_strings", Type::Int));
	dotnetStruct->addAttribute(std::make_shared<ArraySymbol>("user_strings", Type::String));

	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("typelib", Type::String));

	dotnetStruct->addAttribute(std::make_shared<ArraySymbol>("constants", Type::String));
	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_constants", Type::Int));

	dotnetStruct->addAttribute(std::make_shared<ArraySymbol>("field_offsets", Type::Int));
	dotnetStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_field_offsets", Type::Int));

	_structure = dotnetStruct;
	return true;
}

}
