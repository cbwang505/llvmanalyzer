//===-- ErrorHandling.cpp -------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Support/ErrorHandling.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include <set>

using namespace klee;
using namespace llvm;

namespace klee {
	cl::OptionCategory MiscCat("Miscellaneous options", "");
}
void msgap(const char *msg, va_list ap)
{
	char outmsg[_MAX_PATH];	
	vsnprintf(outmsg, _MAX_PATH, msg, ap);	
	retdec::utils::io::Log::info()<< retdec::utils::io::Log::SubSubPhase << outmsg << std::endl;
}

void klee::klee_message(const char *msg, ...) {
	
	va_list ap;
	va_start(ap, msg);
	msgap(msg, ap);
	va_end(ap);
}

/* Message to be written only to file */
void klee::klee_message_to_file(const char *msg, ...) {
	
	va_list ap;
	va_start(ap, msg);
	msgap(msg, ap);
	va_end(ap);
	
	
}

void klee::klee_error(const char *msg, ...) {
	char outmsg[_MAX_PATH];
	va_list ap;
	va_start(ap, msg);
	vsnprintf(outmsg, _MAX_PATH, msg, ap);
	va_end(ap);
	retdec::utils::io::Log::error() << retdec::utils::io::Log::Error << outmsg << std::endl;
	
	exit(1);
}

void klee::klee_warning(const char *msg, ...) {
	char outmsg[_MAX_PATH];
	va_list ap;
	va_start(ap, msg);
	vsnprintf(outmsg, _MAX_PATH, msg, ap);
	va_end(ap);
	retdec::utils::io::Log::info() << retdec::utils::io::Log::Warning << outmsg << std::endl;
	;
}

/* Prints a warning once per message. */
void klee::klee_warning_once(const void *id, const char *msg, ...) {
	static std::set<std::pair<const void *, const char *> > keys;
	std::pair<const void *, const char *> key;

	/* "calling external" messages contain the actual arguments with
	   which we called the external function, so we need to ignore them
	   when computing the key. */
	if (strncmp(msg, "calling external", strlen("calling external")) != 0)
		key = std::make_pair(id, msg);
	else
		key = std::make_pair(id, "calling external");

	if (!keys.count(key)) {
		keys.insert(key);
		va_list ap;
		va_start(ap, msg);
		msgap(msg, ap);
		va_end(ap);
	}
}
