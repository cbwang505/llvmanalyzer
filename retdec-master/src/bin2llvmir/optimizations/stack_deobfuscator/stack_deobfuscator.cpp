#include "retdec/bin2llvmir/optimizations/stack_deobfuscator/stack_deobfuscator.h"



#include "retdec/utils/io/log.h"

using namespace retdec::common;
using namespace retdec::utils;
using namespace llvm;

namespace retdec {
	namespace bin2llvmir {
		char StackDeobfuscator::ID = 0;
		RegisterPass<StackDeobfuscator> IdiomsRegistered(
			"retdec-stack-deobfuscator",
			"Stack deobfuscator optimization",
			false,
			false);
		StackDeobfuscator::StackDeobfuscator() : FunctionPass(ID) {

		}

		bool isRSP(llvm::StringRef const valueName) {
			return  valueName == "esp" || valueName == "rsp";
		}


		void  StackDeobfuscator::DeobfuscatorFunction(llvm::Function& F)
		{
			llvm::Function *tmp = &F;
			for (llvm::Function::iterator bb = tmp->begin(); bb != tmp->end(); bb++) {
				for (llvm::BasicBlock::iterator inst = bb->begin(); inst != bb->end(); inst++)
				{
					if(_switchGenerated)
					{
						return;
					}
					transferUnbalancedCall(*inst);
				}
			}
		}
		void StackDeobfuscator::setDecoder(retdec::bin2llvmir::Decoder* decoder)
		{
			_decoder = decoder;

		}
		void StackDeobfuscator::setAbi(retdec::bin2llvmir::Abi* abi)
		{
			_abi = abi;
		}

		void StackDeobfuscator::setConfig(Config* config)
		{
			_config = config;
		}

		void StackDeobfuscator::setStartAddress(retdec::common::Address startNow)
		{
			_startNow = startNow;
		}
		bool StackDeobfuscator::runOnFunction(llvm::Function& f)
		{
			//default stack ptr
			stackNow = 0x1000;
			DeobfuscatorFunction(f);
			return true;
		}
		bool StackDeobfuscator::runOnStoreInst(llvm::StoreInst* f)
		{
			//default stack ptr
			stackNow = 0x1000;
			//回溯到原始入口
			llvm::StoreInst* startStore = f;
			for (llvm::Instruction* i = f; i; i = i->getPrevNode()) {
				llvm::StoreInst* s = llvm::dyn_cast<llvm::StoreInst>(i);
				if (s)
				{
					auto* ci = llvm::dyn_cast<llvm::ConstantInt>(s->getValueOperand());
					if (ci&&ci->getZExtValue()==_startNow)
					{
						startStore = s;
					}
				}
			}
			for (llvm::Instruction* i = startStore; i; i = i->getNextNode()) {
				if (_switchGenerated)
				{
					return true;
				}
				transferUnbalancedCall(*i);
			}
			return true;
		}

		bool StackDeobfuscator::doInitialization(llvm::Module& M)
		{
			
			return true;
		}

		bool StackDeobfuscator::doFinalization(llvm::Module& M)
		{
			return _switchGenerated;
		}
		bool StackDeobfuscator::AddSameReferenceValue(llvm::Value* op, llvm::Value* other)
		{
			std::map<llvm::Value*, std::vector<llvm::Value*>>::iterator kv = findRootValueMap(op);
			if (kv != v2u.end())
			{
				std::vector<llvm::Value*>::iterator result = std::find(kv->second.begin(), kv->second.end(), other); //查找1
				if (result == kv->second.end())
				{
					kv->second.push_back(other);
				}

			}
			return true;
		}

		bool  StackDeobfuscator::HandleIntToPtrInst(llvm::IntToPtrInst *i2p)
		{
			auto op0 = i2p->getOperand(0);
			AddSameReferenceValue(op0, dyn_cast<Value>(i2p));
			return true;
		}
		bool  StackDeobfuscator::InitLocalVar(llvm::Value* val, retdec::common::Address addr)
		{
			std::vector<llvm::Value*> vals;
			vals.push_back(val);
			//val别名
			v2u.insert(std::make_pair(val, vals));
			//v2s.insert(std::make_pair(llvm::dyn_cast<Value>(l), dt));
			//key要加入root的值引用对象
			v2o.insert(std::make_pair(val, addr));

			return true;
		}

