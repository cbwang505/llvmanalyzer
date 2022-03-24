# Install script for directory: E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod")
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
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/include/pog")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/include" TYPE DIRECTORY FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/include/pog")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-targets.cmake"
         "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/CMakeFiles/Export/E_/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-targets.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog" TYPE FILE FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/CMakeFiles/Export/E_/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-targets.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-config.cmake;E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/pog-config-version.cmake;E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog/Findfmt.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/share/cmake/pog" TYPE FILE FILES
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/share/pog-config.cmake"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/share/pog-config-version.cmake"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/cmake/fmt/Findfmt.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/lib/pkgconfig/pog.pc")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "E:/git/WindowsResearch/retdec-master-build/deps/install/yaramod/lib/pkgconfig" TYPE FILE FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/share/pog.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/cmake_install.cmake")

endif()

