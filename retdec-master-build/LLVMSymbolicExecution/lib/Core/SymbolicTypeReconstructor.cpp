#include <cassert>
#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "SymbolicTypeReconstructor.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>
#include <retdec/llvmir2hll/support/types.h>

#include "Context.h"
#include "retdec/utils/string.h"
#include "retdec/utils/time.h"
#include "retdec/bin2llvmir/analyses/symbolic_tree.h"
#include "retdec/bin2llvmir/providers/asm_instruction.h"
#include "retdec/bin2llvmir/utils/llvm.h"
#include "retdec/bin2llvmir/utils/ir_modifier.h"
#include "retdec/utils/io/log.h"
#include "../lib/Support/RetdecInterpreterHandler.h"
#include "klee/Support/ErrorHandling.h"

using namespace retdec::utils;
using namespace llvm;
using namespace klee;

namespace retdec {
	namespace bin2llvmir {

		char SymbolicTypeReconstructor::ID = 0;
		static RegisterPass<SymbolicTypeReconstructor> X(
			"retdec-symbolic-type-reconstructor",
			"Symbolic type reconstructor optimization",
			false, // Only looks at CFG
			false // Analysis Pass
		);

		SymbolicTypeReconstructor::SymbolicTypeReconstructor() :
			ModulePass(ID)
		{
		}

