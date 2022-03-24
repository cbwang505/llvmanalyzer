//===-- Context.h -----------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_CONTEXT_H
#define KLEE_CONTEXT_H

#include <retdec/bin2llvmir/providers/abi/abi.h>

#include "klee/Expr/Expr.h"

namespace klee {

  /// Context - Helper class for storing global information about a KLEE run.
  class Context {
    /// Whether the target architecture is little endian or not.
    bool IsLittleEndian;

    /// The pointer width of the target architecture.
    Expr::Width PointerWidth;

  protected:
    Context(bool _IsLittleEndian, Expr::Width _PointerWidth)
      : IsLittleEndian(_IsLittleEndian), PointerWidth(_PointerWidth) {}
    
  public:
    Context() {}

    /// initialize - Construct the global Context instance.
    static void initialize(bool IsLittleEndian, Expr::Width PointerWidth);

    /// get - Return the global singleton instance of the Context.
    static const Context &get();

    bool isLittleEndian() const { return IsLittleEndian; }

    bool is64Arch() const { return PointerWidth==64; }

	static  llvm::Value* getRegisterRcx(retdec::bin2llvmir::Abi* _abi)
    {
		return _abi->getRegister(Context::get().is64Arch() ? x86_reg::X86_REG_RCX : x86_reg::X86_REG_ECX);
    }

    /// Returns width of the pointer in bits
    Expr::Width getPointerWidth() const { return PointerWidth; }
  };
  
} // End klee namespace

#endif /* KLEE_CONTEXT_H */
