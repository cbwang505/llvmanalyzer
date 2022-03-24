cmake_minimum_required(VERSION 3.8)
project(pog
	LANGUAGES CXX
	VERSION 0.5.2
)

## Module path for custom finders.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

## Options.
option(POG_BUNDLED_RE2  "Use bundled re2"                   OFF)
option(POG_BUNDLED_FMT  "Use bundled fmt"                   OFF)
option(POG_EXAMPLES     "Build examples"                    OFF)
option(POG_TESTS        "Build tests"                       OFF)
option(POG_COVERAGE     "Enable coverage"                   OFF)
option(POG_PIC          "Enable position independent code"  OFF)

## Includes.
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

## Require C++17 support.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Variables.
set(POG_INCLUDE_DIR          "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(POG_INSTALL_CMAKE_DIR    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/cmake/pog")
set(POG_INSTALL_LIB_DIR      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(POG_INSTALL_INCLUDE_DIR  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
set(POG_CMAKE_CONFIG_FILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/share/pog-config.cmake.in")
set(POG_CMAKE_CONFIG_FILE    "${CMAKE_CURRENT_BINARY_DIR}/share/pog-config.cmake")
set(POG_CMAKE_VERSION_FILE   "${CMAKE_CURRENT_BINARY_DIR}/share/pog-config-version.cmake")
set(POG_PC_CONFIG_FILE_IN    "${CMAKE_CURRENT_SOURCE_DIR}/share/pog.pc.in")
set(POG_PC_CONFIG_FILE       "${CMAKE_CURRENT_BINARY_DIR}/share/pog.pc")
set(POG_CMAKE_INSTALL_FILES  "${POG_CMAKE_CONFIG_FILE}" "${POG_CMAKE_VERSION_FILE}")
set(POG_PC_INSTALL_FILES     "${POG_PC_CONFIG_FILE}")

## Requirements
### Threads - because of RE2
find_package(Threads REQUIRED)

## If bundled RE2 is not being used, try to find it using find_package().
if(NOT POG_BUNDLED_RE2)
	find_package(re2 REQUIRED)
	if(RE2_FOUND)
		message(STATUS "RE2 include directory: ${RE2_INCLUDE_DIR}")
		message(STATUS "RE2 library: ${RE2_LIBRARY}")
	endif()
	## Install our custom finder in case user system does not contain one.
	list(APPEND POG_CMAKE_INSTALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Findre2.cmake")
endif()

## If bundled fmt is not being used, try to find it using find_package().
if(NOT POG_BUNDLED_FMT)
	find_package(fmt REQUIRED)
	if(fmt_FOUND)
		message(STATUS "Found libfmt")
	endif()
else()
	## Install our custom finder in case user system does not contain one.
	list(APPEND POG_CMAKE_INSTALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fmt/Findfmt.cmake")
endif()

## Add deps folder to resolve bundled dependencies.
add_subdirectory(deps)

## Library.
add_library(pog INTERFACE)
target_sources(pog INTERFACE ${POG_FILES})
if(MSVC)
	target_compile_options(pog INTERFACE /W3)
else()
	target_compile_options(pog INTERFACE -Wall -Wextra -Wno-empty-body)
endif()
target_include_directories(pog INTERFACE
	"$<BUILD_INTERFACE:${POG_INCLUDE_DIR}>"
	"$<INSTALL_INTERFACE:${POG_INSTALL_INCLUDE_DIR}>"
)
target_link_libraries(pog INTERFACE re2::re2 fmt::fmt)
if(POG_COVERAGE)
	target_compile_options(pog INTERFACE --coverage)
	target_link_libraries(pog INTERFACE --coverage)
endif()

## Build examples if requested.
if(POG_EXAMPLES)
	add_subdirectory(examples)
endif()

## Build tests if requested.
if(POG_TESTS)
	add_subdirectory(tests)
endif()

## We need to set custom RE2 include directory and library path if bundled dependencies are used.
## Also we need to set proper requirements for pkg-config file.
if(POG_BUNDLED_RE2)
	set(RE2_INCLUDE_DIR "${POG_INSTALL_INCLUDE_DIR}/pog")
	set(RE2_LIBRARY "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}pog_re2${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set(POG_PC_REQUIREMENT "Libs: -L\$\{libdir\} -lpog_re2 -lpthread")
else()
	set(RE2_INCLUDE_DIR "<NOT_SET>")
	set(RE2_LIBRARY "<NOT_SET>")
	set(POG_PC_REQUIREMENT "Requires: re2\nLibs: -L\$\{libdir\}")
endif()

## Handle also fmt in pkg-config file.
if(POG_BUNDLED_FMT)
	set(FMT_INCLUDE_DIR "${POG_INSTALL_INCLUDE_DIR}/pog")
	set(FMT_LIBRARY "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}//${CMAKE_STATIC_LIBRARY_PREFIX}pog_fmt${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set(POG_PC_REQUIREMENT "${POG_PC_REQUIREMENT} -lpog_fmt")
else()
	set(FMT_INCLUDE_DIR "<NOT_SET>")
	set(FMT_LIBRARY "<NOT_SET>")
	set(POG_PC_REQUIREMENT "${POG_PC_REQUIREMENT} -lfmt")
endif()

## Create pog-config.cmake
configure_package_config_file(
	${POG_CMAKE_CONFIG_FILE_IN}
	${POG_CMAKE_CONFIG_FILE}
	INSTALL_DESTINATION ${POG_INSTALL_CMAKE_DIR}
	PATH_VARS
		RE2_INCLUDE_DIR RE2_LIBRARY
		FMT_INCLUDE_DIR FMT_LIBRARY
)

## Create pog-config-version.cmake
write_basic_package_version_file(
	${POG_CMAKE_VERSION_FILE}
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

## Create pog.pc
configure_file(
	${POG_PC_CONFIG_FILE_IN}
	${POG_PC_CONFIG_FILE}
	@ONLY
)

## Install.
### Include directory.
install(
	DIRECTORY ${POG_INCLUDE_DIR}/pog
	DESTINATION ${POG_INSTALL_INCLUDE_DIR}
)

### Export pog::pog target into pog-targets.cmake
install(TARGETS pog EXPORT pog)
install(
	EXPORT pog
	FILE pog-targets.cmake
	NAMESPACE pog::
	DESTINATION ${POG_INSTALL_CMAKE_DIR}
)

### Install CMake related files (cmake-config.cmake, cmake-config-version.cmake)
install(
	FILES ${POG_CMAKE_INSTALL_FILES}
	DESTINATION ${POG_INSTALL_CMAKE_DIR}
)

### Install pkg-config related files (pog.pc)
install(
	FILES ${POG_PC_CONFIG_FILE}
	DESTINATION ${POG_INSTALL_LIB_DIR}/pkgconfig
)
