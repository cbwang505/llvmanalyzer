/**
 * @file include/retdec/bin2llvmir/optimizations/idioms/idioms.h
 * @brief Instruction idioms analysis
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#ifndef RETDEC_BIN2LLVMIR_OPTIMIZATIONS_STACKDEOBFUSCATOR_H
#define RETDEC_BIN2LLVMIR_OPTIMIZATIONS_STACKDEOBFUSCATOR_H

#include <list>

#include <llvm/IR/Function.h>
#include <llvm/Pass.h>
#include "retdec/bin2llvmir/providers/abi/abi.h"
#include "retdec/bin2llvmir/providers/config.h"
#include "retdec/bin2llvmir/optimizations/decoder/decoder.h""

namespace retdec {
namespace bin2llvmir {

typedef struct EmulatedStack_t {
	common::Address offset;
	std::size_t length;
	std::uint8_t* data;	
}EmulatedStack,*PEmulatedStack;
/**
 * @brief Instruction idiom analysis.
 */
class LLVM_LIBRARY_VISIBILITY StackDeobfuscator : public llvm::FunctionPass {
public:
	static char ID;
	StackDeobfuscator ();

	virtual bool runOnFunction(llvm::Function & f) override;
	virtual bool doInitialization(llvm::Module & M) override;
	virtual bool doFinalization(llvm::Module & M) override;
	bool runOnStoreInst(llvm::StoreInst* f);
	bool HandleIntToPtrInst(llvm::IntToPtrInst* i2p);
	bool HandleLoadInst(llvm::LoadInst* l);
	bool HandleStoreInst(llvm::StoreInst* s);
	bool AddSameReferenceValue(llvm::Value* op, llvm::Value* other);
	bool InitLocalVar(llvm::Value* val, retdec::common::Address addr);
	std::map<llvm::Value*, std::vector<llvm::Value*>>::iterator findRootValueMap(llvm::Value* op);
	llvm::Value* findRootValue(llvm::Value* op);
	bool modifyAssignedStackData(llvm::Value* ptrSrc, retdec::common::Address valData);
	llvm::Value* DereferenceValuePoint(llvm::Value* op);
	std::vector<EmulatedStack>::iterator findStackRealValue(retdec::common::Address stackoffset);
	std::map<llvm::Value*, retdec::common::Address>::iterator findAssignedValue(llvm::Value* op);
	retdec::common::Address findAssignedAddress(llvm::Value* op);
	bool HandleBinaryOperator(llvm::Instruction* bop);
	bool HandleReturnInst(llvm::ReturnInst* ret);
	bool HandleCallInst(llvm::CallInst* call);
	bool transferUnbalancedCall(llvm::Instruction& i);
	void DeobfuscatorFunction(llvm::Function& F);
	void setAbi(retdec::bin2llvmir::Abi* abi);
	void setConfig(Config* config);
	void setDecoder(retdec::bin2llvmir::Decoder* decoder);
	void setStartAddress(retdec::common::Address startNow);

private:
	bool transferUnbalancedCall(llvm::StoreInst* ai);
	Config* m_config = nullptr;
	//一个root的value和多个同值的value关联
	std::map<llvm::Value*,std::vector<llvm::Value*>> v2u;
	retdec::common::Address stackNow;
	retdec::common::Address _startNow;
	retdec::bin2llvmir::Abi* _abi = nullptr;
	retdec::bin2llvmir::Decoder* _decoder = nullptr;
	Config* _config = nullptr;	
	std::vector<EmulatedStack> stacktbl	;
	//一个root的value和关联的值,如果2个value有不同的值就要有2个元素
	std::map<llvm::Value*, retdec::common::Address> v2o;
	std::map<llvm::Value*, llvm::Value*> p2v;
	bool _switchGenerated = false;
	
};

} // namespace bin2llvmir
} // namespace retdec

#endif
