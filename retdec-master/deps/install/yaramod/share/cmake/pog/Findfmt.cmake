if(UNIX)
	find_package(PkgConfig QUIET)
	pkg_check_modules(PKG_FMT QUIET fmt)
endif()

find_path(
	FMT_INCLUDE_DIR
	NAMES
		format.h
	HINTS
		${PKG_FMT_INCLUDEDIR}
	PATH_SUFFIXES
		fmt
)

find_library(
	FMT_LIBRARY
	NAMES
		fmt
	HINTS
		${PKG_FMT_LIBDIR}
	PATH_SUFFIXES
		lib
		lib64
)

mark_as_advanced(FMT_INCLUDE_DIR FMT_LIBRARY)

if(FMT_INCLUDE_DIR AND FMT_LIBRARY)
	set(FMT_FOUND 1)

	if(NOT TARGET fmt::fmt)
		add_library(fmt::fmt UNKNOWN IMPORTED)
		set_target_properties(fmt::fmt PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${FMT_INCLUDE_DIR}"
			IMPORTED_LOCATION "${FMT_LIBRARY}"
		)
	endif()
endif()

find_package_handle_standard_args(
	FMT
	REQUIRED_VARS
		FMT_INCLUDE_DIR
		FMT_LIBRARY
)

