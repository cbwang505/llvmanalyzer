
if(NOT TARGET retdec::rtti-finder)
    find_package(retdec 4.0
        REQUIRED
        COMPONENTS
            loader
            common
            utils
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-rtti-finder-targets.cmake)
endif()
