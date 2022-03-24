/**
 * @file src/types/modules/hash_module.cpp
 * @brief Implementation of HashModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/hash_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
HashModule::HashModule() : Module("hash", ImportFeatures::Basic)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool HashModule::initialize(ImportFeatures/* features*/)
{
	using Type = Expression::Type;

	auto hashStruct = std::make_shared<StructureSymbol>("hash");

	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("md5", Type::String, Type::Int, Type::Int));
	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("md5", Type::String, Type::String));

	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("sha1", Type::String, Type::Int, Type::Int));
	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("sha1", Type::String, Type::String));

	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("sha256", Type::String, Type::Int, Type::Int));
	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("sha256", Type::String, Type::String));

	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("checksum32", Type::Int, Type::Int, Type::Int));
	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("checksum32", Type::Int, Type::String));

	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("crc32", Type::Int, Type::Int, Type::Int));
	hashStruct->addAttribute(std::make_shared<FunctionSymbol>("crc32", Type::Int, Type::String));

	_structure = hashStruct;
	return true;
}

}
