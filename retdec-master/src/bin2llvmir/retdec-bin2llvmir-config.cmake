
if(NOT TARGET retdec::bin2llvmir)
    find_package(retdec 4.0
        REQUIRED
        COMPONENTS
            rtti-finder
            capstone2llvmir
            debugformat
            demangler
            stacofin
            cpdetect
            loader
            fileformat
            config
            ctypesparser
            common
            utils
            llvm
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-bin2llvmir-targets.cmake)
endif()
