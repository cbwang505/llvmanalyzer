pybind11_add_module(yaramod-python
	yaramod_python.cpp
	py_visitor.cpp
)

# We need to build python extension with -O1 when using GCC because
# there is some problem with linking std::shared_ptr<BoolLiteralExpression>:
#
# undefined reference to `vtable for std::_Sp_counted_ptr_inplace<yaramod::BoolLiteralExpression, std::allocator<yaramod::BoolLiteralExpression>, (__gnu_cxx::_Lock_policy)2>'
#
# It seems like -O2 and -O3 produce some kind of different template instantiation which is not compatible
# with other compiler options use when compiling python extension.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS_RELEASE "-O1 -DNDEBUG")
endif()

set_target_properties(yaramod-python PROPERTIES OUTPUT_NAME "yaramod")
target_link_libraries(yaramod-python PUBLIC yaramod)
