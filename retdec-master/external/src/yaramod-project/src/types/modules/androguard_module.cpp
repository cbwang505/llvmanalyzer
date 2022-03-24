/**
 * @file src/types/modules/androguard_module.cpp
 * @brief Implementation of AndroguardModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/androguard_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
AndroguardModule::AndroguardModule() : Module("androguard", ImportFeatures::AvastOnly)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool AndroguardModule::initialize(ImportFeatures)
{
	using Type = Expression::Type;

	auto androguardStruct = std::make_shared<StructureSymbol>("androguard");

	auto certificateStruct = std::make_shared<StructureSymbol>("certificate");
	certificateStruct->addAttribute(std::make_shared<FunctionSymbol>("issuer", Type::Int, Type::Regexp));
	certificateStruct->addAttribute(std::make_shared<FunctionSymbol>("subject", Type::Int, Type::Regexp));
	certificateStruct->addAttribute(std::make_shared<FunctionSymbol>("sha1", Type::Int, Type::String));
	certificateStruct->addAttribute(std::make_shared<FunctionSymbol>("not_after", Type::Int, Type::Regexp));
	certificateStruct->addAttribute(std::make_shared<FunctionSymbol>("not_after", Type::Int, Type::String));
	certificateStruct->addAttribute(std::make_shared<FunctionSymbol>("not_before", Type::Int, Type::Regexp));
	certificateStruct->addAttribute(std::make_shared<FunctionSymbol>("not_before", Type::Int, Type::String));
	certificateStruct->addAttribute(std::make_shared<FunctionSymbol>("trusted_by", Type::Int, Type::String));
	androguardStruct->addAttribute(certificateStruct);

	androguardStruct->addAttribute(std::make_shared<ValueSymbol>("min_sdk", Type::Int));
	androguardStruct->addAttribute(std::make_shared<ValueSymbol>("max_sdk", Type::Int));
	androguardStruct->addAttribute(std::make_shared<ValueSymbol>("target_sdk", Type::Int));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("displayed_version", Type::Int, Type::Regexp));
	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("displayed_version", Type::Int, Type::String));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("url", Type::Int, Type::Regexp));
	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("url", Type::Int, Type::String));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("app_name", Type::Int, Type::Regexp));
	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("app_name", Type::Int, Type::String));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("permission", Type::Int, Type::Regexp));

	androguardStruct->addAttribute(std::make_shared<ValueSymbol>("permissions_number", Type::Int));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("receiver", Type::Int, Type::Regexp));
	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("receiver", Type::Int, Type::String));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("activity", Type::Int, Type::Regexp));
	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("activity", Type::Int, Type::String));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("main_activity", Type::Int, Type::Regexp));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("service", Type::Int, Type::Regexp));
	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("service", Type::Int, Type::String));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("filter", Type::Int, Type::Regexp));
	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("filter", Type::Int, Type::String));

	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("package_name", Type::Int, Type::Regexp));
	androguardStruct->addAttribute(std::make_shared<FunctionSymbol>("package_name", Type::Int, Type::String));

	auto signatureStruct = std::make_shared<StructureSymbol>("signature");
	signatureStruct->addAttribute(std::make_shared<FunctionSymbol>("hits", Type::Int, Type::Regexp));
	signatureStruct->addAttribute(std::make_shared<FunctionSymbol>("hits", Type::Int, Type::Regexp, Type::Regexp));
	signatureStruct->addAttribute(std::make_shared<FunctionSymbol>("hits", Type::Int, Type::String));
	signatureStruct->addAttribute(std::make_shared<FunctionSymbol>("hits", Type::Int, Type::String, Type::Regexp));
	androguardStruct->addAttribute(signatureStruct);

	_structure = androguardStruct;
	return true;
}

}