		bool SymbolicTypeReconstructor::runOnModule(llvm::Module& m)
		{
			if (!InitializedPass)
			{
				PrepareInitialization(&m);
				InitializedPass = true;
			}

			for (auto kv : vtbl2addrs)
			{
				klee::PVatbleStructType classtp = forkVatbleStructType(kv.first);
				addr2sct.emplace(kv.first, classtp);
				SmallVector<llvm::Function*, 256> funclist;
				for (auto addr : kv.second)
				{
					interpreterHandlerPtr->setCurrentVatbleStructType(classtp);
					llvm::Function* func = _config->getLlvmFunction(addr);
					char * argv = nullptr;
					char * env = nullptr;
					interpreter->runFunctionAsMain(func, 0, &argv, &env);
					funclist.emplace_back(func);
				}
				if(interpreterHandlerPtr->FinalizeStructType())
				{
					klee_message("Produced Finalized StructType Vtable At:=> 0x%x ,Element Count:=> %d", kv.first.getValue(), classtp->typesct->size());

					StructType*	sct = classtp->sct;
					//f.dump();
					std::string  ecxName = "ecx";
					std::string  ecxNameGlobal = "gl_ecx";
					std::string StructTypeName = sct->getName();
					llvm::Type* poitp = PointerType::getUnqual(sct);
					llvm::Value* fakercx = Context::getRegisterRcx(_abi);
					auto* stgv = _config->_module->getOrInsertGlobal(StructTypeName, sct);
					if(!stgv)
					{
						continue;
					}
					for (llvm::Function* func : funclist)
					{
						Instruction* insert_before = nullptr;
						Instruction& checkAsmProgramCounter = func->getEntryBlock().front();
						if (SkipAsmProgramCounter(&checkAsmProgramCounter))
						{
							insert_before = checkAsmProgramCounter.getNextNode();
						}
						else
						{
							insert_before = &checkAsmProgramCounter;
						}
						llvm::LoadInst* ld = new LoadInst(fakercx, "loadecxptr", insert_before);
						llvm::LoadInst* ld2 = new LoadInst(stgv, "loadecxval");
						ld2->insertAfter(ld);
						IntToPtrInst* i2p = new IntToPtrInst(ld, poitp);
						i2p->insertAfter(ld2);
						llvm::StoreInst* st = new StoreInst(ld2, i2p);
						st->insertAfter(i2p);

					}
				}
			}

			return true;
		}
		klee::PVatbleStructType SymbolicTypeReconstructor::forkVatbleStructType( retdec::common::Address vtaddr)
		{
			std::string varName = names::generateVtableName(vtaddr);
			std::string typeName = varName + "_type";
			std::string StructTypeName = "Class_" + varName + "_type";
			llvm::StructType*	sct = llvm::StructType::create(_config->_module->getContext(), StructTypeName);
			llvm::SmallVector<POverlappedEmulatedType, 256> typesctval;
			return  new VatbleStructType
			{
				vtaddr,
				StructTypeName,
				sct,
				std::make_unique<llvm::SmallVector<POverlappedEmulatedType, 256>>(typesctval)
			};
		}
		bool SymbolicTypeReconstructor::isAsmProgramCounter(llvm::Value* val) {

			const llvm::StringRef valueName = val->getName();
			return  valueName.find("asm_program_counter") != std::string::npos;
		}
		bool SymbolicTypeReconstructor::SkipAsmProgramCounter(llvm::Instruction*i)
		{
			auto* store_inst = llvm::dyn_cast<llvm::StoreInst>(i);
			if (store_inst)
			{
				auto val = store_inst->getValueOperand();
				auto ptr = store_inst->getPointerOperand();
				if (isAsmProgramCounter(ptr))
				{
					return true;
				}
			}
			return false;
		}
		common::Address SymbolicTypeReconstructor::GetPairFunctionAddress(llvm::Instruction* insert_before)
		{
			if (SkipAsmProgramCounter(insert_before))
			{
				Instruction* checkcall = insert_before->getNextNode();
				if (CallInst* callfun = dyn_cast_or_null<CallInst>(checkcall))
				{

					auto* funcReal = callfun->getCalledFunction();
					if (funcReal)
					{
						common::Address startReal = _config->getFunctionAddress(funcReal);

						return startReal;
					}

				}
			}
			return common::Address(0);
		}
		void SymbolicTypeReconstructor::PrepareInitialization(llvm::Module* M)
		{
			FileImage* image;
			LLVMContext& ctx = M->getContext();
			if (!_config)
			{
				const Interpreter::InterpreterOptions opt;				
				interpreterHandlerPtr = new RetdecInterpreterHandler(M);
				interpreter = Interpreter::create(ctx, opt, interpreterHandlerPtr);
				_config = ConfigProvider::getConfig(M);
				_module = _config->_module;
				interpreter->initSingleModule(M);
				DataLayout TD = _module->getDataLayout();
				Context::initialize(TD.isLittleEndian(),
				                    static_cast<Expr::Width>(TD.getPointerSizeInBits()));
				Module::FunctionListType& fl = _module->getFunctionList();
			}
			if (!_abi)
			{
				_abi = AbiProvider::getAbi(M);
			}

			if (!FileImageProvider::getFileImage(_config->_module, image))
			{
				LOG << "[ABORT] config file is not available\n";

			}

			for (auto& p : image->getRtti().getVtablesGcc())
			{
				vtable.push_back(&p.second);
			}
			for (auto& p : image->getRtti().getVtablesMsvc())
			{
				vtable.push_back(&p.second);
			}

			for (auto* p : vtable)
			{
				auto& vt = *p;
				llvmir2hll::Address vtaddr = vt.getAddress();
				llvm::SmallVector<retdec::common::Address, 256> paddrs;
				for (auto& item : vt.items)
				{
					common::Address compareAddrFunc = item.getTargetFunctionAddress();
					llvm::Function* f= _config->getLlvmFunction(compareAddrFunc);
					Instruction& checkAsmProgramCounter = f->getEntryBlock().front();
					common::Address  pairAddr = GetPairFunctionAddress(&checkAsmProgramCounter);
					if (pairAddr.isDefined() && pairAddr != 0)
					{			
						
						paddrs.emplace_back(pairAddr);
						
					}else
					{
						paddrs.emplace_back(compareAddrFunc);
					}

					

				}
				vtbl2addrs.emplace(vtaddr, paddrs);
			}

		}

		bool SymbolicTypeReconstructor::doInitialization(llvm::Module& m)
		{
		
			
			return true;
		}

		bool SymbolicTypeReconstructor::doFinalization(llvm::Module& m)
		{
			return true;
		}
	}
}
