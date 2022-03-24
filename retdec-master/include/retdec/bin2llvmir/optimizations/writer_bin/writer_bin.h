/**
 * @file include/retdec/bin2llvmir/optimizations/writer_dsm/writer_dsm.h
 * @brief Generate the current disassembly.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#ifndef RETDEC_BIN2LLVMIR_OPTIMIZATIONS_WRITER_DSM_WRITER_BIN_H
#define RETDEC_BIN2LLVMIR_OPTIMIZATIONS_WRITER_DSM_WRITER_BIN_H

#include "retdec/bin2llvmir/providers/abi/abi.h"

#include <llvm/Pass.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/Support/SmallVectorMemoryBuffer.h>
#include <llvm/IR/Module.h>


#include "retdec/bin2llvmir/providers/asm_instruction.h"



namespace retdec {
	namespace bin2llvmir {

		class BinWriter : public llvm::ModulePass,public llvm::JITSymbolResolver,public llvm::RuntimeDyld::MemoryManager
		{
		public:
			
			static char ID;
			BinWriter();
			~BinWriter();
			void BinWriter::setOutputBuff(std::vector<std::uint8_t>* outBuff);
			void BinWriter::setRemapSeg(uint64_t addr);
			llvm::Expected<uint64_t> BinWriter::GetSymbolAddrReal(uint64_t Offset, llvm::object::symbol_iterator& It, uint64_t secidx, llvm::StringRef& Name) const;
			void BinWriter::remapGlobalSymbolTable(std::map<llvm::StringRef, llvm::JITEvaluatedSymbol>& tbl);
			virtual bool BinWriter::runOnModule(llvm::Module& m) override;
			 llvm::JITSymbol findSymbol(const std::string &Name) ;
			 llvm::JITSymbol findSymbolInternal(const std::string &Name) ;
			 llvm::JITSymbol findSymbolInLogicalDylib(const std::string &Name) ;
			void lookup(const llvm::JITSymbolResolver::LookupSet &Symbols,
				 OnResolvedFunction OnResolved) final;
			llvm::Expected<llvm::JITSymbolResolver::LookupSet> getResponsibilitySet(const llvm::JITSymbolResolver::LookupSet &Symbols) final;
			virtual void anchor() override;
			virtual uint8_t *allocateCodeSection(uintptr_t Size, unsigned Alignment,
				unsigned SectionID,
				llvm::StringRef SectionName) override;
			virtual uint8_t *allocateDataSection(uintptr_t Size, unsigned Alignment,
				unsigned SectionID,
				llvm::StringRef SectionName,
				bool IsReadOnly) override;
			virtual void reserveAllocationSpace(uintptr_t CodeSize, uint32_t CodeAlign,
				uintptr_t RODataSize,
				uint32_t RODataAlign,
				uintptr_t RWDataSize,
				uint32_t RWDataAlign) override;
			virtual bool needsToReserveAllocationSpace() override;
			virtual void registerEHFrames(uint8_t *Addr, uint64_t LoadAddr,
				size_t Size) override;
			virtual void deregisterEHFrames() override;
			virtual bool finalizeMemory(std::string *ErrMsg = nullptr) override;
			virtual void notifyObjectLoaded(llvm::RuntimeDyld &RTDyld,
				const llvm::object::ObjectFile &Obj)  override;

		private:
			void mapRealSec(unsigned SectionID,
				llvm::StringRef SectionName, uint8_t * addr);
			void remapSecLoadAddress();
		private:
			uint64_t remap_seg_addr = 0;
			llvm::SectionMemoryManager* RTDyldMM = nullptr;
			llvm::object::ObjectFile* LoadedObjectPtr = nullptr;
			llvm::RuntimeDyld* Dyld = nullptr;
			llvm::Module* _module = nullptr;
			Config* _config = nullptr;
			std::vector<std::uint8_t>* outbuf = nullptr;
		    std::map<uint64_t,uint64_t> realSec;
		    std::map<uint64_t,uint64_t> realSecBuf;
		};
	};
};
#endif