		bool  StackDeobfuscator::HandleLoadInst(llvm::LoadInst *l)
		{
			auto* val = llvm_utils::skipCasts(l->getPointerOperand());
			auto rsp = val->getName();
			if (isRSP(rsp))
			{
				common::Address stackptr(stackNow);
				InitLocalVar(l, stackptr);
			}
			else
			{
				llvm::Value* rootValue = findRootValue(val);
				retdec::common::Address addr = findAssignedAddress(rootValue);
				if (addr.isDefined()) {
					std::vector<EmulatedStack>::iterator stk = findStackRealValue(addr);
					if (stk != stacktbl.end())
					{
						retdec::common::Address srcVal = 0;
						size_t datalen = _abi->getTypeByteSize(_abi->getDefaultType());
						if (stk->data)
						{
							if (datalen == 8)
							{
								uint64_t* buf = reinterpret_cast<uint64_t*>(stk->data);
								srcVal = *buf;

							}
							else
							{
								uint32_t* buf = reinterpret_cast<uint32_t*>(stk->data);
								srcVal = *buf;

							}
						}
						v2o.insert(std::make_pair(llvm::dyn_cast<Value>(l), srcVal));
					}
				}

				p2v.insert(std::make_pair(rootValue, llvm::dyn_cast<Value>(l)));


			}

			return true;
		}
		bool  StackDeobfuscator::HandleStoreInst(llvm::StoreInst *s)
		{
			auto val = s->getValueOperand();
			auto ptr = s->getPointerOperand();
			auto nm = ptr->getName();
			auto* ci = llvm::dyn_cast<llvm::ConstantInt>(val);
			llvm::Value* rootVal = findRootValue(val);
			llvm::Value* rootPtr = findRootValue(ptr);
			retdec::common::Address destVal = 0;
			if (ci)
			{
				destVal = ci->getZExtValue();
			}
			else
			{

				destVal = findAssignedAddress(val);
			}

			if (isRSP(nm))
			{
				if (destVal.isDefined())
				{
					stackNow = destVal;
				}

			}
			else
			{
				if (destVal.isUndefined())
				{
					destVal = 0;
				}
				modifyAssignedStackData(rootPtr, destVal);
			}





			return true;
		}
		std::vector<EmulatedStack>::iterator StackDeobfuscator::findStackRealValue(retdec::common::Address stackoffset)
		{
			for (std::vector<EmulatedStack>::iterator it = stacktbl.begin(); it != stacktbl.end(); it++)
			{
				if (it->offset == stackoffset)
				{
					if (it->data)
					{
						return it;
					}
				}
			}
			return  stacktbl.end();
		}

		llvm::Value* StackDeobfuscator::DereferenceValuePoint(llvm::Value* op)
		{
			for (std::map<Value*, Value*>::iterator it = p2v.begin(); it != p2v.end(); it++)
			{
				if (it->second == op)
				{
					return it->first;
				}
			}

			return nullptr;
		}
		//引用栈指针和栈指针指向的实际值要分开
		std::map<llvm::Value*, retdec::common::Address>::iterator StackDeobfuscator::findAssignedValue(llvm::Value* op)
		{
			for (std::map<llvm::Value*, retdec::common::Address>::iterator it = v2o.begin(); it != v2o.end(); it++)
			{
				if (it->first == op)
				{
					return it;
				}
			}

			return v2o.end();
		}

		retdec::common::Address StackDeobfuscator::findAssignedAddress(llvm::Value* op)
		{
			std::map<llvm::Value*, retdec::common::Address>::iterator it = findAssignedValue(op);
			if (it != v2o.end())
			{
				return it->second;
			}

			return retdec::common::Address::Undefined;
		}
		std::map<Value*, std::vector<llvm::Value*>>::iterator StackDeobfuscator::findRootValueMap(llvm::Value* op)
		{
			if (op)
			{
				for (std::map<Value*, std::vector<Value*>>::iterator kv = v2u.begin(); kv != v2u.end(); kv++)
				{
					for (std::vector<llvm::Value*>::iterator v = kv->second.begin(); v != kv->second.end(); v++)
					{
						llvm::Value* dv = *v;
						if (dv == op)
						{
							return  kv;
						}
					}
				}
			}

			return   v2u.end();
		}
		//一个root的value和多个同值的value关联
		llvm::Value* StackDeobfuscator::findRootValue(llvm::Value* op)
		{
			std::map<Value*, std::vector<Value*>>::iterator kv = findRootValueMap(op);
			if (kv != v2u.end())
			{
				return kv->first;
			}


			return op;
		}

