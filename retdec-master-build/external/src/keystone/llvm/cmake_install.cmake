# Install script for directory: E:/git/WindowsResearch/retdec-master-build/external/src/keystone/llvm

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/keystone")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "E:/git/WindowsResearch/retdec-master-build/external/src/keystone/llvm/../include/keystone")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/git/WindowsResearch/retdec-master-build/external/src/keystone/llvm/lib/Support/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/external/src/keystone/llvm/include/llvm/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/external/src/keystone/llvm/lib/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/external/src/keystone/llvm/keystone/cmake_install.cmake")
  include("E:/git/WindowsResearch/retdec-master-build/external/src/keystone/llvm/cmake/modules/cmake_install.cmake")

endif()

