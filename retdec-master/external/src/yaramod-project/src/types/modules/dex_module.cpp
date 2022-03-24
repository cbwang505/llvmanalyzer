/**
 * @file src/types/modules/dex_module.cpp
 * @brief Implementation of DexModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/dex_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
DexModule::DexModule() : Module("dex", ImportFeatures::Basic)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool DexModule::initialize(ImportFeatures/* features*/)
{
	using Type = Expression::Type;

	auto dexStruct = std::make_shared<StructureSymbol>("dex");

	dexStruct->addAttribute(std::make_shared<ValueSymbol>("DEX_FILE_MAGIC_035", Type::String));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("DEX_FILE_MAGIC_036", Type::String));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("DEX_FILE_MAGIC_037", Type::String));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("DEX_FILE_MAGIC_038", Type::String));

	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ENDIAN_CONSTANT", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("REVERSE_ENDIAN_CONSTANT", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("NO_INDEX", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_PUBLIC", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_PRIVATE", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_PROTECTED", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_STATIC", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_FINAL", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_SYNCHRONIZED", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_VOLATILE", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_BRIDGE", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_TRANSIENT", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_VARARGS", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_NATIVE", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_INTERFACE", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_ABSTRACT", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_STRICT", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_SYNTHETIC", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_ANNOTATION", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_ENUM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_CONSTRUCTOR", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("ACC_DECLARED_SYNCHRONIZED", Type::Int));

	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_HEADER_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_STRING_ID_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_TYPE_ID_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_PROTO_ID_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_FIELD_ID_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_METHOD_ID_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_CLASS_DEF_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_CALL_SITE_ID_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_METHOD_HANDLE_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_MAP_LIST", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_TYPE_LIST", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_ANNOTATION_SET_REF_LIST", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_ANNOTATION_SET_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_CLASS_DATA_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_CODE_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_STRING_DATA_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_DEBUG_INFO_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_ANNOTATION_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_ENCODED_ARRAY_ITEM", Type::Int));
	dexStruct->addAttribute(std::make_shared<ValueSymbol>("TYPE_ANNOTATIONS_DIRECTORY_ITEM", Type::Int));

	auto headerStruct = std::make_shared<StructureSymbol>("header");
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("magic", Type::String));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("checksum", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("signature", Type::String));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("file_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("header_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("endian_tag", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("link_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("link_offset", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("map_offset", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("string_ids_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("string_ids_offset", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("type_ids_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("type_ids_offset", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("proto_ids_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("proto_ids_offset", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("field_ids_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("field_ids_offset", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("method_ids_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("method_ids_offset", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("class_defs_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("class_defs_offset", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("data_size", Type::Int));
	headerStruct->addAttribute(std::make_shared<ValueSymbol>("data_offset", Type::Int));
	dexStruct->addAttribute(headerStruct);

	auto stringIdsStruct = std::make_shared<StructureSymbol>("string_ids");
	stringIdsStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	stringIdsStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	stringIdsStruct->addAttribute(std::make_shared<ValueSymbol>("value", Type::String));
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("string_ids", stringIdsStruct));

	auto typeIdsStruct = std::make_shared<StructureSymbol>("type_ids");
	typeIdsStruct->addAttribute(std::make_shared<ValueSymbol>("descriptor_idx", Type::Int));
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("type_ids", typeIdsStruct));

	auto protoIdsStruct = std::make_shared<StructureSymbol>("proto_ids");
	protoIdsStruct->addAttribute(std::make_shared<ValueSymbol>("shorty_idx", Type::Int));
	protoIdsStruct->addAttribute(std::make_shared<ValueSymbol>("return_type_idx", Type::Int));
	protoIdsStruct->addAttribute(std::make_shared<ValueSymbol>("parameters_offset", Type::Int));
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("proto_ids", protoIdsStruct));

	auto fieldIdsStruct = std::make_shared<StructureSymbol>("field_ids");
	fieldIdsStruct->addAttribute(std::make_shared<ValueSymbol>("class_idx", Type::Int));
	fieldIdsStruct->addAttribute(std::make_shared<ValueSymbol>("type_idx", Type::Int));
	fieldIdsStruct->addAttribute(std::make_shared<ValueSymbol>("name_idx", Type::Int));
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("field_ids", fieldIdsStruct));

	auto methodIdsStruct = std::make_shared<StructureSymbol>("method_ids");
	methodIdsStruct->addAttribute(std::make_shared<ValueSymbol>("class_idx", Type::Int));
	methodIdsStruct->addAttribute(std::make_shared<ValueSymbol>("proto_idx", Type::Int));
	methodIdsStruct->addAttribute(std::make_shared<ValueSymbol>("name_idx", Type::Int));
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("method_ids", methodIdsStruct));

	auto classDefsStruct = std::make_shared<StructureSymbol>("class_defs");
	classDefsStruct->addAttribute(std::make_shared<ValueSymbol>("class_idx", Type::Int));
	classDefsStruct->addAttribute(std::make_shared<ValueSymbol>("access_flags", Type::Int));
	classDefsStruct->addAttribute(std::make_shared<ValueSymbol>("superclass_idx", Type::Int));
	classDefsStruct->addAttribute(std::make_shared<ValueSymbol>("interfaces_offset", Type::Int));
	classDefsStruct->addAttribute(std::make_shared<ValueSymbol>("source_file_idx", Type::Int));
	classDefsStruct->addAttribute(std::make_shared<ValueSymbol>("annotations_offset", Type::Int));
	classDefsStruct->addAttribute(std::make_shared<ValueSymbol>("class_data_offset", Type::Int));
	classDefsStruct->addAttribute(std::make_shared<ValueSymbol>("static_values_offset", Type::Int));
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("class_defs", classDefsStruct));

	auto classDataItemStruct = std::make_shared<StructureSymbol>("class_data_item");
	classDataItemStruct->addAttribute(std::make_shared<ValueSymbol>("static_fields_size", Type::Int));
	classDataItemStruct->addAttribute(std::make_shared<ValueSymbol>("instance_fields_size", Type::Int));
	classDataItemStruct->addAttribute(std::make_shared<ValueSymbol>("direct_methods_size", Type::Int));
	classDataItemStruct->addAttribute(std::make_shared<ValueSymbol>("virtual_methods_size", Type::Int));
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("class_data_item", classDataItemStruct));

	auto mapListStruct = std::make_shared<StructureSymbol>("map_list");
	mapListStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	auto mapItemStruct = std::make_shared<StructureSymbol>("map_item");
	mapItemStruct->addAttribute(std::make_shared<ValueSymbol>("type", Type::Int));
	mapItemStruct->addAttribute(std::make_shared<ValueSymbol>("unused", Type::Int));
	mapItemStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	mapItemStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	mapListStruct->addAttribute(std::make_shared<ArraySymbol>("map_item", mapItemStruct));
	dexStruct->addAttribute(mapListStruct);

	dexStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_fields", Type::Int));
	auto fieldStruct = std::make_shared<StructureSymbol>("field");
	fieldStruct->addAttribute(std::make_shared<ValueSymbol>("class_name", Type::String));
	fieldStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	fieldStruct->addAttribute(std::make_shared<ValueSymbol>("proto", Type::String));
	fieldStruct->addAttribute(std::make_shared<ValueSymbol>("field_idx_diff", Type::Int));
	fieldStruct->addAttribute(std::make_shared<ValueSymbol>("access_flags", Type::Int));
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("field", fieldStruct));

	dexStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_methods", Type::Int));
	auto methodStruct = std::make_shared<StructureSymbol>("method");
	methodStruct->addAttribute(std::make_shared<ValueSymbol>("class_name", Type::String));
	methodStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	methodStruct->addAttribute(std::make_shared<ValueSymbol>("proto", Type::String));
	methodStruct->addAttribute(std::make_shared<ValueSymbol>("direct", Type::Int));
	methodStruct->addAttribute(std::make_shared<ValueSymbol>("virtual", Type::Int));
	methodStruct->addAttribute(std::make_shared<ValueSymbol>("method_idx_diff", Type::Int));
	methodStruct->addAttribute(std::make_shared<ValueSymbol>("access_flags", Type::Int));
	methodStruct->addAttribute(std::make_shared<ValueSymbol>("code_off", Type::Int));
	auto codeItemStruct = std::make_shared<StructureSymbol>("code_item");
	codeItemStruct->addAttribute(std::make_shared<ValueSymbol>("registers_size", Type::Int));
	codeItemStruct->addAttribute(std::make_shared<ValueSymbol>("ins_size", Type::Int));
	codeItemStruct->addAttribute(std::make_shared<ValueSymbol>("outs_size", Type::Int));
	codeItemStruct->addAttribute(std::make_shared<ValueSymbol>("tries_size", Type::Int));
	codeItemStruct->addAttribute(std::make_shared<ValueSymbol>("debug_info_off", Type::Int));
	codeItemStruct->addAttribute(std::make_shared<ValueSymbol>("insns_size", Type::Int));
	codeItemStruct->addAttribute(std::make_shared<ValueSymbol>("insns", Type::String));
	codeItemStruct->addAttribute(std::make_shared<ValueSymbol>("padding", Type::Int));
	auto triesStruct = std::make_shared<StructureSymbol>("tries");
	codeItemStruct->addAttribute(triesStruct);
	auto handlersStruct = std::make_shared<StructureSymbol>("handlers");
	codeItemStruct->addAttribute(std::make_shared<ArraySymbol>("handlers", handlersStruct));
	methodStruct->addAttribute(codeItemStruct);
	dexStruct->addAttribute(std::make_shared<ArraySymbol>("method", methodStruct));

	_structure = dexStruct;
	return true;
}

}
