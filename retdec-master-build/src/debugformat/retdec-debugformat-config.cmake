
if(NOT TARGET retdec::debugformat)
    find_package(retdec 4.0
        REQUIRED
        COMPONENTS
            demangler
            loader
            fileformat
            common
            pdbparser
            llvm
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-debugformat-targets.cmake)
endif()
