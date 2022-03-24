##
## CMake build script for the yaramod library.
##

# Source files.
set(SOURCES
	builder/yara_expression_builder.cpp
	builder/yara_file_builder.cpp
	builder/yara_hex_string_builder.cpp
	builder/yara_rule_builder.cpp
	parser/parser_driver.cpp
	types/hex_string.cpp
	types/literal.cpp
	types/meta.cpp
	types/modules/androguard_module.cpp
	types/modules/cuckoo_module.cpp
	types/modules/dex_module.cpp
	types/modules/dotnet_module.cpp
	types/modules/elf_module.cpp
	types/modules/hash_module.cpp
	types/modules/macho_module.cpp
	types/modules/magic_module.cpp
	types/modules/math_module.cpp
	types/modules/module.cpp
	types/modules/pe_module.cpp
	types/modules/phish_module.cpp
	types/modules/time_module.cpp
	types/modules/metadata_module.cpp
	types/plain_string.cpp
	types/rule.cpp
	types/token.cpp
	types/token_stream.cpp
	types/yara_file.cpp
	utils/filesystem.cpp
	utils/utils.cpp
	yaramod.cpp
)

## Compile parser_driver.cpp with /bigobj because it generates too many sections
if(MSVC)
	set_source_files_properties(parser/parser_driver.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()

if(NOT TARGET yaramod)
	# Library.
	add_library(yaramod ${SOURCES})

	# Library includes and interface.
	target_include_directories(yaramod PRIVATE "${YARAMOD_INCLUDE_DIR}")
	target_include_directories(yaramod SYSTEM INTERFACE "${YARAMOD_INCLUDE_DIR}")
	# We need to be able to include the generated files from the build directory.
	# By declaring the include path as a system path, we automatically discard
	# warnings from the generated header files, which is useful.
	target_include_directories(yaramod SYSTEM PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
	target_include_directories(yaramod SYSTEM PUBLIC "${YARAMOD_DEPS_DIR}")

	# Platform dependant defines.
	if(WIN32)
		target_compile_definitions(yaramod PRIVATE -DYARAMOD_OS_WINDOWS)
	elseif(UNIX)
		target_compile_definitions(yaramod PRIVATE -DYARAMOD_OS_LINUX)
	endif()

	# Linking.
	if(WIN32)
		target_link_libraries(yaramod shlwapi)
	endif()

	# Pog library
	target_link_libraries(yaramod pog)

	# Python module.
	if(YARAMOD_PYTHON)
		add_subdirectory(python)
	endif()

	# Examples.
	if(YARAMOD_EXAMPLES)
		add_subdirectory(examples)
	endif()

	# Installation.
	install(
		TARGETS yaramod
		ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
	)
	install(
		DIRECTORY "${YARAMOD_INCLUDE_DIR}/"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}"
	)
	install(
		DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/yaramod/"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/yaramod"
		FILES_MATCHING
			PATTERN "*.hpp"
			PATTERN "*.hh"
	)
endif()
