# Install script for directory: E:/git/WindowsResearch/retdec-master/support

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  
		if(EXISTS "C:/Program Files (x86)/retdec/share/retdec/support/version-yarac.txt")
			file(READ "C:/Program Files (x86)/retdec/share/retdec/support/version-yarac.txt" YARAC_OLD_VERSION)
			execute_process(
				COMMAND "C:/Program Files (x86)/retdec/bin/retdec-yarac.exe" --version
				OUTPUT_VARIABLE YARAC_CURRENT_VERSION
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)
			if(YARAC_OLD_VERSION STREQUAL YARAC_CURRENT_VERSION)
				message(STATUS "Up-to-date: yarac version '${YARAC_OLD_VERSION}'")
			else()
				message(STATUS "Previously used yarac version '${YARAC_OLD_VERSION}' 	does not match the current version '${YARAC_CURRENT_VERSION}' 	-> clean the support directory")
				FILE(REMOVE_RECURSE "C:/Program Files (x86)/retdec/share/retdec/support")
			endif()
		elseif(EXISTS "C:/Program Files (x86)/retdec/share/retdec/support")
			message(STATUS "yarac version does not exist 	-> clean the support directory")
			FILE(REMOVE_RECURSE "C:/Program Files (x86)/retdec/share/retdec/support")
		else()
			# Support directory itself does not yet exist.
		endif()
	
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  
		execute_process(
			# -u = unbuffered -> print debug messages right away.
			COMMAND "C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python37_64/python.exe" -u "E:/git/WindowsResearch/retdec-master/support/install-share.py" "C:/Program Files (x86)/retdec" "https://github.com/avast/retdec-support/releases/download/2019-03-08/retdec-support_2019-03-08.tar.xz" "629351609bca0f4b8edbd4e53789192305256aeb908e953f5546e121a911d54e" "2019-03-08"
			RESULT_VARIABLE INSTALL_SHARE_RES
		)
		if(INSTALL_SHARE_RES)
			message(FATAL_ERROR "RetDec share directory installation FAILED")
		endif()
	
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  
		execute_process(
			COMMAND "C:/Program Files (x86)/retdec/bin/retdec-yarac.exe" --version
			OUTPUT_VARIABLE YARAC_CURRENT_VERSION
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		message(STATUS "yarac version '${YARAC_CURRENT_VERSION}' 	written to 'C:/Program Files (x86)/retdec/share/retdec/support/version-yarac.txt'")
		file(WRITE "C:/Program Files (x86)/retdec/share/retdec/support/version-yarac.txt" "${YARAC_CURRENT_VERSION}")
	
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/retdec/share/retdec/support//ordinals")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/retdec/share/retdec/support/" TYPE DIRECTORY MESSAGE_LAZY FILES "E:/git/WindowsResearch/retdec-master/support/ordinals")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  
		execute_process(
			COMMAND "C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python37_64/python.exe" -u "E:/git/WindowsResearch/retdec-master/support/install-yara.py"
				"C:/Program Files (x86)/retdec/bin/retdec-yarac.exe"
				"C:/Program Files (x86)/retdec/share/retdec/support"
				"E:/git/WindowsResearch/retdec-master/support/yara_patterns"
				ON
			RESULT_VARIABLE INSTALL_YARA_RES
		)
		if(INSTALL_YARA_RES)
			message(FATAL_ERROR "YARA tool signatures installation FAILED")
		endif()
	
endif()

