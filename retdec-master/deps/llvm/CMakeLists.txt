
find_package(Threads REQUIRED)
if(UNIX OR MINGW)
	find_package(ZLIB REQUIRED)
endif()

# Set the default build type to 'Release'
if(NOT CMAKE_BUILD_TYPE)
	set(default_build_type "Release")
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(IS_DEBUG_BUILD "YES")
else()
	set(IS_DEBUG_BUILD "NO")
endif()

if(RETDEC_MSVC_STATIC_RUNTIME)
	set(RT_TYPE_RELEASE "MT")
	set(RT_TYPE_DEBUG "MT")
else()
	set(RT_TYPE_RELEASE "MD")
	set(RT_TYPE_DEBUG "MDd")
endif()

set(LLVM_INSTALL_DIR ${CMAKE_BINARY_DIR}/deps/install/llvm)

if(LLVM_LOCAL_DIR)
	message(STATUS "LLVM: using local LLVM directory.")
	set(LLVM_SOURCE_DIR "${LLVM_LOCAL_DIR}")
	set(_LLVM_URL "")
else()
	message(STATUS "LLVM: using remote LLVM revision.")
	set(LLVM_SOURCE_DIR "")
	set(_LLVM_URL "${LLVM_URL}")
endif()

ExternalProject_Add(llvm-project
	SOURCE_DIR ${LLVM_SOURCE_DIR}	
)
if(LLVM_LOCAL_DIR)
	force_configure_step(llvm-project)
endif()

check_if_variable_changed(LLVM_LOCAL_DIR CHANGED)
if(CHANGED)
	ExternalProject_Get_Property(llvm-project binary_dir)
	message(STATUS "LLVM: path to LLVM directory changed -> cleaning CMake files in ${binary_dir}.")
	clean_cmake_files(${binary_dir})
endif()

set(LLVM_LIB_LIST
	LLVMDebugInfoDWARF
	LLVMBitWriter
	LLVMIRReader
	LLVMObject
	LLVMBinaryFormat
	LLVMInstCombine
	LLVMSupport
	LLVMDemangle
	LLVMipo
	LLVMAsmParser
	LLVMBitReader
	LLVMMCParser
	LLVMCodeGen
	LLVMScalarOpts
	LLVMTransformUtils
	LLVMAnalysis
	LLVMTarget
	LLVMCore
	LLVMMC
	LLVMObject
	LLVMPasses
	LLVMX86AsmParser
	LLVMX86AsmPrinter
	LLVMX86Desc
	LLVMX86Disassembler
	LLVMX86Info
	LLVMX86Utils
	LLVMAsmPrinter
	LLVMGlobalISel
	LLVMDebugInfoCodeView
	LLVMMCDisassembler
	LLVMInstrumentation
	LLVMProfileData
	LLVMSelectionDAG
	LLVMVectorize
	LLVMX86CodeGen
	LLVMExecutionEngine
	LLVMRuntimeDyld
	LLVMMCJIT
)

ExternalProject_Get_Property(llvm-project source_dir)

# Create target.

add_library(llvm INTERFACE)
add_library(retdec::deps::llvm ALIAS llvm)
add_dependencies(llvm llvm-project)

target_include_directories(llvm
	SYSTEM INTERFACE
		$<BUILD_INTERFACE:${source_dir}/include>
		$<BUILD_INTERFACE:${LLVM_INSTALL_DIR}/include>
		$<INSTALL_INTERFACE:${RETDEC_INSTALL_DEPS_INCLUDE_DIR}>
)

foreach(LLVM_LIB ${LLVM_LIB_LIST})
	set(LLVM_LIB_NAME  ${CMAKE_STATIC_LIBRARY_PREFIX}${LLVM_LIB}${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(LLVM_LIB_PNAME ${CMAKE_STATIC_LIBRARY_PREFIX}retdec-${LLVM_LIB}${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(LLVM_LIB       ${LLVM_INSTALL_DIR}/lib/${LLVM_LIB_NAME})

	target_link_libraries(llvm INTERFACE
		$<BUILD_INTERFACE:${LLVM_LIB}>
	)

	# Install libs.
	install(
		FILES       ${LLVM_LIB}
		DESTINATION ${RETDEC_INSTALL_LIB_DIR}
		RENAME      ${LLVM_LIB_PNAME}
		OPTIONAL
	)

	list(APPEND LLVM_LIBS_PATHS "${RETDEC_INSTALL_LIB_DIR_ABS}/${LLVM_LIB_PNAME}")
endforeach(LLVM_LIB)

target_link_libraries(llvm
	INTERFACE
		$<INSTALL_INTERFACE:retdec::deps::llvm-libs>
		Threads::Threads
)

if(UNIX)
	set(EXECINFO "")
	if (${CMAKE_SYSTEM_NAME} MATCHES "BSD")
		set(EXECINFO "execinfo")
	endif()
	target_link_libraries(llvm
		INTERFACE
			${ZLIB_LIBRARIES}
			${CMAKE_DL_LIBS}
			${EXECINFO}
	)
elseif(MINGW)
	target_link_libraries(llvm INTERFACE ${ZLIB_LIBRARIES})
endif()

# Install includes.
install(
	DIRECTORY
		${LLVM_INSTALL_DIR}/include/
	DESTINATION
		${RETDEC_INSTALL_DEPS_INCLUDE_DIR}
)

# Install targets.
install(TARGETS llvm
	EXPORT llvm-targets
)

# Export targets.
install(EXPORT llvm-targets
	FILE "retdec-llvm-targets.cmake"
	NAMESPACE retdec::deps::
	DESTINATION ${RETDEC_INSTALL_CMAKE_DIR}
)

# Configure config file.
configure_package_config_file(
	"retdec-llvm-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/retdec-llvm-config.cmake"
	INSTALL_DESTINATION ${RETDEC_INSTALL_CMAKE_DIR}
	PATH_VARS
		LLVM_LIBS_PATHS
)

# Install CMake files.
install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/retdec-llvm-config.cmake"
	DESTINATION
		"${RETDEC_INSTALL_CMAKE_DIR}"
)
