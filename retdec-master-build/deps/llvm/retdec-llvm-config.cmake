
####### Expanded from 
####### Expanded from 
####### Expanded from 
####### Expanded from 
####### Expanded from 
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was retdec-llvm-config.cmake                            ########

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

#################################################################################### by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was retdec-llvm-config.cmake                            ########

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

#################################################################################### by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was retdec-llvm-config.cmake                            ########

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

#################################################################################### by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was retdec-llvm-config.cmake                            ########

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

#################################################################################### by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was retdec-llvm-config.cmake                            ########

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

#################################################################################### by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was retdec-llvm-config.cmake                            ########

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

if(NOT TARGET llvm-libs)
	add_library(llvm-libs INTERFACE)
	add_library(retdec::deps::llvm-libs ALIAS llvm-libs)
	foreach(LLVM_LIB ${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMDebugInfoDWARF.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMBitWriter.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMIRReader.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMObject.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMBinaryFormat.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMInstCombine.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMSupport.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMDemangle.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMipo.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMAsmParser.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMBitReader.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMMCParser.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMCodeGen.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMScalarOpts.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMTransformUtils.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMAnalysis.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMTarget.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMCore.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMMC.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMObject.lib;${PACKAGE_PREFIX_DIR}/lib/retdec-LLVMPasses.lib)
		target_link_libraries(llvm-libs INTERFACE
			${LLVM_LIB}
		)
	endforeach(LLVM_LIB)
endif()

if(NOT TARGET retdec::deps::llvm)
	find_package(Threads REQUIRED)
	if(UNIX OR MINGW)
		find_package(ZLIB REQUIRED)
	endif()

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-llvm-targets.cmake)
endif()
