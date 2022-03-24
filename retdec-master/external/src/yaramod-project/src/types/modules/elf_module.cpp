/**
 * @file src/types/modules/elf_module.cpp
 * @brief Implementation of ElfModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/elf_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
ElfModule::ElfModule() : Module("elf", ImportFeatures::Basic)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool ElfModule::initialize(ImportFeatures/* features*/)
{
	using Type = Expression::Type;

	auto elfStruct = std::make_shared<StructureSymbol>("elf");

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("ET_NONE", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("ET_REL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("ET_EXEC", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("ET_DYN", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("ET_CORE", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_NONE", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_M32", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_SPARC", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_386", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_68K", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_88K", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_860", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_MIPS", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_MIPS_RS3_LE", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_PPC", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_PPC64", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_ARM", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_X86_64", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("EM_AARCH64", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_NULL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_PROGBITS", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_SYMTAB", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_STRTAB", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_RELA", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_HASH", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_DYNAMIC", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_NOTE", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_NOBITS", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_REL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_SHLIB", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHT_DYNSYM", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHF_WRITE", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHF_ALLOC", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("SHF_EXECINSTR", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("type", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("machine", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("entry_point", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_sections", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("sh_offset", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("sh_entry_size", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("number_of_segments", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("ph_offset", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("ph_entry_size", Type::Int));

	auto sectionsStruct = std::make_shared<StructureSymbol>("sections");
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("type", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("flags", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("address", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	sectionsStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	elfStruct->addAttribute(std::make_shared<ArraySymbol>("sections", sectionsStruct));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_NULL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_LOAD", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_DYNAMIC", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_INTERP", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_NOTE", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_SHLIB", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_PHDR", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_TLS", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_GNU_EH_FRAME", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PT_GNU_STACK", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_NULL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_NEEDED", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_PLTRELSZ", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_PLTGOT", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_HASH", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_STRTAB", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_SYMTAB", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_RELA", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_RELASZ", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_RELAENT", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_STRSZ", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_SYMENT", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_INIT", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_FINI", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_SONAME", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_RPATH", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_SYMBOLIC", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_REL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_RELSZ", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_RELENT", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_PLTREL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_DEBUG", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_TEXTREL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_JMPREL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_BIND_NOW", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_INIT_ARRAY", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_FINI_ARRAY", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_INIT_ARRAYSZ", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_FINI_ARRAYSZ", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_RUNPATH", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_FLAGS", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("DT_ENCODING", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STT_NOTYPE", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STT_OBJECT", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STT_FUNC", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STT_SECTION", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STT_FILE", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STT_COMMON", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STT_TLS", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STB_LOCAL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STB_GLOBAL", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("STB_WEAK", Type::Int));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PF_X", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PF_W", Type::Int));
	elfStruct->addAttribute(std::make_shared<ValueSymbol>("PF_R", Type::Int));

	auto segmentsStruct = std::make_shared<StructureSymbol>("segments");
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("type", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("flags", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("offset", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("virtual_address", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("physical_address", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("file_size", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("memory_size", Type::Int));
	segmentsStruct->addAttribute(std::make_shared<ValueSymbol>("alignment", Type::Int));
	elfStruct->addAttribute(std::make_shared<ArraySymbol>("segments", segmentsStruct));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("dynamic_section_entries", Type::Int));
	auto dynamicStruct = std::make_shared<StructureSymbol>("dynamic");
	dynamicStruct->addAttribute(std::make_shared<ValueSymbol>("type", Type::Int));
	dynamicStruct->addAttribute(std::make_shared<ValueSymbol>("val", Type::Int));
	elfStruct->addAttribute(std::make_shared<ArraySymbol>("dynamic", dynamicStruct));

	elfStruct->addAttribute(std::make_shared<ValueSymbol>("symtab_entries", Type::Int));
	auto symtabStruct = std::make_shared<StructureSymbol>("symtab");
	symtabStruct->addAttribute(std::make_shared<ValueSymbol>("name", Type::String));
	symtabStruct->addAttribute(std::make_shared<ValueSymbol>("value", Type::Int));
	symtabStruct->addAttribute(std::make_shared<ValueSymbol>("size", Type::Int));
	symtabStruct->addAttribute(std::make_shared<ValueSymbol>("type", Type::Int));
	symtabStruct->addAttribute(std::make_shared<ValueSymbol>("bind", Type::Int));
	symtabStruct->addAttribute(std::make_shared<ValueSymbol>("shndx", Type::Int));
	elfStruct->addAttribute(std::make_shared<ArraySymbol>("symtab", symtabStruct));

	elfStruct->addAttribute(std::make_unique<FunctionSymbol>("symtab_symbol", Type::Int, Type::String));
	elfStruct->addAttribute(std::make_unique<FunctionSymbol>("symtab_symbol", Type::Int, Type::Regexp));

	_structure = elfStruct;
	return true;
}

}
