/**
 * @file src/bin2llvmir/optimizations/writer_dsm/writer_dsm.cpp
 * @brief Generate the current disassembly.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <llvm/ADT/Triple.h>
#include <llvm/Analysis/CallGraph.h>
#include <llvm/Analysis/CallGraphSCCPass.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/RegionPass.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LegacyPassNameParser.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/InitializePasses.h>
#include <llvm/LinkAllIR.h>
#include <llvm/MC/SubtargetFeature.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/SystemUtils.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>

#include "retdec/utils/io/log.h"
#include "retdec/bin2llvmir/optimizations/writer_bin/writer_bin.h"




using namespace retdec::common;
using namespace retdec::utils;
using namespace llvm;






namespace retdec {
	namespace bin2llvmir {

		char BinWriter::ID = 0;

		static llvm::RegisterPass<BinWriter> X(
			"retdec-write-bin",
			"Disassembly Binary generation",
			false, // Only looks at CFG
			false // Analysis Pass
		);

		BinWriter::BinWriter() :
			ModulePass(ID)
		{

		}

		BinWriter::~BinWriter() {
			if (Dyld)
			{
				Dyld->deregisterEHFrames();
				delete Dyld;
			}
			/*if(LoadedObjectPtr)
			{
				delete	LoadedObjectPtr;
			}
			if(RTDyldMM)
			{
				delete RTDyldMM;
			}*/

		}
		void BinWriter::setRemapSeg(uint64_t addr)
		{
			remap_seg_addr = addr;

		}
		void BinWriter::setOutputBuff(std::vector<std::uint8_t>* outBuff)
		{
			outbuf = outBuff;

		}
		void  BinWriter::remapSecLoadAddress()
		{
			for (auto kv : realSec)
			{
				uint64_t idaRealAddr = remap_seg_addr + kv.second;
				Dyld->mapSectionAddress(reinterpret_cast<void*>(kv.first), idaRealAddr);
			}
		}
		void BinWriter::mapRealSec(unsigned SectionID,
			llvm::StringRef SectionName, uint8_t * addr)
		{
			StringRef Name;
			for (auto sec : LoadedObjectPtr->sections())
			{
				auto err = sec.getName(Name);
				if (Name == SectionName)
				{
					if (realSecBuf.find(sec.getIndex()) == realSecBuf.end())
					{
						realSecBuf.insert(std::make_pair(sec.getIndex(), reinterpret_cast<uintptr_t>(addr)));
						realSec.insert(std::make_pair(reinterpret_cast<uintptr_t>(addr), (sec.getAddress())));
					}
				}
			}
		}

		uint8_t * BinWriter::allocateCodeSection(uintptr_t Size, unsigned Alignment,
			unsigned SectionID,
			llvm::StringRef SectionName)
		{

			uint8_t * addr = RTDyldMM->allocateCodeSection(Size, Alignment, SectionID, SectionName);
			mapRealSec(SectionID, SectionName, addr);
			return addr;
		};
		uint8_t * BinWriter::allocateDataSection(uintptr_t Size, unsigned Alignment,
			unsigned SectionID,
			llvm::StringRef SectionName,
			bool IsReadOnly) {
			uint8_t * addr = RTDyldMM->allocateDataSection(Size, Alignment, SectionID, SectionName, IsReadOnly);
			mapRealSec(SectionID, SectionName, addr);
			return addr;
		};
		void BinWriter::reserveAllocationSpace(uintptr_t CodeSize, uint32_t CodeAlign,
			uintptr_t RODataSize,
			uint32_t RODataAlign,
			uintptr_t RWDataSize,
			uint32_t RWDataAlign) {
			RTDyldMM->reserveAllocationSpace(CodeSize, CodeAlign, RODataSize, RODataAlign, RWDataSize, RWDataAlign);

		};
		bool BinWriter::needsToReserveAllocationSpace()
		{
			return false;
		}
		void BinWriter::registerEHFrames(uint8_t *Addr, uint64_t LoadAddr,
			size_t Size) {
			RTDyldMM->registerEHFrames(Addr, LoadAddr, Size);
		};
		void BinWriter::deregisterEHFrames() {
			RTDyldMM->deregisterEHFrames();
		};
		bool BinWriter::finalizeMemory(std::string *ErrMsg) {
			return  RTDyldMM->finalizeMemory(ErrMsg);
		};
		void BinWriter::notifyObjectLoaded(llvm::RuntimeDyld &RTDyld,
			const llvm::object::ObjectFile &Obj) {
			return  RTDyldMM->notifyObjectLoaded(RTDyld, Obj);
		};
		void writeBytesUnaligned(uint64_t Value, uint8_t *Dst,
			unsigned Size=sizeof(void*), bool IsTargetLittleEndian = true) {
			if (IsTargetLittleEndian) {
				while (Size--) {
					*Dst++ = Value & 0xFF;
					Value >>= 8;
				}
			}
			else {
				Dst += Size - 1;
				while (Size--) {
					*Dst-- = Value & 0xFF;
					Value >>= 8;
				}
			}
		}


		void push32(std::vector<unsigned char>* outbuf1, uint64_t reloffset, uint64_t addr)
		{
			outbuf1->emplace(outbuf1->begin() + reloffset, static_cast<unsigned char>((addr) & 0xff));
			outbuf1->emplace(outbuf1->begin() + reloffset + 1, static_cast<unsigned char>((addr >> 8) & 0xff));
			outbuf1->emplace(outbuf1->begin() + reloffset + 2, static_cast<unsigned char>((addr >> 16) & 0xff));
			outbuf1->emplace(outbuf1->begin() + reloffset + 3, static_cast<unsigned char>((addr >> 24) & 0xff));
		}
		void push64(std::vector<unsigned char>* outbuf1, uint64_t reloffset, uint64_t addr)
		{
			push32(outbuf1, reloffset, addr);
			outbuf1->emplace(outbuf1->begin() + reloffset + 4, static_cast<unsigned char>((addr >> 32) & 0xff));
			outbuf1->emplace(outbuf1->begin() + reloffset + 5, static_cast<unsigned char>((addr >> 40) & 0xff));
			outbuf1->emplace(outbuf1->begin() + reloffset + 6, static_cast<unsigned char>((addr >> 48) & 0xff));
			outbuf1->emplace(outbuf1->begin() + reloffset + 7, static_cast<unsigned char>((addr >> 56) & 0xff));
		}

		void BinWriter::anchor()
		{

		}

		llvm::JITSymbol BinWriter::findSymbolInLogicalDylib(const std::string &Name) {
			return findSymbol(Name);
		};
		void  BinWriter::lookup(const llvm::JITSymbolResolver::LookupSet &Symbols,
			OnResolvedFunction OnResolved)
		{
			JITSymbolResolver::LookupResult Result;
			for (auto &Symbol : Symbols) {
				std::string SymName = Symbol.str();
				if (auto Sym = findSymbolInLogicalDylib(SymName)) {
					if (auto AddrOrErr = Sym.getAddress())
						Result[Symbol] = JITEvaluatedSymbol(*AddrOrErr, Sym.getFlags());
					else {
						OnResolved(AddrOrErr.takeError());
						return;
					}
				}
				else if (auto Err = Sym.takeError()) {
					OnResolved(std::move(Err));
					return;
				}
				else {
					// findSymbolInLogicalDylib failed. Lets try findSymbol.
					if (auto Sym = findSymbol(SymName)) {
						if (auto AddrOrErr = Sym.getAddress())
							Result[Symbol] = JITEvaluatedSymbol(*AddrOrErr, Sym.getFlags());
						else {
							OnResolved(AddrOrErr.takeError());
							return;
						}
					}
					else if (auto Err = Sym.takeError()) {
						OnResolved(std::move(Err));
						return;
					}
					else {
						OnResolved(make_error<StringError>("Symbol not found: " + Symbol,
							inconvertibleErrorCode()));
						return;
					}
				}
			}

			OnResolved(std::move(Result));
		}

		llvm::Expected<llvm::JITSymbolResolver::LookupSet> BinWriter::getResponsibilitySet(const llvm::JITSymbolResolver::LookupSet &Symbols)
		{

			llvm::JITSymbolResolver::LookupSet Result;

			for (auto &Symbol : Symbols) {
				std::string SymName = Symbol.str();
				if (auto Sym = findSymbolInLogicalDylib(SymName)) {
					// If there's an existing def but it is not strong, then the caller is
					// responsible for it.
					if (!Sym.getFlags().isStrong())
						Result.insert(Symbol);
				}
				else if (auto Err = Sym.takeError())
					return std::move(Err);
				else {
					// If there is no existing definition then the caller is responsible for
					// it.
					Result.insert(Symbol);
				}
			}

			return std::move(Result);
		}


		llvm::JITSymbol BinWriter::findSymbol(const std::string &Name)
		{
			llvm::JITSymbol ret = this->findSymbolInternal(Name);
			if (ret)
			{				
					return ret;				
			}else
			{
				io::Log::error()<< io::Log::Warning << "failed to resolve external Symbol :=> "<< Name <<" , skip by redirect to remaped segment "<< std::endl;
			}
			return llvm::JITSymbol(remap_seg_addr,
				JITSymbolFlags::Absolute);;
		}
		llvm::JITSymbol BinWriter::findSymbolInternal(const std::string &Name)
		{
			StringRef DemangledName = Name;
			if (DemangledName[0] == _module->getDataLayout().getGlobalPrefix())
				DemangledName = DemangledName.substr(1);
			llvm::Function *F = _module->getFunction(DemangledName);
			//if (F && !F->isDeclaration())
			if (F)
			{
				Address function_address = _config
					->getFunctionAddress(F);
				if (function_address.isDefined())
				{
					uint64_t addr = function_address.getValue();
					/*return llvm::JITSymbol(addr,
						llvm::JITSymbolFlags::fromGlobalValue(*F));*/
					return llvm::JITSymbol(addr, JITSymbolFlags::Absolute);
				}
			}
			GlobalVariable *G = _module->getGlobalVariable(DemangledName, true);
			//if (G && !G->isDeclaration())
			if (G)
			{
				Address global_address = _config
					->getGlobalAddress(G);
				if (global_address.isDefined())
				{
					uint64_t addr = global_address.getValue();
					/*return llvm::JITSymbol(addr,
						llvm::JITSymbolFlags::fromGlobalValue(*G));*/
					return llvm::JITSymbol(addr, JITSymbolFlags::Absolute);
				}
			}			
			// FIXME: Do we need to worry about global aliases?
			return  llvm::JITSymbol(llvm::make_error<StringError>("Symbol not found: ",
				inconvertibleErrorCode()));

			//return  Dyld->getSymbol(Name);
		}



		llvm::Expected<uint64_t> BinWriter::GetSymbolAddrReal(uint64_t Offset, llvm::object::symbol_iterator& It, uint64_t secidx, llvm::StringRef& Name) const
		{
			uint64_t ret = Offset;
			if (auto NameOrErr = It->getName())
			{
				Name = *NameOrErr;
				auto kv = realSecBuf.find(secidx);
				if (kv != realSecBuf.end())
				{
					ret += kv->second;
					if (ret)
					{
						return  ret;
					}
				}

			}
			return llvm::make_error<StringError>("Symbol not found: ",
				inconvertibleErrorCode());
		}

		void BinWriter::remapGlobalSymbolTable(std::map<llvm::StringRef, llvm::JITEvaluatedSymbol>& tbl)
		{
			int sizeptr = _config->getConfig().fileFormat.isPe32() ? 4 : 8;
			for (auto sec : LoadedObjectPtr->sections())
			{
				for (auto rel : sec.relocations())
				{
					object::symbol_iterator It = rel.getSymbol();
					if (It != LoadedObjectPtr->symbol_end())
					{
						StringRef Name;
						uint8_t* ret = 0;
						uint64_t Offset = rel.getOffset();
						if (Offset < sec.getSize()) {
							if (auto addrReal = this->GetSymbolAddrReal(Offset, It, sec.getIndex(), Name))
							{
								ret = (uint8_t*)*addrReal;
								for (auto sym : tbl)
								{

									if (Name == sym.first)
									{
										llvm::JITSymbol retSym = this->findSymbolInternal(sym.first);
										if (retSym)
										{
											uint64_t AddrOrErr = *retSym.getAddress();											
											writeBytesUnaligned(AddrOrErr, ret, sizeptr);

										}
									}
								}
							}
						}

					}
				}
			}
		}
		/**
		 * @return Always @c false. This pass produces DSM output, it does not modify
		 *         module.
		 */
		bool BinWriter::runOnModule(llvm::Module& m)
		{
			SMDiagnostic Err;
			_module = &m;
			_config = ConfigProvider::getConfig(_module);
			if (_config == nullptr)
			{
				return false;
			}

			/*
			std::string InputFilename = "E:\\git\\ReverseEngineer\\Scatterm\\Sterm32.exe.bc";
			LLVMContext Context;

			std::unique_ptr<Module> mdl = parseIRFile(InputFilename, Err, Context, false);
			if(!mdl)
			{
				throw std::runtime_error(Err.getMessage());
			}*/
			std::string TheTriple = _module->getTargetTriple();
			if (TheTriple.empty())
			{
				if (_config->getConfig().fileFormat.isPe32()) {
					TheTriple = Triple::normalize("i686-pc-windows-msvc");
				}
				else if (_config->getConfig().fileFormat.isPe64())
				{
					TheTriple = Triple::normalize("x86_64-pc-windows-msvc");
				}
				else {
					TheTriple = Triple::normalize(sys::getDefaultTargetTriple());
				}

			}
			else
			{
				TheTriple = Triple::normalize(TheTriple);
			}

			Triple ModuleTriple(TheTriple);

			_module->setTargetTriple(TheTriple);
			//mdl->setTargetTriple(TheTriple);
			char *ErrorMsg = 0;
			std::string Error;
			const Target *TheTarget = TargetRegistry::lookupTarget(TheTriple,
				Error);
			if (!TheTarget)
			{
				throw std::runtime_error("Could not allocate target machine!");
			}
			TargetOptions opt;
			std::unique_ptr<TargetMachine> Target_Machine(TheTarget->createTargetMachine(
				TheTriple, "", "", opt, None, None, CodeGenOpt::None));

			llvm::legacy::PassManager pm;
			TargetLibraryInfoImpl TLII(ModuleTriple);
			// The -disable-simplify-libcalls flag actually disables all builtin optzns.
			TLII.disableAllFunctions();
			pm.add(new TargetLibraryInfoWrapperPass(TLII)); \
				std::error_code EC;

			_module->setDataLayout(Target_Machine->createDataLayout());
			//mdl->setDataLayout(Target_Machine->createDataLayout());
			bool usefile = false;
			bool ret = false;
			std::string binOut = _config->getConfig().parameters.getOutputBinFile();
			if (binOut.empty())
			{
				if (!outbuf)
				{
					throw std::runtime_error("binOut empty");
					return false;
				}
				usefile = false;
				SmallVector<char, 0> Buffer;
				raw_svector_ostream OS(Buffer);
				MCContext *Ctx;
				cantFail(_module->materializeAll());
				ret = Target_Machine->addPassesToEmitMC(pm, Ctx, OS);
				/*ret = Target_Machine->addPassesToEmitFile(pm, OS,
					nullptr,
					TargetMachine::CGFT_ObjectFile);*/
				if (ret)
				{
					throw std::runtime_error("target does not support generation of this file type");
					return false;
				}
				pm.run(*_module);
				size_t len = Buffer.size();
				if (len == 0)
				{
					io::Log::error() << "target does not support generation of this file type" << std::endl;
					return false;
				}
				std::unique_ptr<MemoryBuffer> CompiledObjBuffer(
					new SmallVectorMemoryBuffer(std::move(Buffer)));

				Expected<std::unique_ptr<object::ObjectFile>> LoadedObject =
					object::ObjectFile::createObjectFile(CompiledObjBuffer->getMemBufferRef());
				if (!LoadedObject) {
					io::Log::error() << "target does not support generation of this file type" << std::endl;
					return false;
				}


				RTDyldMM = new SectionMemoryManager();
				Dyld = new RuntimeDyld(*static_cast<RuntimeDyld::MemoryManager*>(this), *static_cast<JITSymbolResolver*>(this));
				LoadedObjectPtr = LoadedObject->get();
				std::unique_ptr<RuntimeDyld::LoadedObjectInfo> L =
					Dyld->loadObject(*LoadedObjectPtr);
				if (Dyld->hasError())
				{
					io::Log::error() << "target does not support generation of this file type" << Dyld->getErrorString() << std::endl;
					return false;
				}
				this->remapSecLoadAddress();
				Dyld->resolveRelocations();
				this->remapGlobalSymbolTable(Dyld->getSymbolTable());

				Dyld->registerEHFrames();
				// Set page permissions.
				this->finalizeMemory();

				size_t	len_org = CompiledObjBuffer->getBufferSize();

				if (len_org) {
					for (auto sec : LoadedObjectPtr->sections())
					{
						if (sec.isText() && !sec.isData())
						{
							auto pair = realSecBuf.find(sec.getIndex());
							if (pair != realSecBuf.end())
							{
								std::uint8_t* buffer_start = reinterpret_cast<std::uint8_t*>(pair->second);
								size_t	len = sec.getSize();
								outbuf->reserve(len);
								outbuf->clear();
								for (int i = 0; i < len; i++)
								{
									outbuf->emplace_back(static_cast<unsigned char>(*(buffer_start + i)));
								}
								break;
							}
						}
					}
					//memcpy(outbuf->data(), buffer_start, len);				

					size_t rawlen = outbuf->size();
					if (rawlen == 0)
					{
						io::Log::error() << "target does not support generation of this file type" << std::endl;
						return false;
					}
					std::ostringstream logstr;
					logstr << "target compiler generate function binary buff :=> total size ";
					logstr << std::hex << len_org;
					logstr <<" bytes , raw size ";
					logstr << std::hex << rawlen;
					logstr << " bytes";
					io::Log::phase(logstr.str(), io::Log::SubPhase);

				}
				else
				{
					io::Log::error() << "target does not support generation of this file type" << std::endl;
					return false;
				}

			}
			else
			{
				usefile = true;
				auto	FDOut =
					llvm::make_unique<ToolOutputFile>(binOut, EC, sys::fs::OpenFlags::F_None);

				if (EC) {
					throw std::runtime_error(EC.message());
					return false;
				}
				ret = Target_Machine->addPassesToEmitFile(pm, FDOut->os(),
					nullptr,
					TargetMachine::CGFT_ObjectFile);
				if (ret)
				{
					throw std::runtime_error("target does not support generation of this file type");
					return false;
				}
				pm.run(*_module);
				FDOut->keep();
				std::string logstr = "target compiler generate function binary to file :=>";
				logstr += binOut;
				io::Log::phase(logstr, io::Log::SubPhase);
			}


			return !ret;
		}


	}
}

