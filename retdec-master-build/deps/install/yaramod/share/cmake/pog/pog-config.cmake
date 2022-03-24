
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was pog-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set(POG_BUNDLED_RE2 ON)
set(POG_BUNDLED_FMT ON)

if(POG_BUNDLED_RE2)
	find_package(Threads REQUIRED)
	add_library(re2::re2 STATIC IMPORTED)
	set_target_properties(re2::re2 PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES ${PACKAGE_PREFIX_DIR}/include/pog
		IMPORTED_LOCATION ${PACKAGE_PREFIX_DIR}/lib/pog_re2.lib
	)
	target_link_libraries(re2::re2 INTERFACE Threads::Threads)
else()
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
	find_package(re2 REQUIRED)
endif()

if(POG_BUNDLED_FMT)
	add_library(fmt::fmt STATIC IMPORTED)
	set_target_properties(fmt::fmt PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES ${PACKAGE_PREFIX_DIR}/include/pog
		IMPORTED_LOCATION ${PACKAGE_PREFIX_DIR}/lib//pog_fmt.lib
	)
else()
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
	find_package(fmt REQUIRED)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/pog-targets.cmake)
check_required_components(pog)
