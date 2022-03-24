//===-- ErrorHandling.h -----------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_ERRORHANDLING_H
#define KLEE_ERRORHANDLING_H

#ifdef __CYGWIN__
#ifndef WINDOWS
#define WINDOWS
#endif
#endif

#include "retdec/utils/io/log.h"

namespace klee {



/// Print "KLEE: ERROR: " followed by the msg in printf format and a
/// newline on stderr and to warnings.txt, then exit with an error.
void klee_error(const char *msg, ...);
    

/// Print "KLEE: " followed by the msg in printf format and a
/// newline on stderr and to messages.txt.
void klee_message(const char *msg, ...);

/// Print "KLEE: " followed by the msg in printf format and a
/// newline to messages.txt.
void klee_message_to_file(const char *msg, ...);
   
/// Print "KLEE: WARNING: " followed by the msg in printf format and a
/// newline on stderr and to warnings.txt.
void klee_warning(const char *msg, ...);

/// Print "KLEE: WARNING: " followed by the msg in printf format and a
/// newline on stderr and to warnings.txt. However, the warning is only
/// printed once for each unique (id, msg) pair (as pointers).
void klee_warning_once(const void *id, const char *msg, ...);
   
}

#endif /* KLEE_ERRORHANDLING_H */
