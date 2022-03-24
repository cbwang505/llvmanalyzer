#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LLVMANALYZER_LIB)
#  define LLVMANALYZER_EXPORT Q_DECL_EXPORT
# else
#  define LLVMANALYZER_EXPORT Q_DECL_IMPORT
# endif
#else
# define LLVMANALYZER_EXPORT
#endif
