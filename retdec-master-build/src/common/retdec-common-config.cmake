
if(NOT TARGET retdec::common)
    find_package(retdec 4.0 REQUIRED
        COMPONENTS
            utils
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-common-targets.cmake)
endif()
