
if(NOT TARGET retdec::unpackertool)
    find_package(retdec 4.0
        REQUIRED
        COMPONENTS
            unpacker
            loader
            cpdetect
            utils
            pelib
    )

    include(${CMAKE_CURRENT_LIST_DIR}/retdec-unpackertool-targets.cmake)
endif()