		bool StackDeobfuscator::modifyAssignedStackData(Value* ptrSrc, retdec::common::Address valData)
		{
			retdec::common::Address addr = findAssignedAddress(ptrSrc);
			if (addr.isDefined())
			{
				std::vector<EmulatedStack>::iterator sk = findStackRealValue(addr);
				if (sk != stacktbl.end())
				{
					size_t datalen = sk->length;

					if (sk->data)
					{
						if (datalen == 8)
						{
							uint64_t* buf = reinterpret_cast<uint64_t*>(sk->data);
							uint64_t srcVal = *buf;
							*buf = valData.getValue();
						}
						else
						{
							uint32_t* buf = reinterpret_cast<uint32_t*>(sk->data);
							*buf = static_cast<uint32_t>(valData.getValue());

						}
						return true;
					}
				}
				else
				{



					size_t datalen = _abi->getTypeByteSize(_abi->getDefaultType());
					uint8_t* bufRaw = new uint8_t[datalen];
					if (datalen == 8)
					{
						uint64_t* buf = reinterpret_cast<uint64_t*>(bufRaw);
						*buf = valData.getValue();
					}
					else
					{
						uint32_t* buf = reinterpret_cast<uint32_t*>(bufRaw);
						*buf = static_cast<uint32_t>(valData.getValue());
					}

					EmulatedStack dt = { addr,datalen,bufRaw };
					stacktbl.emplace_back(dt);

					return true;

				}
			}
			return false;
		}
		bool StackDeobfuscator::HandleBinaryOperator(llvm::Instruction* bop)
		{
			unsigned opcode = bop->getOpcode();
			Value* op0 = bop->getOperand(0);
			Value* op1 = bop->getOperand(1);
			auto* pi = llvm::dyn_cast<llvm::LoadInst>(op0);
			auto* ci = llvm::dyn_cast<llvm::ConstantInt>(op1);
			llvm::Value* rootSrc = findRootValue(op0);
			if (ci)
			{
				uint64_t vl = ci->getZExtValue();
				retdec::common::Address addr = findAssignedAddress(rootSrc);
				if (addr.isDefined()) {
					if (opcode == llvm::Instruction::Sub || opcode == llvm::Instruction::FSub)
					{

						addr = addr - vl;
					}
					if (opcode == llvm::Instruction::Add || opcode == llvm::Instruction::FAdd)
					{
						addr = addr + vl;
					}
					InitLocalVar(dyn_cast<Value>(bop), addr);

				}

			}


			return true;
		}
		bool StackDeobfuscator::HandleReturnInst(llvm::ReturnInst* ret)
		{
			size_t returnsize = _abi->getTypeByteSize(_abi->getDefaultType());
			std::vector<EmulatedStack>::iterator sk = findStackRealValue(stackNow );
			if (sk != stacktbl.end())
			{
				size_t datalen = sk->length;
				retdec::common::Address addrcall = retdec::common::Address::Undefined;
				if (sk->data)
				{
					if (datalen == 8)
					{
						uint64_t* buf = reinterpret_cast<uint64_t*>(sk->data);
						addrcall = *buf;

					}
					else
					{
						uint32_t* buf = reinterpret_cast<uint32_t*>(sk->data);
						addrcall = *buf;
					}

					if (addrcall.isDefined()&& addrcall!=0&&_decoder->validRange(addrcall))
					{
						
						llvm::Function* tFnc = _decoder->createFunction(addrcall,false);						
						auto* c = CallInst::Create(tFnc);
						c->insertBefore(ret);
						_decoder->addJumpTargets(addrcall, JumpTarget::eType::LEFTOVER, _startNow);
						_switchGenerated = true;
					}
					return true;
				}
			}
			return true;
		}

		bool StackDeobfuscator::HandleCallInst(llvm::CallInst* call)
		{
			size_t returnsize = _abi->getTypeByteSize(_abi->getDefaultType());
			std::vector<EmulatedStack>::iterator sk = findStackRealValue(stackNow);
			if (sk != stacktbl.end())
			{
				size_t datalen = sk->length;
				retdec::common::Address addrcall = retdec::common::Address::Undefined;
				if (sk->data)
				{
					if (datalen == 8)
					{
						uint64_t* buf = reinterpret_cast<uint64_t*>(sk->data);
						addrcall = *buf;

					}
					else
					{
						uint32_t* buf = reinterpret_cast<uint32_t*>(sk->data);
						addrcall = *buf;
					}

					if (addrcall.isDefined() && addrcall != 0 && _decoder->validRange(addrcall))
					{

						llvm::Function* tFnc = _decoder->createFunction(addrcall, true);
						auto* c = CallInst::Create(tFnc);
						c->insertBefore(call);
						call->eraseFromParent();
						_decoder->addJumpTargets(addrcall, JumpTarget::eType::CONTROL_FLOW_CALL_TARGET, _startNow);
						_switchGenerated = true;
					}
					return true;
				}
			}
			return true;
		}

		bool StackDeobfuscator::transferUnbalancedCall(llvm::Instruction&i)
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
				if (opcode == llvm::Instruction::Sub || opcode == llvm::Instruction::FSub || opcode == llvm::Instruction::Add || opcode == llvm::Instruction::FAdd)
				{
					return HandleBinaryOperator(bop);
				}
			}
			return true;
		}

	}
}