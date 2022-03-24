##
## CMake build script for the yaramod documentation.
##

add_custom_target(doc
	WORKING_DIRECTORY "${YARAMOD_DOC_DIR}"
	COMMAND ${DOXYGEN_EXECUTABLE}
	COMMENT "Generating doxygen documentation"
)
