
if(NOT TARGET retdec::ctypesparser)
    find_package(retdec 4.0
        REQUIRED
        COMPONENTS
            ctypes
            utils
            rapidjson
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-ctypesparser-targets.cmake)
endif()
