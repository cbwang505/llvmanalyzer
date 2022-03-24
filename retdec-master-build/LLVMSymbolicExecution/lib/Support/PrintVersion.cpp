//===-- PrintVersion.cpp --------------------------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Support/PrintVersion.h"
#include "klee/Config/config.h"
#include "klee/Config/Version.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

//#include "klee/Config/CompileTimeInfo.h"


void klee::printVersion(llvm::raw_ostream &OS)
{
	llvm::cl::PrintVersionMessage();
}
