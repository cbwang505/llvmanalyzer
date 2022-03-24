/**
 * @file src/types/modules/pe_module.cpp
 * @brief Implementation of PeModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/pe_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
PeModule::PeModule() : Module("pe", ImportFeatures::Basic)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool PeModule::initialize(ImportFeatures/* features*/)
{
	using Type = Expression::Type;

	auto peStruct = std::make_shared<StructureSymbol>("pe");

	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_UNKNOWN", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_AM33", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_AMD64", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_ARM", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_ARMNT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_ARM64", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_EBC", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_I386", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_IA64", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_M32R", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_MIPS16", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_MIPSFPU", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_MIPSFPU16", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_POWERPC", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_POWERPCFP", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_R4000", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_SH3", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_SH3DSP", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_SH4", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_SH5", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_THUMB", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_WCEMIPSV2", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_UNKNOWN", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_NATIVE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_WINDOWS_GUI", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_WINDOWS_CUI", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_OS2_CUI", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_POSIX_CUI", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_NATIVE_WINDOWS", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_WINDOWS_CE_GUI", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_EFI_APPLICATION", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_EFI_RUNTIME_DRIVER", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_XBOX", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SUBSYSTEM_WINDOWS_BOOT_APPLICATION", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("DYNAMIC_BASE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("FORCE_INTEGRITY", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("NX_COMPAT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("NO_ISOLATION", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("NO_SEH", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("NO_BIND", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("WDM_DRIVER", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("TERMINAL_SERVER_AWARE", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("RELOCS_STRIPPED", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("EXECUTABLE_IMAGE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("LINE_NUMS_STRIPPED", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("LOCAL_SYMS_STRIPPED", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("AGGRESIVE_WS_TRIM", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("LARGE_ADDRESS_AWARE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("BYTES_REVERSED_LO", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("MACHINE_32BIT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("DEBUG_STRIPPED", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("REMOVABLE_RUN_FROM_SWAP", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("NET_RUN_FROM_SWAP", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SYSTEM", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("DLL", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("UP_SYSTEM_ONLY", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("BYTES_REVERSED_HI", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_EXPORT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_IMPORT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_RESOURCE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_EXCEPTION", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_SECURITY", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_BASERELOC", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_DEBUG", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_ARCHITECTURE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_GLOBALPTR", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_TLS", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_IAT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_CNT_CODE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_CNT_INITIALIZED_DATA", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_CNT_UNINITIALIZED_DATA", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_GPREL", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_MEM_16BIT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_LNK_NRELOC_OVFL", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_MEM_DISCARDABLE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_MEM_NOT_CACHED", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_MEM_NOT_PAGED", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_MEM_SHARED", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_MEM_EXECUTE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_MEM_READ", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_MEM_WRITE", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_CURSOR", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_BITMAP", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_ICON", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_MENU", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_DIALOG", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_STRING", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_FONTDIR", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_FONT", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_ACCELERATOR", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_RCDATA", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_MESSAGETABLE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_GROUP_CURSOR", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_GROUP_ICON", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_VERSION", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_DLGINCLUDE", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_PLUGPLAY", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_VXD", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_ANICURSOR", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_ANIICON", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_HTML", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("RESOURCE_TYPE_MANIFEST", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("is_pe", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("machine", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_sections", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("timestamp", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("pointer_to_symbol_table", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_symbols", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_optional_header", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("characteristics", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("entry_point", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("image_base", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_rva_and_sizes", Type::Int));

	peStruct->addAttribute(std::make_shared<DictionarySymbol>("version_info", Type::String));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("opthdr_magic", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_code", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_initialized_data", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_uninitialized_data", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("base_of_code", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("base_of_data", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("section_alignment", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("file_alignment", Type::Int));

	auto linkerVersionStruct = std::make_shared<StructureSymbol>("linker_version");
	peStruct->addAttribute(linkerVersionStruct);
	linkerVersionStruct->addAttribute(std::make_shared<ValueSymbol>("major", Type::Int));
	linkerVersionStruct->addAttribute(std::make_shared<ValueSymbol>("minor", Type::Int));

	auto osVersionStruct = std::make_shared<StructureSymbol>("os_version");
	peStruct->addAttribute(osVersionStruct);
	osVersionStruct->addAttribute(std::make_shared<ValueSymbol>("major", Type::Int));
	osVersionStruct->addAttribute(std::make_shared<ValueSymbol>("minor", Type::Int));

	auto imageVersionStruct = std::make_shared<StructureSymbol>("image_version");
	peStruct->addAttribute(imageVersionStruct);
	imageVersionStruct->addAttribute(std::make_shared<ValueSymbol>("major", Type::Int));
	imageVersionStruct->addAttribute(std::make_shared<ValueSymbol>("minor", Type::Int));

	auto subsystemVersionStruct = std::make_shared<StructureSymbol>("subsystem_version");
	peStruct->addAttribute(subsystemVersionStruct);
	subsystemVersionStruct->addAttribute(std::make_shared<ValueSymbol>("major", Type::Int));
	subsystemVersionStruct->addAttribute(std::make_shared<ValueSymbol>("minor", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("win32_version_value", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_image", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_headers", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("checksum", Type::Int));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("calculate_checksum", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("subsystem", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("dll_characteristics", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_stack_reserve", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_stack_commit", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_heap_reserve", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("size_of_heap_commit", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("loader_flags", Type::Int));

	auto dataDirectoriesStruct = std::make_shared<StructureSymbol>("data_directories");
	dataDirectoriesStruct->addAttribute(std::make_shared<ValueSymbol>("virtual_address", Type::Int));
	dataDirectoriesStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	peStruct->addAttribute(std::make_shared<ArraySymbol>("data_directories", dataDirectoriesStruct));

	auto sectionsStruct = std::make_shared<StructureSymbol>("sections");
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("characteristics", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("virtual_address", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("virtual_size", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("raw_data_offset", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("raw_data_size", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("pointer_to_relocations", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("pointer_to_line_numbers", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_relocations", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_line_numbers", Type::Int));
	peStruct->addAttribute(std::make_shared<ArraySymbol>("sections", sectionsStruct));

	auto overlayStruct = std::make_shared<StructureSymbol>("overlay");
	peStruct->addAttribute(overlayStruct);
	overlayStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	overlayStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));

	auto richSignatureStruct = std::make_shared<StructureSymbol>("rich_signature");
	peStruct->addAttribute(richSignatureStruct);
	richSignatureStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	richSignatureStruct->addAttribute(std::make_shared<ValueSymbol>("length", Type::Int));
	richSignatureStruct->addAttribute(std::make_shared<ValueSymbol>("key", Type::Int));
	richSignatureStruct->addAttribute(std::make_shared<ValueSymbol>("raw_data", Type::String));
	richSignatureStruct->addAttribute(std::make_shared<ValueSymbol>("clear_data", Type::String));
	richSignatureStruct->addAttribute(std::make_shared<FunctionSymbol>("version", Type::Int, Type::Int));
	richSignatureStruct->addAttribute(std::make_shared<FunctionSymbol>("version", Type::Int, Type::Int, Type::Int));
	richSignatureStruct->addAttribute(std::make_shared<FunctionSymbol>("toolid", Type::Int, Type::Int));
	richSignatureStruct->addAttribute(std::make_shared<FunctionSymbol>("toolid", Type::Int, Type::Int, Type::Int));

	peStruct->addAttribute(std::make_shared<FunctionSymbol>("imphash", Type::String));

	peStruct->addAttribute(std::make_shared<FunctionSymbol>("section_index", Type::Int, Type::String));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("section_index", Type::Int, Type::Int));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("exports", Type::Int, Type::String));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("exports", Type::Int, Type::Regexp));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("exports", Type::Int, Type::Int));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("imports", Type::Int, Type::String, Type::String));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("imports", Type::Int, Type::String, Type::Int));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("imports", Type::Int, Type::String));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("imports", Type::Int, Type::Regexp, Type::Regexp));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("locale", Type::Int, Type::Int));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("language", Type::Int, Type::Int));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("is_dll", Type::Int));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("is_32bit", Type::Int));
	peStruct->addAttribute(std::make_shared<FunctionSymbol>("is_64bit", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_imports", Type::Int));
	peStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_exports", Type::Int));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("resource_timestamp", Type::Int));

	auto resourceVersionStruct = std::make_shared<StructureSymbol>("resource_version");
	peStruct->addAttribute(resourceVersionStruct);
	resourceVersionStruct->addAttribute(std::make_shared<ValueSymbol>("major", Type::Int));
	resourceVersionStruct->addAttribute(std::make_shared<ValueSymbol>("minor", Type::Int));

	auto resourcesStruct = std::make_shared<StructureSymbol>("resources");
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("length", Type::Int));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("type", Type::Int));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("id", Type::Int));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("language", Type::Int));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("type_string", Type::String));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("name_string", Type::String));
	resourcesStruct->addAttribute(std::make_shared<ValueSymbol>("language_string", Type::String));
	peStruct->addAttribute(std::make_shared<ArraySymbol>("resources", resourcesStruct));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_resources", Type::Int));

	auto signaturesStruct = std::make_shared<StructureSymbol>("signatures");
	signaturesStruct->addAttribute(std::make_shared<ValueSymbol>("thumbprint", Type::String));
	signaturesStruct->addAttribute(std::make_shared<ValueSymbol>("issuer", Type::String));
	signaturesStruct->addAttribute(std::make_shared<ValueSymbol>("subject", Type::String));
	signaturesStruct->addAttribute(std::make_shared<ValueSymbol>("version", Type::Int));
	signaturesStruct->addAttribute(std::make_shared<ValueSymbol>("algorithm", Type::String));
	signaturesStruct->addAttribute(std::make_shared<ValueSymbol>("serial", Type::String));
	signaturesStruct->addAttribute(std::make_shared<ValueSymbol>("not_before", Type::Int));
	signaturesStruct->addAttribute(std::make_shared<ValueSymbol>("not_after", Type::Int));
	signaturesStruct->addAttribute(std::make_shared<FunctionSymbol>("valid_on", Type::Int, Type::Int));
	peStruct->addAttribute(std::make_shared<ArraySymbol>("signatures", signaturesStruct));

	peStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_signatures", Type::Int));

	peStruct->addAttribute(std::make_shared<FunctionSymbol>("iconhash", Type::String));

	peStruct->addAttribute(std::make_shared<FunctionSymbol>("rva_to_offset", Type::Int, Type::Int));

	_structure = peStruct;
	return true;
}

}
