#include "retdec/bin2llvmir/optimizations/type_reconstructor/type_reconstructor.h"

#include <retdec/llvmir2hll/support/smart_ptr.h>

#include "retdec/utils/io/log.h"



using namespace retdec::common;
using namespace retdec::utils;
using namespace llvm;

namespace retdec {
	namespace bin2llvmir {
		char TypeReconstructor::ID = 0;
		RegisterPass<TypeReconstructor> TypeReconstructorRegistered(
			"retdec-type-reconstructor",
			"Type reconstructor optimization",
			false,
			false);
		FakeFunction::FakeFunction()
		{
			std::vector<llvm::BasicBlock*> fakefn;
			fakefn.reserve(256);
			blocks = std::make_shared<std::vector<llvm::BasicBlock*>>(fakefn);
		}
		void FakeFunction::dump()
		{
			for (llvm::BasicBlock* b : *this->blocks)
			{
				b->dump();
			}
		}

		bool FakeFunction::emplace_back(llvm::BasicBlock* b)
		{
			for (llvm::BasicBlock* b1 : *this->blocks)
			{
				if (b1 == b)
				{
					return false;
				}

			}
			this->blocks->emplace_back(b);
			return true;
		}
		TypeReconstructor::TypeReconstructor() : FunctionPass(ID) {

		}

		void TypeReconstructor::FistInit(llvm::Module* M)
		{
			FileImage* image;

			if (!_config)
			{
				_config = ConfigProvider::getConfig(M);

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
				Address vtaddr = vt.getAddress();
				llvm::SmallVector<retdec::common::Address, 256> paddrs;
				for (auto& item : vt.items)
				{
					Address compareAddrFunc = item.getTargetFunctionAddress();
					paddrs.emplace_back(compareAddrFunc);

				}
				vtbl2addrs.emplace(vtaddr, paddrs);
			}

		}

		bool TypeReconstructor::doInitialization(Module &M) {

			return true;
		}

		/**
		 * Finalization method called on every module
		 *
		 * @param M module
		 * @return always true
		 */
		bool TypeReconstructor::doFinalization(Module &M) {
			/*for (auto& kv : addr2sct)
			{
				FinalizeStructType(kv.second);
			}*/
			return true;
		}
		bool TypeReconstructor::runOnFunction(llvm::Function& f)
		{
			if (!InitializedPass)
			{
				FistInit(f.getParent());
				InitializedPass = true;
			}
			AnalyzeFunction(f);

			return true;
		}
		bool TypeReconstructor::isRCX(llvm::Value* val) {

			const llvm::StringRef valueName = val->getName();
			return  val == fakercx || valueName == "ecx" || valueName == "rcx";
		}

		bool TypeReconstructor::isRSP(llvm::Value* val) {

			const llvm::StringRef valueName = val->getName();
			return  valueName == "esp" || valueName == "rsp";
		}

		bool TypeReconstructor::isRBP(llvm::Value* val) {

			const llvm::StringRef valueName = val->getName();
			return  valueName == "ebp" || valueName == "rbp";
		}
		bool TypeReconstructor::isAsmProgramCounter(llvm::Value* val) {

			const llvm::StringRef valueName = val->getName();
			return  valueName.find("asm_program_counter") != std::string::npos;
		}
		PVatbleStructType TypeReconstructor::NeedAnalyzeFunction(llvm::Function & f, Address vtaddr, std::string StructTypeName, bool& doAnalyze)
		{
			PVatbleStructType classtp = nullptr;
			LLVMContext& ctx = f.getParent()->getContext();
			Instruction& checkAsmProgramCounter = f.getEntryBlock().front();
			Address  pairAddr = GetPairFunctionAddress(&checkAsmProgramCounter);
			if (pairAddr.isDefined() && pairAddr != 0)
			{
				doAnalyze = false;
				auto v2a = vtbl2addrs.find(vtaddr);
				v2a->second.emplace_back(pairAddr);
				return nullptr;
			}

			doAnalyze = true;
			auto kv = addr2sct.find(vtaddr);
			if (kv == addr2sct.end())
			{
				StructType*	sct = StructType::create(ctx, StructTypeName);
				llvm::SmallVector<POverlappedEmulatedType, 256> typesctval;
				classtp = new VatbleStructType
				{
					vtaddr,
					StructTypeName,
					sct,
					std::make_unique<llvm::SmallVector<POverlappedEmulatedType, 256>>(typesctval)
				};

				addr2sct.emplace(vtaddr, classtp);
			}
			else
			{
				classtp = kv->second;
			}
			return classtp;
		}
		PVatbleStructType TypeReconstructor::NeedAnalyzeClassStruct(llvm::Function & f)
		{
			LLVMContext& ctx = f.getParent()->getContext();
			Address start = _config->getFunctionAddress(&f);
			PVatbleStructType classtp = nullptr;
			bool doAnalyze = false;

			for (auto* p : vtable)
			{
				auto& vt = *p;
				Address vtaddr = vt.getAddress();
				auto v2a = vtbl2addrs.find(vtaddr);
				std::string varName = names::generateVtableName(vtaddr);
				std::string typeName = varName + "_type";
				std::string StructTypeName = "Class_" + varName + "_type";
				if (v2a != vtbl2addrs.end())
				{
					for (auto it = v2a->second.begin(); it != v2a->second.end(); it++)
					{
						Address caddr = *it;
						if (caddr == start)
						{
							v2a->second.erase(it);
							classtp = NeedAnalyzeFunction(f, vtaddr, StructTypeName, doAnalyze);
							goto doAnalyze_end;
						}
					}
				}
			}
		doAnalyze_end:
			if (!doAnalyze)
			{
				return nullptr;
			}
			else
			{
				return classtp;
			}

		}

