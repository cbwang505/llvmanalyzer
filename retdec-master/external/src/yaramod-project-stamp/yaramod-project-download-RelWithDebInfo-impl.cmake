set(command "C:/Program Files/CMake/bin/cmake.exe;-P;E:/git/WindowsResearch/retdec-master/external/src/yaramod-project-stamp/download-yaramod-project.cmake")

execute_process(COMMAND ${command} RESULT_VARIABLE result)
if(result)
  set(msg "Command failed (${result}):\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  message(FATAL_ERROR "${msg}")
endif()
set(command "C:/Program Files/CMake/bin/cmake.exe;-P;E:/git/WindowsResearch/retdec-master/external/src/yaramod-project-stamp/verify-yaramod-project.cmake")

execute_process(COMMAND ${command} RESULT_VARIABLE result)
if(result)
  set(msg "Command failed (${result}):\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  message(FATAL_ERROR "${msg}")
endif()
set(command "C:/Program Files/CMake/bin/cmake.exe;-P;E:/git/WindowsResearch/retdec-master/external/src/yaramod-project-stamp/extract-yaramod-project.cmake")

execute_process(COMMAND ${command} RESULT_VARIABLE result)
if(result)
  set(msg "Command failed (${result}):\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  message(FATAL_ERROR "${msg}")
endif()