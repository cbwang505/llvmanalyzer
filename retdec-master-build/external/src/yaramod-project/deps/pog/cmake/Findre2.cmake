if(UNIX)
	find_package(PkgConfig QUIET)
	pkg_check_modules(PKG_RE2 QUIET re2)
endif()

find_path(
	RE2_INCLUDE_DIR
	NAMES
		re2.h
	HINTS
		${PKG_RE2_INCLUDEDIR}
	PATH_SUFFIXES
		re2
)

find_library(
	RE2_LIBRARY
	NAMES
		re2
	HINTS
		${PKG_RE2_LIBDIR}
	PATH_SUFFIXES
		lib
)

mark_as_advanced(RE2_INCLUDE_DIR RE2_LIBRARY)

if(RE2_INCLUDE_DIR AND RE2_LIBRARY)
	set(RE2_FOUND 1)

	if(NOT TARGET re2::re2)
		add_library(re2::re2 UNKNOWN IMPORTED)
		set_target_properties(re2::re2 PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${RE2_INCLUDE_DIR}"
			IMPORTED_LOCATION "${RE2_LIBRARY}"
		)
	endif()
endif()

find_package_handle_standard_args(
	RE2
	REQUIRED_VARS
		RE2_INCLUDE_DIR
		RE2_LIBRARY
)