		bool TypeReconstructor::SkipAsmProgramCounter(llvm::Instruction*i)
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

		Address TypeReconstructor::GetPairFunctionAddress(llvm::Instruction* insert_before)
		{
			if (SkipAsmProgramCounter(insert_before))
			{
				Instruction* checkcall = insert_before->getNextNode();
				if (CallInst* callfun = dyn_cast_or_null<CallInst>(checkcall))
				{

					auto* funcReal = callfun->getCalledFunction();
					if (funcReal)
					{
						Address startReal = _config->getFunctionAddress(funcReal);

						return startReal;
					}

				}
			}
			return Address(0);
		}
		bool TypeReconstructor::SplitBasicBlock(llvm::BasicBlock* b, FakeFunction* from)
		{
			Instruction* termer = b->getTerminator();
			if (isa<ReturnInst>(termer))
			{
				return true;
			}
			llvm::BasicBlock* suc = b->getSingleSuccessor();
			if (suc)
			{

				if (from->emplace_back(suc))
				{
				 	SplitBasicBlock(suc, from);

				}
				return true;
			}
			int num_successors = termer->getNumSuccessors();
			//无分支就收敛
			if (num_successors == 0)
			{
				return true;
			}

			if (num_successors == 1)
			{
				llvm::BasicBlock* branch = termer->getSuccessor(0);
				from->emplace_back(branch);
				return true;
			}
			for (int i = 0; i < num_successors; i++)
			{
				FakeFunction* fork = ForkRootFunction();
				allfuncpath.emplace_back(fork);
				//拷贝复制的分支
				for (llvm::BasicBlock* b2 : *from->blocks) {
					fork->emplace_back(b2);
				}
				llvm::BasicBlock* branch = termer->getSuccessor(i);
				//成功后再递归
				if (fork->emplace_back(branch)) {
					SplitBasicBlock(branch, fork);
				}
			}

			return false;
		}
		bool TypeReconstructor::SplitFunction(llvm::Function& f)
		{
			FakeFunction* fork = ForkRootFunction();
			allfuncpath.emplace_back(fork);
			llvm::BasicBlock& b = f.getEntryBlock();
			fork->emplace_back(&b);
			for (llvm::BasicBlock& b : f) {
				SplitBasicBlock(&b, fork);
			}
			return false;
		}

		FakeFunction* TypeReconstructor::ForkRootFunction()
		{

			return  new FakeFunction();
		}
		bool TypeReconstructor::AnalyzeInitialize()
		{
			val2refs.clear();
			val2use.clear();
			return true;
		}
		bool TypeReconstructor::DoAnalyzeFunction(FakeFunction* fakefn)
		{
			AnalyzeInitialize();
			for (llvm::BasicBlock* b : *fakefn->blocks) {

				//b.dump();
				for (Instruction& i : *b)
				{
					HandleInstruction(i);
				}
			}
			return true;
		}
		bool TypeReconstructor::AnalyzeFunction(llvm::Function& f)
		{
			root_func = &f;

			LLVMContext& ctx = f.getParent()->getContext();
			Address start = _config->getFunctionAddress(&f);
			if (start != 0x411109 && start != 0x411600)
			{
			     //return false;
			}
			classtp = NeedAnalyzeClassStruct(f);
			if (classtp)
			{
				StructType*	sct = classtp->sct;
				//f.dump();
				std::string  ecxName = "ecx";
				std::string  ecxNameGlobal = "gl_ecx";
				std::string StructTypeName = sct->getName();
				llvm::Type* poitp = PointerType::getUnqual(sct);
				fakercx = _abi->getRegister(x86_reg::X86_REG_ECX);
				auto* stgv = _config->_module->getOrInsertGlobal(StructTypeName, sct);
				Instruction* insert_before = nullptr;
				Instruction& checkAsmProgramCounter = f.getEntryBlock().front();
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
				SplitFunction(f);
				for (FakeFunction* fork : allfuncpath) {
					//fork->dump();
					DoAnalyzeFunction(fork);
				}
				FinalizeStructType(classtp);
			}
			return true;
		}
		//从val能找到rcx
		bool TypeReconstructor::SameReferenceToRcx(llvm::Value* val, int64_t& offset)
		{
			if (isRCX(val))
			{
				return true;
			}
			//if (!rootrcx_dirty)
			if (true)
			{
				auto kv = val2refs.find(val);
				if (kv == val2refs.end()) {

					return false;
				}
				else
				{
					PEmulatedValueType refval = kv->second.back();
					while (refval&&refval->ref == SameLevel)
					{
						offset += refval->offsetinner;
						if (isRCX(refval->val))
						{
							return true;
						}
						if (refval->left)
						{
							refval = refval->left;
						}
						else
						{
							break;
						}
					}
					return false;
				}
			}

			return false;
		}

