
if(NOT TARGET retdec::ar-extractor)
    find_package(retdec 4.0
        REQUIRED
        COMPONENTS
            utils
            llvm
            rapidjson
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-ar-extractor-targets.cmake)
endif()
