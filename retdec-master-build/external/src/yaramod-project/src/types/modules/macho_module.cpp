/** * @file src/types/modules/macho_module.cpp
 * @brief Implementation of MachoModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/macho_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

namespace {

void initializeCommonAttributes(const std::shared_ptr<StructureSymbol>& rootStruct)
{
	using Type = Expression::Type;

	rootStruct->addAttribute(std::make_shared<ValueSymbol>("magic", Type::Int));
	rootStruct->addAttribute(std::make_shared<ValueSymbol>("cputype", Type::Int));
	rootStruct->addAttribute(std::make_shared<ValueSymbol>("cpusubtype", Type::Int));
	rootStruct->addAttribute(std::make_shared<ValueSymbol>("filetype", Type::Int));
	rootStruct->addAttribute(std::make_shared<ValueSymbol>("ncmds", Type::Int));
	rootStruct->addAttribute(std::make_shared<ValueSymbol>("sizeofcmds", Type::Int));
	rootStruct->addAttribute(std::make_shared<ValueSymbol>("flags", Type::Int));
	rootStruct->addAttribute(std::make_shared<ValueSymbol>("reserved", Type::Int));

	rootStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_segments", Type::Int));

	auto segmentsStruct = std::make_shared<StructureSymbol>("segments");
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("segname", Type::String));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("vmaddr", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("vmsize", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("fileoff", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("fsize", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("maxprot", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("initprot", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("nsects", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("flags", Type::Int));
	auto sectionsStruct = std::make_shared<StructureSymbol>("sections");
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("sectname", Type::String));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("segname", Type::String));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("addr", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("align", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("reloff", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("nreloc", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("flags", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("reserved1", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("reserved2", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("reserved3", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ArraySymbol>("sections", sectionsStruct));
	rootStruct->addAttribute(std::make_shared<ArraySymbol>("segments", segmentsStruct));

	rootStruct->addAttribute(std::make_shared<ValueSymbol>("entry_point", Type::Int));
	rootStruct->addAttribute(std::make_shared<ValueSymbol>("stack_size", Type::Int));
}

}

/**
 * Constructor.
 */
MachoModule::MachoModule() : Module("macho", ImportFeatures::Basic)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool MachoModule::initialize(ImportFeatures /*features*/)
{
	using Type = Expression::Type;

	auto machoStruct = std::make_shared<StructureSymbol>("macho");

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_MAGIC", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_CIGAM", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_MAGIC_64", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_CIGAM_64", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("FAT_MAGIC", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("FAT_CIGAM", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("FAT_MAGIC_64", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("FAT_CIGAM_64", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_ARCH_ABI64", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_LIB64", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_MC680X0", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_X86", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_I386", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_X86_64", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_MIPS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_MC98000", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_ARM", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_ARM64", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_MC88000", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_SPARC", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_POWERPC", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_TYPE_POWERPC64", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_INTEL_MODEL_ALL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_386", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_I386_ALL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_X86_64_ALL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_486", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_486SX", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_586", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENT", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTPRO", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTII_M3", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTII_M5", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_CELERON", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_CELERON_MOBILE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTIUM_3", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTIUM_3_M", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTIUM_3_XEON", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTIUM_M", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTIUM_4", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_PENTIUM_4_M", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ITANIUM", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ITANIUM_2", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_XEON", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_XEON_MP", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_ALL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V4T", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V6", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V5", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V5TEJ", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_XSCALE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V7", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V7F", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V7S", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V7K", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V6M", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V7M", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM_V7EM", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_ARM64_ALL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_SPARC_ALL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_ALL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_MC980000_ALL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_601", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_MC98601", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_602", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_603", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_603e", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_603ev", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_604", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_604e", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_620", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_750", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_7400", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_7450", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("CPU_SUBTYPE_POWERPC_970", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_OBJECT", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_EXECUTE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_FVMLIB", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_CORE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_PRELOAD", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_DYLIB", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_DYLINKER", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_BUNDLE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_DYLIB_STUB", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_DSYM", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_KEXT_BUNDLE", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_NOUNDEFS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_INCRLINK", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_DYLDLINK", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_BINDATLOAD", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_PREBOUND", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_SPLIT_SEGS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_LAZY_INIT", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_TWOLEVEL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_FORCE_FLAT", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_NOMULTIDEFS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_NOFIXPREBINDING", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_PREBINDABLE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_ALLMODSBOUND", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_SUBSECTIONS_VIA_SYMBOLS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_CANONICAL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_WEAK_DEFINES", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_BINDS_TO_WEAK", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_ALLOW_STACK_EXECUTION", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_ROOT_SAFE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_SETUID_SAFE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_NO_REEXPORTED_DYLIBS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_PIE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_DEAD_STRIPPABLE_DYLIB", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_HAS_TLV_DESCRIPTORS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_NO_HEAP_EXECUTION", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("MH_APP_EXTENSION_SAFE", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("SG_HIGHVM", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("SG_FVMLIB", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("SG_NORELOC", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("SG_PROTECTED_VERSION_1", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_TYPE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("SECTION_ATTRIBUTES", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_REGULAR", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ZEROFILL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_CSTRING_LITERALS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_4BYTE_LITERALS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_8BYTE_LITERALS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_LITERAL_POINTERS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_NON_LAZY_SYMBOL_POINTERS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_LAZY_SYMBOL_POINTERS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_SYMBOL_STUBS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_MOD_INIT_FUNC_POINTERS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_MOD_TERM_FUNC_POINTERS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_COALESCED", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_GB_ZEROFILL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_INTERPOSING", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_16BYTE_LITERALS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_DTRACE_DOF", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_LAZY_DYLIB_SYMBOL_POINTERS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_THREAD_LOCAL_REGULAR", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_THREAD_LOCAL_ZEROFILL", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_THREAD_LOCAL_VARIABLES", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_THREAD_LOCAL_VARIABLE_POINTERS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_THREAD_LOCAL_INIT_FUNCTION_POINTERS", Type::Int));

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_PURE_INSTRUCTIONS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_NO_TOC", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_STRIP_STATIC_SYMS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_NO_DEAD_STRIP", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_LIVE_SUPPORT", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_SELF_MODIFYING_CODE", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_DEBUG", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_SOME_INSTRUCTIONS", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_EXT_RELOC", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("S_ATTR_LOC_RELOC", Type::Int));

	initializeCommonAttributes(machoStruct);

	machoStruct->addAttribute(std::make_shared<ValueSymbol>("fat_magic", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("nfat_arch", Type::Int));

	auto fatArchStruct = std::make_shared<StructureSymbol>("fat_arch");
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("cputype", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("cpusubtype", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	machoStruct->addAttribute(std::make_shared<ValueSymbol>("align", Type::Int));
	machoStruct->addAttribute(std::make_shared<ArraySymbol>("fat_arch", fatArchStruct));

	auto fileStruct = std::make_shared<StructureSymbol>("file");
	initializeCommonAttributes(fileStruct);
	machoStruct->addAttribute(std::make_shared<ArraySymbol>("file", fileStruct));

	machoStruct->addAttribute(std::make_shared<FunctionSymbol>("file_index_for_arch", Type::Int, Type::Int));
	machoStruct->addAttribute(std::make_shared<FunctionSymbol>("file_index_for_arch", Type::Int, Type::Int, Type::Int));
	machoStruct->addAttribute(std::make_shared<FunctionSymbol>("entry_point_for_arch", Type::Int, Type::Int));
	machoStruct->addAttribute(std::make_shared<FunctionSymbol>("entry_point_for_arch", Type::Int, Type::Int, Type::Int));

	_structure = machoStruct;
	return true;
}

}