		//树不需要递归引用相同键
		bool TypeReconstructor::AddSameReferenceValue(llvm::Value* op, llvm::Value* other)
		{

			auto otheremu = FindOrCreateEmulatedValueType(other);
			auto userageval = val2use.find(other);
			auto userageop = val2use.find(op);
			bool swapuage = false;
			if (userageval != val2use.end())
			{
				if (userageop != val2use.end())
				{
					userageop->second.usage = userageval->second.usage;
					swapuage = true;
				}
			}

			if (!swapuage)
			{
				EmulatedValueUasge uasge = {
				 SameLevel,UndefinedUasge
				};
				val2use.emplace(op, uasge);
			}
			int64_t offsetrcx;
			//跟rcx引用的都要是dirty
			bool valdirty = SameReferenceToRcx(other, offsetrcx);
			//如果other指向rcx只要还没更新就是back
			PEmulatedValueType reftp = new EmulatedValueType(
				0,
				0,
				SameLevel,
				valdirty,
				op,
				otheremu,
				nullptr
			);
			//递归放入树可以从底层遍历到最上层和rcx的关系
			EmplaceEmulatedValueType(op, reftp);
			return true;
		}
		PEmulatedValueType TypeReconstructor::FindEmulatedValueType(llvm::Value* val)
		{
			if (_abi->isGeneralPurposeRegister(val) && (isRSP(val) || isRBP(val)))
			{
				PEmulatedValueType reftp = new EmulatedValueType(
					0,
					0,
					SameLevel,
					false,
					val,
					nullptr,
					nullptr
				);
				return reftp;
			}
			auto pair = val2refs.find(val);
			if (pair != val2refs.end())
			{
				return pair->second.back();
			}

			return nullptr;
		}

		PEmulatedValueType TypeReconstructor::FindOrCreateEmulatedValueType(llvm::Value* val)
		{
			PEmulatedValueType reftp = new EmulatedValueType(
				0,
				0,
				SameLevel,
				false,
				val,
				nullptr,
				nullptr
			);
			auto* ci = llvm::dyn_cast<llvm::ConstantInt>(val);
			if (ci)
			{
				return reftp;
			}
			int64_t offsetrcx = 0;
			//跟rcx引用的都要是dirty
			bool isrcx = SameReferenceToRcx(val, offsetrcx);
			if (isrcx)
			{
				reftp->val = fakercx;
				reftp->offsetinner = offsetrcx;
				return reftp;
			}
			PEmulatedValueType emu = FindEmulatedValueType(val);
			if (emu)
			{
				return emu;
			}
			else {
				//递归放入树可以从底层遍历到最上层和rcx的关系
				EmplaceEmulatedValueType(val, reftp);
				return reftp;
			}
		}
		PEmulatedValueType TypeReconstructor::AddPointReferenceValue(llvm::Value* store, llvm::Value* ptr, llvm::Value* val, int64_t offsetout, int64_t offsetinner)
		{
			rootrcx_dirty |= isRCX(ptr);
			auto valemu = FindOrCreateEmulatedValueType(val);
			auto ptremu = FindOrCreateEmulatedValueType(ptr);
			bool valdirty = !rootrcx_dirty&&valemu && isRCX(val);
			//对称ReferenceLevel和DeReferenceLevel
			PEmulatedValueType reftp2 = new EmulatedValueType(
				offsetout,
				offsetinner,
				ReferenceLevel,
				valdirty,
				ptr,
				valemu,
				nullptr
			);
			EmplaceEmulatedValueType(ptr, reftp2);
			//用于处理store指令,ptrhe val指向的都是不同的枚举类型
			PEmulatedValueType reftp = new EmulatedValueType(
				offsetout,
				offsetinner,
				PointerLevel,
				valdirty,
				store,
				ptremu,
				valemu
			);

			EmplaceEmulatedValueType(store, reftp);
			return reftp;
		}

		PEmulatedValueType TypeReconstructor::AddPointDeReferenceValue(llvm::Value* ptr, llvm::Value* val, int64_t offsetout, int64_t offsetinner)
		{
			//mov ptr,[val]
			rootrcx_dirty |= isRCX(ptr);
			auto valemu = FindOrCreateEmulatedValueType(val);
			bool valdirty = !rootrcx_dirty&&val2refs.find(val) == val2refs.end() && isRCX(val);
			PEmulatedValueType reftp = new EmulatedValueType(
				offsetout,
				offsetinner,
				DereferenceLevel,
				valdirty,
				ptr,
				valemu,
				nullptr
			);
			EmplaceEmulatedValueType(ptr, reftp);
			llvm::ConstantInt*  ci0 = DemoteConstantIntToOriginalInt(ptr);			
			//左边是常量,右边也是常量
			if (ci0)
			{
				return AddSameReferenceConstant(ptr, 0, ci0);
			}
			return reftp;
		}
		bool  SortNoOverlappedStruct(POverlappedEmulatedType x, POverlappedEmulatedType y)
		{
			return y->offset > x->offset;

		}

