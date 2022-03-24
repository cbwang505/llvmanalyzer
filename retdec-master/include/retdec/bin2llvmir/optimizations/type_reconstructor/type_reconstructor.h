/**
 * @file include/retdec/bin2llvmir/optimizations/idioms/idioms.h
 * @brief Instruction idioms analysis
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#ifndef RETDEC_BIN2LLVMIR_OPTIMIZATIONS_TYPERECONSTRUCTOR_H
#define RETDEC_BIN2LLVMIR_OPTIMIZATIONS_TYPERECONSTRUCTOR_H

#include <list>

#include <llvm/IR/Function.h>
#include <llvm/Pass.h>
#include "retdec/bin2llvmir/providers/abi/abi.h"
#include "retdec/bin2llvmir/providers/config.h"
#include "retdec/bin2llvmir/optimizations/decoder/decoder.h"

namespace retdec {
	namespace bin2llvmir {
		
		typedef struct OverlappedEmulatedType_t {
			uint64_t offset;
			std::size_t length;
			llvm::Type* typefield;
		}OverlappedEmulatedType, *POverlappedEmulatedType;
		typedef enum Emulated_Ref_t
		{
			SameLevel,
			SameLevelAdd,
			SameLevelSub,
			PointerLevel,
			ReferenceLevel,
			DereferenceLevel
		}Emulated_Ref;

		typedef enum ValueUasge_t {
			UndefinedUasge,
			InitialzedUasge,
			DirtyUasge
		}ValueUasge;

		typedef struct EmulatedValueUasge_t {
			Emulated_Ref ref;
			ValueUasge usage;
		}EmulatedValueUasge, *PEmulatedValueUasge;
		typedef struct VatbleStructType_t {
			common::Address vtaddr;
			std::string StructTypeName;
			llvm::StructType* sct;			
			std::unique_ptr<llvm::SmallVector<POverlappedEmulatedType, 256>> typesct;
		}VatbleStructType,*PVatbleStructType;
		typedef class EmulatedValueType {
		public:
			int64_t offsetout;
			int64_t offsetinner;
			Emulated_Ref ref;
			bool valdirty;
			llvm::Value* val;
			EmulatedValueType* left;
			EmulatedValueType* right;
			EmulatedValueType(int64_t _offsetout, int64_t _offsetinner, Emulated_Ref _ref, bool _valdirty, llvm::Value* _val, EmulatedValueType* _left = nullptr, EmulatedValueType* _right = nullptr) :offsetout(_offsetout), offsetinner(_offsetinner), ref(_ref), valdirty(_valdirty), val(_val), left(_left), right(_right) {
			};
		}	*PEmulatedValueType;
		class LLVM_LIBRARY_VISIBILITY FakeFunction {
		public:	
			FakeFunction();
			void dump();
			bool emplace_back(llvm::BasicBlock* b);
			std::shared_ptr<std::vector<llvm::BasicBlock*>> blocks;
		};
		/**
		 * @brief Instruction idiom analysis.
		 */
		class LLVM_LIBRARY_VISIBILITY TypeReconstructor : public llvm::FunctionPass {
		public:
			static char ID;
			TypeReconstructor();
			bool FistInit(llvm::Module& M, bool& value1);
			virtual bool doInitialization(llvm::Module& M) override;
			virtual bool doFinalization(llvm::Module& M) override;
			virtual bool runOnFunction(llvm::Function & f) override;
			bool isRCX(llvm::Value* val);
			bool isRSP(llvm::Value* val);
			bool isRBP(llvm::Value* val);
			bool isAsmProgramCounter(llvm::Value* val);
			PVatbleStructType NeedAnalyzeFunction(llvm::Function& f, common::Address vtaddr, std::string StructTypeName,
			                                      bool& doAnalyze);
			bool AnalyzeFunction(llvm::Function & f);
			bool SameReferenceToRcx(llvm::Value* val, int64_t& offset);
			PEmulatedValueType AddSameReferenceConstant(llvm::Value* op, int64_t offset, llvm::Value* val);
			PVatbleStructType NeedAnalyzeClassStruct(llvm::Function & f);
			bool SkipAsmProgramCounter(llvm::Instruction* i);
			common::Address GetPairFunctionAddress(llvm::Instruction* insert_before);
			bool SplitFunction(llvm::Function& f);
			bool SplitBasicBlock(llvm::BasicBlock* b, FakeFunction* from);
			FakeFunction* ForkRootFunction();
			bool AnalyzeInitialize();
			bool DoAnalyzeFunction(FakeFunction* fakefn);
			bool AddSameReferenceValue(llvm::Value* op, llvm::Value* other);
			PEmulatedValueType FindEmulatedValueType(llvm::Value* val);
			PEmulatedValueType FindOrCreateEmulatedValueType(llvm::Value* val);
			PEmulatedValueType AddPointReferenceValue(llvm::Value* store, llvm::Value* ptr, llvm::Value* val, int64_t offsetou = 0,
				int64_t offsetinner = 0);
			PEmulatedValueType AddPointDeReferenceValue(llvm::Value* ptr, llvm::Value* val, int64_t offsetout = 0, int64_t offsetinner = 0);
			bool IsOverlapped(POverlappedEmulatedType hasType, POverlappedEmulatedType hasType2);
			bool FinalizeStructType(PVatbleStructType classtpFinal);
			bool AddOrReplaceOverlappedEmulatedType(llvm::Type* tp, uint64_t offset, std::size_t length);
			bool GenerateReferenceStruct(PEmulatedValueType val);
			bool GenerateDereferenceStruct(PEmulatedValueType val);
			bool UpdateToRcxTree(llvm::Value* ptr, llvm::Value* val);
			bool SamllDereferenceFromRcxTree(llvm::Value* ptr, llvm::Value* val);
			bool UpdateFromRcxTree(llvm::Value* ptr, llvm::Value* val);
			bool HandleStoreInst(llvm::StoreInst* store_inst);
			bool HandleLoadInst(llvm::LoadInst* load_inst);
			bool HandleIntToPtrInst(llvm::IntToPtrInst* i2_p);
			bool HandleTruncInst(llvm::TruncInst* tc);
			bool HandleCallInst(llvm::CallInst* call);
			void EmplaceEmulatedValueType(llvm::Value* val, PEmulatedValueType reftp);
			llvm::ConstantInt* DemoteConstantIntToOriginalInt(llvm::Value* val);
			bool HandleBinaryOperator(llvm::Instruction* bop);
			bool HandleReturnInst(llvm::ReturnInst* ret);
			bool HandleInstruction(llvm::Instruction& i);
			void setAbi(retdec::bin2llvmir::Abi* abi);
			void setConfig(Config* config);
			void setDecoder(retdec::bin2llvmir::Decoder* decoder);
			void FistInit(llvm::Module* M);
		private:
			bool InitializedPass = false;
			std::vector<const common::Vtable*> vtable;
			std::vector<FakeFunction*> allfuncpath;
			llvm::GlobalVariable* fakercx = nullptr;
			llvm::StructType* sct = nullptr;
			PVatbleStructType classtp = nullptr;
			llvm::Function* root_func = nullptr;
			bool rootrcx_dirty = false;
			Config* _config = nullptr;
			std::map < retdec::common::Address, llvm::SmallVector<retdec::common::Address, 256>> vtbl2addrs;
			std::map<retdec::common::Address, PVatbleStructType> addr2sct;
			std::map<llvm::Value*, llvm::SmallVector<PEmulatedValueType, 256>> val2refs;
			std::map<llvm::Value*, EmulatedValueUasge> val2use;
			retdec::bin2llvmir::Abi* _abi = nullptr;
			retdec::bin2llvmir::Decoder* _decoder = nullptr;
		};

	} // namespace bin2llvmir
} // namespace retdec
#endif