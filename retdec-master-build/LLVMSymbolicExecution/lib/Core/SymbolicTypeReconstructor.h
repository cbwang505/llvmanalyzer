/**
* @file include/retdec/bin2llvmir/optimizations/constants/constants.h
* @brief Composite type reconstruction analysis.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_BIN2LLVMIR_OPTIMIZATIONS_SYMBOLICTYPERECONSTRUCTOR_H
#define RETDEC_BIN2LLVMIR_OPTIMIZATIONS_SYMBOLICTYPERECONSTRUCTOR_H

#include <set>
#include <unordered_set>
#include <vector>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>

#include "klee/Core/Interpreter.h"

#include "retdec/bin2llvmir/providers/abi/abi.h"
#include "retdec/bin2llvmir/providers/config.h"
#include "retdec/bin2llvmir/providers/debugformat.h"
#include "retdec/bin2llvmir/providers/fileimage.h"
#include "retdec/bin2llvmir/optimizations/decoder/decoder.h"
#include "Support/RetdecInterpreterHandler.h"


namespace retdec {
	namespace bin2llvmir {

		class SymbolicTypeReconstructor : public llvm::ModulePass
		{
		public:
			static char ID;
			SymbolicTypeReconstructor();
			virtual bool runOnModule(llvm::Module& m) override;
			klee::PVatbleStructType forkVatbleStructType(retdec::common::Address vtaddr);
			bool isAsmProgramCounter(llvm::Value* val);
			bool SkipAsmProgramCounter(llvm::Instruction* i);
			common::Address GetPairFunctionAddress(llvm::Instruction* insert_before);
			void PrepareInitialization(llvm::Module* M);
			bool run_on_module_custom(
				llvm::Module& m,
				Config* c,
				Abi* a,
				FileImage* i,
				DebugFormat* d);
			bool doInitialization(llvm::Module&) override;
			bool doFinalization(llvm::Module&) override;
		private:
			bool run();
			

		private:
			bool InitializedPass = false;
			klee::Interpreter *interpreter = nullptr;
			klee::RetdecInterpreterHandler* interpreterHandlerPtr = nullptr;
			std::vector<const common::Vtable*> vtable;
			std::map < retdec::common::Address, llvm::SmallVector<retdec::common::Address, 256>> vtbl2addrs;
			llvm::Module * _module = nullptr;
			Config* _config = nullptr;
			Abi* _abi = nullptr;
			FileImage* _image = nullptr;
			DebugFormat* _dbgf = nullptr;
			std::map<retdec::common::Address, klee::PVatbleStructType> addr2sct;
			std::unordered_set<llvm::Value*> _toRemove;
		};

	} // namespace bin2llvmir
} // namespace retdec

#endif
