# Install script for directory: E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/fmt-config.cmake"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/fmt-config-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake"
         "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/Debug/fmtd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/Release/fmt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/MinSizeRel/fmt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/RelWithDebInfo/fmt.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/$<TARGET_PDB_FILE:fmt"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/fmt-header-only>"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fmt" TYPE FILE FILES
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/chrono.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/color.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/compile.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/core.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/format.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/format-inl.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/locale.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/ostream.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/printf.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/ranges.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/safe-duration-cast.h"
    "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project/deps/pog/deps/fmt/fmt/include/fmt/posix.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/fmt.pc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "E:/git/WindowsResearch/retdec-master-build/external/src/yaramod-project-build/deps/pog/deps/fmt/fmt/src/fmt-dep-build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
