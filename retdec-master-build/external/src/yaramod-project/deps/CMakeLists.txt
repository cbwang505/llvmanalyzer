if(YARAMOD_TESTS)
	add_subdirectory(googletest)
endif()
if(YARAMOD_PYTHON)
	add_subdirectory(pybind11)
endif()

set(POG_BUNDLED_FMT ON CACHE BOOL "Use bundled fmt" FORCE)
set(POG_BUNDLED_RE2 ON CACHE BOOL "Use bundled re2" FORCE)
set(POG_PIC         ON CACHE BOOL "Build PIC"       FORCE)

add_subdirectory(pog)
