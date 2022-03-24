
if(NOT TARGET retdec::config)
    find_package(retdec 4.0
        REQUIRED
        COMPONENTS
            serdes
            common
            utils
            rapidjson
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-config-targets.cmake)
endif()
