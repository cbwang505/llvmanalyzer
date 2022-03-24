# Install script for directory: E:/git/WindowsResearch/retdec-master/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/retdec")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/git/WindowsResearch/retdec-master-build/src/ar-extractor/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/ar-extractortool/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/bin2llvmir/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/bin2pat/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/capstone2llvmir/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/common/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/config/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/cpdetect/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/ctypes/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/ctypesparser/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/debugformat/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/demangler/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/fileformat/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/fileinfo/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/getsig/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/idr2pat/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/llvmir2hll/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/loader/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/macho-extractor/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/macho-extractortool/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/pat2yara/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/patterngen/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/pdbparser/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/pelib/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/retdec/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/retdec-decompiler/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/rtti-finder/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/serdes/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/stacofin/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/stacofintool/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/unpacker/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/unpackertool/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/utils/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/src/yaracpp/cmake_install.cmake")

endif()