		bool TypeReconstructor::IsOverlapped(POverlappedEmulatedType hasType, POverlappedEmulatedType hasType2)
		{
			bool Overlapped = false;
			//左插入
			if (hasType->offset + hasType->length > hasType2->offset && hasType->offset + hasType->length < hasType2->offset + hasType2->length)
			{
				Overlapped = true;
			}
			//右插入
			if (hasType->offset > hasType2->offset && hasType->offset < hasType2->offset + hasType2->length)
			{
				Overlapped = true;
			}
			//包含
			if (hasType->offset < hasType2->offset && hasType->offset + hasType->length > hasType2->offset + hasType2->length)
			{
				Overlapped = true;
			}
			return Overlapped;
		}

		bool TypeReconstructor::FinalizeStructType(PVatbleStructType classtpFinal)
		{

			if (classtpFinal&&classtpFinal->typesct->size())
			{
				auto v2a = vtbl2addrs.find(classtpFinal->vtaddr);
				//全部分析完了再处理结构体
				if (v2a->second.size() == 0)
				{
					std::vector<llvm::Type *> ElementsStruct;
					llvm::SmallVector<POverlappedEmulatedType, 256> NoOverlappedStruct;
					llvm::SmallVector<POverlappedEmulatedType, 256> NoOverlappedStructFinal;
					llvm::SmallVector<POverlappedEmulatedType, 256> OverlappedStruct;
					std::map<POverlappedEmulatedType, llvm::SmallVector<POverlappedEmulatedType, 256>> ProcessOverlappedStruct;
					for (auto& hasType : *classtpFinal->typesct)
					{
						bool Overlapped = false;
						for (auto& hasType2 : *classtpFinal->typesct)
						{
							if (hasType == hasType2)
							{
								continue;
							}
							Overlapped = IsOverlapped(hasType, hasType2);
						}
						if (!Overlapped)
						{

							NoOverlappedStruct.emplace_back(hasType);
						}
						else
						{
							OverlappedStruct.emplace_back(hasType);
						}
					}

					for (auto& hasType : OverlappedStruct)
					{

						for (auto& hasType2 : OverlappedStruct)
						{
							bool Overlapped = false;
							if (hasType == hasType2)
							{
								continue;
							}
							Overlapped = IsOverlapped(hasType, hasType2);
							if (Overlapped)
							{
								auto sameKey = ProcessOverlappedStruct.find(hasType);
								if (sameKey == ProcessOverlappedStruct.end())
								{
									llvm::SmallVector<POverlappedEmulatedType, 256> sameValue = { hasType2 };
									ProcessOverlappedStruct.emplace(hasType, sameValue);
								}
								else
								{
									sameKey->second.emplace_back(hasType2);
								}

							}
						}
					}
					//处理重叠的结构体
					for (auto& kv : ProcessOverlappedStruct)
					{
						llvm::SmallVector<uint64_t, 256> splitoffset;
						splitoffset.emplace_back(kv.first->offset);
						splitoffset.emplace_back(kv.first->offset + kv.first->length);
						for (auto& hasType : kv.second)
						{
							splitoffset.emplace_back(hasType->offset);
							splitoffset.emplace_back(hasType->offset + hasType->length);
						}
						splitoffset.erase(std::unique(splitoffset.begin(), splitoffset.end()), splitoffset.end());
						std::sort(splitoffset.begin(), splitoffset.end());
						for (auto thunk = splitoffset.begin(); thunk != splitoffset.end(); thunk++)
						{
							uint64_t offset = *thunk;
							thunk++;
							//如果是最后一个那么下一个就是end
							if (thunk != splitoffset.end())
							{
								uint64_t length = *thunk - offset;
								thunk--;
								llvm::Type* tp = llvm::Type::getIntNTy(_config->_module->getContext(), length);

								POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
								offset,length,tp
								};
								NoOverlappedStruct.emplace_back(typeEmu);
							}

						}
					}
					std::sort(NoOverlappedStruct.begin(), NoOverlappedStruct.end(), SortNoOverlappedStruct);
					//填充剩余空隙
					for (auto& hasType : NoOverlappedStruct)
					{
						uint64_t min = hasType->offset;
						uint64_t max = hasType->offset + hasType->length;
						uint64_t maxProc = max;
						for (auto& hasType2 : NoOverlappedStruct)
						{
							bool Overlapped = false;
							if (hasType == hasType2)
							{
								continue;
							}
							if (hasType2->offset + hasType2->length < min)
							{
								continue;
							}
							//一旦有重叠不填充
							if (hasType2->offset == max)
							{
								maxProc = max;
								break;
							}
							//空隙间距最小的那一层,超过了max就要处理
							if (hasType2->offset > max)
							{
								uint64_t maxProcold = maxProc;
								maxProc = hasType2->offset;
								if(maxProcold < maxProc&&maxProcold>max)
								{
									maxProc = maxProcold;
								}
								
							}
						}
						max = maxProc;
						uint64_t paddingLen = max - min;
						assert(paddingLen >= hasType->length);
						if (paddingLen == hasType->length)
						{
							NoOverlappedStructFinal.emplace_back(hasType);
							continue;
						}
						else
						{
							uint64_t arrCount = paddingLen / hasType->length;
							if (arrCount == 1)
							{
								NoOverlappedStructFinal.emplace_back(hasType);
								std::size_t  lengthRemain = paddingLen - hasType->length;
								if (lengthRemain > 0)
								{
									uint64_t offsetRemain = hasType->offset + hasType->length;
									llvm::Type* tp = llvm::Type::getIntNTy(_config->_module->getContext(), lengthRemain);

									POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
									offsetRemain,lengthRemain,tp
									};
									NoOverlappedStructFinal.emplace_back(typeEmu);
								}
								continue;
							}
							else
							{
								llvm::Type* tpArr = ArrayType::get(hasType->typefield, arrCount);
								uint64_t tpArrLen = hasType->length*arrCount;
								POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
									 hasType->offset,tpArrLen,tpArr
								};
								NoOverlappedStructFinal.emplace_back(typeEmu);
								std::size_t lengthRemain = paddingLen - tpArrLen;
								if (lengthRemain > 0)
								{
									uint64_t offsetRemain = hasType->offset + tpArrLen;
									llvm::Type* tp = llvm::Type::getIntNTy(_config->_module->getContext(), lengthRemain);
									POverlappedEmulatedType typeEmu2 = new OverlappedEmulatedType{
									offsetRemain,lengthRemain,tp
									};
									NoOverlappedStructFinal.emplace_back(typeEmu2);
								}
								continue;
							}

						}
					}
					std::sort(NoOverlappedStructFinal.begin(), NoOverlappedStructFinal.end(), SortNoOverlappedStruct);

