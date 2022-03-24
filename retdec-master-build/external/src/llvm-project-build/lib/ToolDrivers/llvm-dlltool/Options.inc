/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Option Parsing Definitions                                                 *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

/////////
// Prefixes

#ifdef PREFIX
#define COMMA ,
PREFIX(prefix_0, {nullptr})
PREFIX(prefix_2, {"-" COMMA nullptr})
PREFIX(prefix_1, {"--" COMMA nullptr})
#undef COMMA
#endif // PREFIX

/////////
// Groups

#ifdef OPTION

//////////
// Options

OPTION(prefix_0, "<input>", INPUT, Input, INVALID, INVALID, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_0, "<unknown>", UNKNOWN, Unknown, INVALID, INVALID, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_1, "as-flags", f_alias, JoinedOrSeparate, INVALID, f, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_1, "as", S_alias, JoinedOrSeparate, INVALID, S, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_1, "dllname", D_long, JoinedOrSeparate, INVALID, D, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_2, "D", D, JoinedOrSeparate, INVALID, INVALID, nullptr, 0, 0,
       "Specify the input DLL Name", nullptr, nullptr)
OPTION(prefix_2, "d", d, JoinedOrSeparate, INVALID, INVALID, nullptr, 0, 0,
       "Input .def File", nullptr, nullptr)
OPTION(prefix_2, "f", f, JoinedOrSeparate, INVALID, INVALID, nullptr, 0, 0,
       "Assembler Flags", nullptr, nullptr)
OPTION(prefix_1, "input-def", d_long, JoinedOrSeparate, INVALID, d, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_1, "kill-at", k_alias, Flag, INVALID, k, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_2, "k", k, Flag, INVALID, INVALID, nullptr, 0, 0,
       "Kill @n Symbol from export", nullptr, nullptr)
OPTION(prefix_2, "l", l, JoinedOrSeparate, INVALID, INVALID, nullptr, 0, 0,
       "Generate an import lib", nullptr, nullptr)
OPTION(prefix_1, "machine", m_long, JoinedOrSeparate, INVALID, m, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_2, "m", m, JoinedOrSeparate, INVALID, INVALID, nullptr, 0, 0,
       "Set target machine", nullptr, nullptr)
OPTION(prefix_1, "output-lib", l_long, JoinedOrSeparate, INVALID, l, nullptr, 0, 0, nullptr, nullptr, nullptr)
OPTION(prefix_2, "S", S, JoinedOrSeparate, INVALID, INVALID, nullptr, 0, 0,
       "Assembler", nullptr, nullptr)
#endif // OPTION

#ifdef OPTTABLE_ARG_INIT
//////////
// Option Values


#endif // OPTTABLE_ARG_INIT
