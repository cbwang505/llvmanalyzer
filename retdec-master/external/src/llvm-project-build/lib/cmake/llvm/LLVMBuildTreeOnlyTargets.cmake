# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget FileCheck llvm-PerfectShuffle count not yaml-bench gtest gtest_main KillTheDoctor)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target FileCheck
add_executable(FileCheck IMPORTED)

# Create imported target llvm-PerfectShuffle
add_executable(llvm-PerfectShuffle IMPORTED)

# Create imported target count
add_executable(count IMPORTED)

# Create imported target not
add_executable(not IMPORTED)

# Create imported target yaml-bench
add_executable(yaml-bench IMPORTED)

# Create imported target gtest
add_library(gtest STATIC IMPORTED)

set_target_properties(gtest PROPERTIES
  INTERFACE_LINK_LIBRARIES "LLVMSupport"
)

# Create imported target gtest_main
add_library(gtest_main STATIC IMPORTED)

set_target_properties(gtest_main PROPERTIES
  INTERFACE_LINK_LIBRARIES "gtest;LLVMSupport"
)

# Create imported target KillTheDoctor
add_executable(KillTheDoctor IMPORTED)

# Import target "FileCheck" for configuration "Debug"
set_property(TARGET FileCheck APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(FileCheck PROPERTIES
  IMPORTED_LOCATION_DEBUG "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Debug/bin/FileCheck.exe"
  )

# Import target "llvm-PerfectShuffle" for configuration "Debug"
set_property(TARGET llvm-PerfectShuffle APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(llvm-PerfectShuffle PROPERTIES
  IMPORTED_LOCATION_DEBUG "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Debug/bin/llvm-PerfectShuffle.exe"
  )

# Import target "count" for configuration "Debug"
set_property(TARGET count APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(count PROPERTIES
  IMPORTED_LOCATION_DEBUG "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Debug/bin/count.exe"
  )

# Import target "not" for configuration "Debug"
set_property(TARGET not APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(not PROPERTIES
  IMPORTED_LOCATION_DEBUG "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Debug/bin/not.exe"
  )

# Import target "yaml-bench" for configuration "Debug"
set_property(TARGET yaml-bench APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(yaml-bench PROPERTIES
  IMPORTED_LOCATION_DEBUG "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Debug/bin/yaml-bench.exe"
  )

# Import target "gtest" for configuration "Debug"
set_property(TARGET gtest APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(gtest PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Debug/lib/gtest.lib"
  )

# Import target "gtest_main" for configuration "Debug"
set_property(TARGET gtest_main APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(gtest_main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Debug/lib/gtest_main.lib"
  )

# Import target "KillTheDoctor" for configuration "Debug"
set_property(TARGET KillTheDoctor APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(KillTheDoctor PROPERTIES
  IMPORTED_LOCATION_DEBUG "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Debug/bin/KillTheDoctor.exe"
  )

# Import target "FileCheck" for configuration "Release"
set_property(TARGET FileCheck APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(FileCheck PROPERTIES
  IMPORTED_LOCATION_RELEASE "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Release/bin/FileCheck.exe"
  )

# Import target "llvm-PerfectShuffle" for configuration "Release"
set_property(TARGET llvm-PerfectShuffle APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(llvm-PerfectShuffle PROPERTIES
  IMPORTED_LOCATION_RELEASE "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Release/bin/llvm-PerfectShuffle.exe"
  )

# Import target "count" for configuration "Release"
set_property(TARGET count APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(count PROPERTIES
  IMPORTED_LOCATION_RELEASE "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Release/bin/count.exe"
  )

# Import target "not" for configuration "Release"
set_property(TARGET not APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(not PROPERTIES
  IMPORTED_LOCATION_RELEASE "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Release/bin/not.exe"
  )

# Import target "yaml-bench" for configuration "Release"
set_property(TARGET yaml-bench APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(yaml-bench PROPERTIES
  IMPORTED_LOCATION_RELEASE "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Release/bin/yaml-bench.exe"
  )

# Import target "gtest" for configuration "Release"
set_property(TARGET gtest APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gtest PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Release/lib/gtest.lib"
  )

# Import target "gtest_main" for configuration "Release"
set_property(TARGET gtest_main APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gtest_main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Release/lib/gtest_main.lib"
  )

# Import target "KillTheDoctor" for configuration "Release"
set_property(TARGET KillTheDoctor APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KillTheDoctor PROPERTIES
  IMPORTED_LOCATION_RELEASE "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/Release/bin/KillTheDoctor.exe"
  )

# Import target "FileCheck" for configuration "MinSizeRel"
set_property(TARGET FileCheck APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(FileCheck PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/MinSizeRel/bin/FileCheck.exe"
  )

# Import target "llvm-PerfectShuffle" for configuration "MinSizeRel"
set_property(TARGET llvm-PerfectShuffle APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(llvm-PerfectShuffle PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/MinSizeRel/bin/llvm-PerfectShuffle.exe"
  )

# Import target "count" for configuration "MinSizeRel"
set_property(TARGET count APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(count PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/MinSizeRel/bin/count.exe"
  )

# Import target "not" for configuration "MinSizeRel"
set_property(TARGET not APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(not PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/MinSizeRel/bin/not.exe"
  )

# Import target "yaml-bench" for configuration "MinSizeRel"
set_property(TARGET yaml-bench APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(yaml-bench PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/MinSizeRel/bin/yaml-bench.exe"
  )

# Import target "gtest" for configuration "MinSizeRel"
set_property(TARGET gtest APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(gtest PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/MinSizeRel/lib/gtest.lib"
  )

# Import target "gtest_main" for configuration "MinSizeRel"
set_property(TARGET gtest_main APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(gtest_main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/MinSizeRel/lib/gtest_main.lib"
  )

# Import target "KillTheDoctor" for configuration "MinSizeRel"
set_property(TARGET KillTheDoctor APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(KillTheDoctor PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/MinSizeRel/bin/KillTheDoctor.exe"
  )

# Import target "FileCheck" for configuration "RelWithDebInfo"
set_property(TARGET FileCheck APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(FileCheck PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/RelWithDebInfo/bin/FileCheck.exe"
  )

# Import target "llvm-PerfectShuffle" for configuration "RelWithDebInfo"
set_property(TARGET llvm-PerfectShuffle APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(llvm-PerfectShuffle PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/RelWithDebInfo/bin/llvm-PerfectShuffle.exe"
  )

# Import target "count" for configuration "RelWithDebInfo"
set_property(TARGET count APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(count PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/RelWithDebInfo/bin/count.exe"
  )

# Import target "not" for configuration "RelWithDebInfo"
set_property(TARGET not APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(not PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/RelWithDebInfo/bin/not.exe"
  )

# Import target "yaml-bench" for configuration "RelWithDebInfo"
set_property(TARGET yaml-bench APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(yaml-bench PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/RelWithDebInfo/bin/yaml-bench.exe"
  )

# Import target "gtest" for configuration "RelWithDebInfo"
set_property(TARGET gtest APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(gtest PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/RelWithDebInfo/lib/gtest.lib"
  )

# Import target "gtest_main" for configuration "RelWithDebInfo"
set_property(TARGET gtest_main APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(gtest_main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/RelWithDebInfo/lib/gtest_main.lib"
  )

# Import target "KillTheDoctor" for configuration "RelWithDebInfo"
set_property(TARGET KillTheDoctor APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(KillTheDoctor PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "E:/git/WindowsResearch/retdec-master/external/src/llvm-project-build/RelWithDebInfo/bin/KillTheDoctor.exe"
  )

# Make sure the targets which have been exported in some other 
# export set exist.
unset(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)
foreach(_target "LLVMSupport" )
  if(NOT TARGET "${_target}" )
    set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets "${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets} ${_target}")
  endif()
endforeach()

if(DEFINED ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)
  if(CMAKE_FIND_PACKAGE_NAME)
    set( ${CMAKE_FIND_PACKAGE_NAME}_FOUND FALSE)
    set( ${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "The following imported targets are referenced, but are missing: ${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets}")
  else()
    message(FATAL_ERROR "The following imported targets are referenced, but are missing: ${${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets}")
  endif()
endif()
unset(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE_targets)

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)