					for (auto& hasType : NoOverlappedStructFinal)
					{
						ElementsStruct.emplace_back(hasType->typefield);
					}
					classtpFinal->sct->setBody(ElementsStruct);
				}
			}
			return true;
		}


		bool TypeReconstructor::AddOrReplaceOverlappedEmulatedType(llvm::Type* tp, uint64_t offset, std::size_t length)
		{
			bool samestruct = false;
			for (auto hasType : *classtp->typesct)
			{
				if (hasType->offset == offset && hasType->offset + hasType->length == offset + length)
				{
					samestruct = true;
				}


			}
			if (!samestruct)
			{
				POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
					offset,length,tp
				};
				classtp->typesct->emplace_back(typeEmu);
			}
			return true;
		}
		bool TypeReconstructor::GenerateReferenceStruct(PEmulatedValueType val)
		{
			//val和ptr都要计算
			bool hasfakercx = false;
			int DereferenceCount = 0;
			int offsetinnerecx = 0;
			PEmulatedValueType kvval = val;
			llvm::Value* valright = nullptr;
			if (kvval)
			{
				for (kvval; kvval != nullptr; kvval = kvval->left)
				{
					auto findval = kvval;
					if (findval->ref == DereferenceLevel)
					{
						DereferenceCount++;
					}
					//mov [ecx+xxx],valvalright
					if (findval->ref == PointerLevel)
					{
						valright = kvval->right->val;
					}
					if (findval->val == fakercx && DereferenceCount <= 1)
					{
						hasfakercx = true;
					}

					if (findval->ref == SameLevel && findval->offsetinner > 0)
					{
						offsetinnerecx = findval->offsetinner;
					}
				}
			}

			if (hasfakercx&&valright)
			{

				llvm::Type* righttype = valright->getType();
				int valsize = _config->_module->getDataLayout().getTypeSizeInBits(righttype);
				AddOrReplaceOverlappedEmulatedType(righttype, offsetinnerecx * 8, valsize);
			}


			return true;
		}
		bool TypeReconstructor::GenerateDereferenceStruct(PEmulatedValueType val)
		{
			//val和ptr都要计算
			bool hasfakercx = false;
			int DereferenceCount = 0;
			int offsetinnerecx = 0;
			PEmulatedValueType kvval = val;
			llvm::Value* valleft = nullptr;
			llvm::Value* valleftptr = nullptr;
			if (kvval)
			{
				for (kvval; kvval != nullptr; kvval = kvval->left)
				{
					auto findval = kvval;
					if (findval->ref == DereferenceLevel)
					{
						DereferenceCount++;
					}
					//mov al,[ecx+xxx]
					if (findval->ref == DereferenceLevel)
					{
						valleft = kvval->left->val;
						valleftptr = kvval->val;
					}
					if (findval->val == fakercx && DereferenceCount <= 1)
					{
						hasfakercx = true;
					}

					if (findval->ref == SameLevel && findval->offsetinner > 0)
					{
						offsetinnerecx = findval->offsetinner;
					}
				}
			}

			if (hasfakercx&&valleft)
			{

				llvm::Type* valleftptrtype = valleftptr->getType();
				llvm::Type* vallefttype = valleft->getType();
				int valsize = _config->_module->getDataLayout().getTypeSizeInBits(valleftptrtype);
				AddOrReplaceOverlappedEmulatedType(valleftptrtype, offsetinnerecx * 8, valsize);
			}


			return true;
		}
		//mov [reg+num]=ecx	
		//mov eax, [reg+num] = mov eax,ecx		
		bool TypeReconstructor::UpdateToRcxTree(llvm::Value* ptr, llvm::Value* val)
		{
			auto kv = FindEmulatedValueType(ptr);
			if (kv)
			{
				//rcx没有解引用
				int64_t offsetrcx = 0;
				int64_t offsetptr = 0;
				//	mov eax,[ebp-4]中ebp-4正好是ecx值
					//auto rcxval = _abi->getRegister(x86_reg::X86_REG_ECX);
				auto kvrcx = val2refs.find(fakercx);
				if (kvrcx != val2refs.end() && kvrcx->second.size() > 0) {
					int DereferenceCount = 0;
					for (kv = kv->right; kv != nullptr&& kv->left != nullptr; kv = FindEmulatedValueType(kv->left->val))
					{
						auto findval = kv;
						if (findval->ref == DereferenceLevel)
						{
							DereferenceCount++;
						}
						if (DereferenceCount > 1)
						{
							return true;
						}

						if (findval->ref == SameLevel)
						{
							offsetptr += findval->offsetinner;
						}
						//mov [reg+num]=ecx,mov eax, [reg+num] => mov eax,ecx
						//
						//这个要取得reg=kv->left->val
						for (auto& deref : kvrcx->second)
						{
							if ((deref->val == findval->val || deref->val == findval->left->val) && deref->offsetinner == offsetptr)
							{
								auto kv2 = val2refs.find(ptr);

								if (kv2 != val2refs.end())
								{
									if (kv2->second.size())
									{
										auto deststore = kv2->second.back();
										if (deststore->left)
										{
											auto dest = deststore->left->val;
											kv2->second.clear();
											val2refs.erase(kv2);
											AddSameReferenceValue(dest, fakercx);
										}
									}
								}

								return true;
							}
						}
					}
				}
			}
			return false;
		}
		//对于不是rcx的引用或者指针的指针的解引用就计算//mov [reg+num]=ecx	
		bool TypeReconstructor::SamllDereferenceFromRcxTree(llvm::Value* ptr, llvm::Value* val)
		{
			int64_t offsetrcx = 0;
			if (SameReferenceToRcx(val, offsetrcx))
			{
				return true;
			}

			return false;
		}
		//对于不是rcx的引用或者指针的指针的解引用就计算//mov [reg+num]=ecx	
		bool TypeReconstructor::UpdateFromRcxTree(llvm::Value* ptr, llvm::Value* val)
		{
			//至底至上计算树也计算虚拟寄存器引用
			auto kv = FindEmulatedValueType(ptr);
			if (kv)
			{
				//rcx没有解引用
				int64_t offsetrcx = 0;
				int64_t offsetptr = 0;
				llvm::Value* finalptr = nullptr;
				if (SameReferenceToRcx(val, offsetrcx))
				{
					int DereferenceCount = 0;
					for (kv; kv != nullptr; kv = kv->left)
					{
						auto& findval = kv;
						if (findval->ref == PointerLevel)
						{
							DereferenceCount++;
						}
						//只计算解引用一层
						if (DereferenceCount > 1)
						{
							return false;
						}


						if (findval->ref == SameLevel)
						{
							offsetptr += findval->offsetinner;
						}
						//mov [reg+num]=ecx					
						//这个要取得最后一个寄存器reg+num中的reg=finalptr
						finalptr = findval->val;


					}
					//当rcx值指向的是原始值的时候,内存中保存的是值的常量
					/*if (rootrcx_dirty)
					{
						return false;
					}*/
					//auto rcxval = _abi->getRegister(x86_reg::X86_REG_ECX);
					PEmulatedValueType reftp = new EmulatedValueType(
						offsetrcx,
						offsetptr,
						DereferenceLevel,
						false,
						finalptr
					);


					EmplaceEmulatedValueType(fakercx, reftp);
					return true;
				}
			}
			return false;

		}
		bool TypeReconstructor::HandleStoreInst(StoreInst* store_inst)
		{
			auto val = store_inst->getValueOperand();
			auto ptr = store_inst->getPointerOperand();

			auto userageval = val2use.find(ptr);
			if (userageval != val2use.end())
			{
				if (userageval->second.usage == UndefinedUasge)
				{
					userageval->second.usage = InitialzedUasge;
				}
			}
			/*auto* ci = llvm::dyn_cast<llvm::ConstantInt>(val);
			if (ci)
			{
				//AddSameReferenceValue(val, val);
				//return true;
				if (!isAsmProgramCounter(ptr))
				{
					store_inst->dump();
				}
			}
			*/
			PEmulatedValueType tp = AddPointReferenceValue(store_inst, ptr, val);
			//只有rcx的引用会模拟计算ptr值
			bool from = UpdateFromRcxTree(store_inst, val);
			//模拟解引用未被污染的rcx
			bool to = UpdateToRcxTree(store_inst, val);
			if (from | to)
			{
				GenerateReferenceStruct(tp);
			}
			return true;
		}

		bool TypeReconstructor::HandleLoadInst(LoadInst* load_inst)
		{

			if (isRCX(load_inst->getPointerOperand()))
			{
				//assert(_abi->isRegister(load_inst->getPointerOperand(), x86_reg::X86_REG_ECX));

			}
			auto* val = llvm_utils::skipCasts(load_inst->getPointerOperand());
			if (val)
			{
				if (isRCX(val))
				{
					AddSameReferenceValue(load_inst, fakercx);
					return true;
				}

				PEmulatedValueType emu = FindEmulatedValueType(val);
				if (emu)
				{
					while (emu&&emu->ref == SameLevel && emu->offsetinner == 0 && emu->offsetout == 0)
					{
						if (emu->val == fakercx)
						{
							AddSameReferenceValue(load_inst, fakercx);
							return true;
						}
						emu = emu->left;
					}

				}
				if (_abi->isGeneralPurposeRegister(val) && (isRSP(val) || isRBP(val)))
				{
					AddSameReferenceValue(load_inst, val);
					return true;
				}
				else {
					PEmulatedValueType tp = AddPointDeReferenceValue(load_inst, val);
					if (SamllDereferenceFromRcxTree(load_inst, val))
					{
						GenerateDereferenceStruct(tp);
					}
					return true;
				}


			}
			return true;
		}

		bool TypeReconstructor::HandleIntToPtrInst(IntToPtrInst* i2_p)
		{
			auto op0 = i2_p->getOperand(0);
			AddSameReferenceValue(i2_p, op0);
			return true;
		}
		bool TypeReconstructor::HandleTruncInst(TruncInst* tc)
		{
			auto op0 = tc->getOperand(0);
			AddSameReferenceValue(tc, op0);
			return true;
		}

		bool TypeReconstructor::HandleCallInst(CallInst* call)
		{
			return true;
		}

		bool TypeReconstructor::HandleReturnInst(ReturnInst* ret)
		{
			return true;
		}

		void TypeReconstructor::EmplaceEmulatedValueType(llvm::Value* val, PEmulatedValueType reftp)
		{
			auto kv = val2refs.find(val);
			if (kv == val2refs.end()) {

				llvm::SmallVector<PEmulatedValueType, 256> refs = { reftp };
				val2refs.emplace(val, refs);
			}
			else
			{
				kv->second.emplace_back(reftp);
			}
		}
		llvm::ConstantInt* TypeReconstructor::DemoteConstantIntToOriginalInt(llvm::Value* val)
		{
			auto kv = FindEmulatedValueType(val);
			if (kv)
			{
				int64_t offsetptr = 0;
				int DereferenceCount = 0;
				for (kv; kv != nullptr; kv = kv->left)
				{
					if (kv->ref == DereferenceLevel)
					{
						DereferenceCount++;
					}
					if (kv->ref == ReferenceLevel)
					{
						DereferenceCount--;
					}
					if (kv->ref == SameLevel)
					{
						offsetptr += kv->offsetinner;
					}
					//如果没有对称解反解引用直接取值也可以
					auto* ci = llvm::dyn_cast<llvm::ConstantInt>(kv->val);
					if (!ci&&kv->left)
					{
						ci = llvm::dyn_cast<llvm::ConstantInt>(kv->left->val);
					}
					if (ci&&DereferenceCount == 0)
					{
						//mov eax,1
						//add eax,33
						//eax=1+33
						offsetptr += ci->getSExtValue();
						return llvm::ConstantInt::get(ci->getType(), offsetptr);
					}
				}

			}
			return nullptr;
		}
		//树不需要递归引用相同键
		PEmulatedValueType TypeReconstructor::AddSameReferenceConstant(llvm::Value* op, int64_t offset, llvm::Value* val)
		{
			auto otheremu = FindOrCreateEmulatedValueType(val);
			PEmulatedValueType reftp = new EmulatedValueType(
				0,
				offset,
				SameLevel,
				false,
				op,
				otheremu,
				nullptr
			);
			EmplaceEmulatedValueType(op, reftp);
			return reftp;
		}
		bool TypeReconstructor::HandleBinaryOperator(Instruction* bop)
		{
			Emulated_Ref level = SameLevel;
			unsigned opcode = bop->getOpcode();
			Value* op0 = bop->getOperand(0);
			Value* op1 = bop->getOperand(1);
			auto* ci1 = llvm::dyn_cast<llvm::ConstantInt>(op1);
			if (ci1)
			{
				llvm::ConstantInt*  ci0 = DemoteConstantIntToOriginalInt(op0);
				int64_t vl1 = ci1->getSExtValue();
				//左边是常量,右边也是常量
				if (ci0)
				{

					int64_t vl0 = ci0->getSExtValue();
					int64_t vfinal = vl0;
					if (opcode == llvm::Instruction::Sub || opcode == llvm::Instruction::FSub)
					{

						vfinal -= vl1;

					}
					if (opcode == llvm::Instruction::Add || opcode == llvm::Instruction::FAdd)
					{
						vfinal += vl1;
					}

					if (opcode == llvm::Instruction::Mul)
					{
						vfinal *= vl1;
					}
					auto vfinalval = llvm::ConstantInt::get(ci0->getType(), vfinal);
					AddSameReferenceConstant(bop, 0, vfinalval);
				}
				else {
					if (opcode == llvm::Instruction::Sub || opcode == llvm::Instruction::FSub)
					{
						level = SameLevel;


					}
					if (opcode == llvm::Instruction::Add || opcode == llvm::Instruction::FAdd)
					{
						level = SameLevel;
					}

					if ((opcode == llvm::Instruction::Mul || opcode == llvm::Instruction::FDiv || opcode == llvm::Instruction::SDiv || opcode == llvm::Instruction::UDiv) && vl1 == 1)
					{
						level = SameLevel;
						vl1 = 0;
					}

					AddSameReferenceConstant(bop, vl1, op0);

				}



			}
			else
			{
				llvm::ConstantInt*  evalint = DemoteConstantIntToOriginalInt(op1);
				if (evalint)
				{
					llvm::ConstantInt*  ci0 = DemoteConstantIntToOriginalInt(op0);
					int64_t vl1 = evalint->getSExtValue();
					//左边是常量,右边也是常量
					if (ci0)
					{
						int64_t vl0 = ci0->getSExtValue();
						int64_t vfinal = vl0;
						if (opcode == llvm::Instruction::Sub || opcode == llvm::Instruction::FSub)
						{

							vfinal -= vl1;

						}
						if (opcode == llvm::Instruction::Add || opcode == llvm::Instruction::FAdd)
						{
							vfinal += vl1;
						}

						if (opcode == llvm::Instruction::Mul)
						{
							vfinal *= vl1;
						}
						auto vfinalval = llvm::ConstantInt::get(ci0->getType(), vfinal);
						AddSameReferenceConstant(bop, 0, vfinalval);
					}
					else {

						if (opcode == llvm::Instruction::Sub || opcode == llvm::Instruction::FSub)
						{
							level = SameLevel;


						}
						if (opcode == llvm::Instruction::Add || opcode == llvm::Instruction::FAdd)
						{
							level = SameLevel;
						}

						if ((opcode == llvm::Instruction::Mul || opcode == llvm::Instruction::FDiv || opcode == llvm::Instruction::SDiv || opcode == llvm::Instruction::UDiv) && vl1 == 1)
						{
							level = SameLevel;
							vl1 = 0;
						}
						auto otheremu = FindOrCreateEmulatedValueType(op0);
						AddSameReferenceConstant(bop, vl1, op0);
					}
				}
			}
			return true;
		}



		bool TypeReconstructor::HandleInstruction(llvm::Instruction&i)
		{
			auto* s = llvm::dyn_cast<llvm::StoreInst>(&i);
			if (s)
			{
				return HandleStoreInst(s);

			}
			auto* l = llvm::dyn_cast<llvm::LoadInst>(&i);
			if (l)
			{
				return HandleLoadInst(l);

			}
			auto* i2p = llvm::dyn_cast<llvm::IntToPtrInst>(&i);
			if (i2p)
			{
				return HandleIntToPtrInst(i2p);
			}

			auto* tc = llvm::dyn_cast<llvm::TruncInst>(&i);
			if (tc)
			{
				return HandleTruncInst(tc);
			}
			auto* call = llvm::dyn_cast<llvm::CallInst>(&i);
			if (call) {
				return HandleCallInst(call);

			}
			auto* ret = llvm::dyn_cast<llvm::ReturnInst>(&i);
			if (ret) {
				return HandleReturnInst(ret);

			}
			auto* bop = llvm::dyn_cast<llvm::Instruction>(&i);
			if (bop)
			{
				unsigned opcode = bop->getOpcode();
				if (opcode == llvm::Instruction::Sub || opcode == llvm::Instruction::FSub || opcode == llvm::Instruction::Add || opcode == llvm::Instruction::FAdd || opcode == llvm::Instruction::Mul || opcode == llvm::Instruction::FDiv || opcode == llvm::Instruction::SDiv || opcode == llvm::Instruction::UDiv)
				{
					return HandleBinaryOperator(bop);
				}
			}
			return true;
		}



		void TypeReconstructor::setDecoder(retdec::bin2llvmir::Decoder* decoder)
		{
			_decoder = decoder;

		}
		void TypeReconstructor::setAbi(retdec::bin2llvmir::Abi* abi)
		{
			_abi = abi;
		}

		void TypeReconstructor::setConfig(Config* config)
		{
			_config = config;
		}

	}
}