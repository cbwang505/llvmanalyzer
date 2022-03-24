
if(NOT TARGET retdec::macho-extractor)
    find_package(retdec 4.0
        REQUIRED
        COMPONENTS
            utils
            rapidjson
            llvm
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-macho-extractor-targets.cmake)
endif()
