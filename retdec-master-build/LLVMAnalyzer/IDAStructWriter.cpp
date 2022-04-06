#include "stdafx.h"
#include "IDAStructWriter.h"
#include "retdec/utils/io/log.h"
#include <retdec/llvmir2hll/analysis/used_types_visitor.h>
#include <retdec/llvmir2hll/ir/struct_type.h>
#include "retdec/llvmir2hll/ir/array_type.h"
#include <retdec/llvmir2hll/support/struct_types_sorter.h>
#include <regex>



namespace retdec {
	namespace llvmir2hll {
		
		REGISTER_AT_FACTORY("cida", IDA_STRUCT_WRITER_ID, IDAStructWriterFactory, IDAStructWriter::create);
		ShPtr<HLLWriter> IDAStructWriter::create(
			llvm::raw_ostream &out,
			const std::string& outputFormat) {
			return ShPtr<HLLWriter>(new IDAStructWriter(out, outputFormat));
		}



		IDAStructWriter::IDAStructWriter(llvm::raw_ostream& out, const std::string& outputFormat) : HLLWriter(*(new llvm::raw_string_ostream(rawbuf)), outputFormat)
		{
		}

		IDAStructWriter::~IDAStructWriter()
		{
		}

		std::string IDAStructWriter::getId() const {
			return IDA_STRUCT_WRITER_ID;
		}

		uint64_t IDAStructWriter::emitVarWithType(ShPtr<Variable> var, tid_t strucval, Address field_offset)
		{
			struc_t* sptr = get_struc(strucval);
			ShPtr<Type> varType(var->getType());
			uint64_t elelen = DetermineTypeSize(varType);
			var->accept(this);
			flags_t flag = byte_flag();
			flags_t flag2 = byte_flag();
			std::string filename = var->getName();	
			if (isa<ArrayType>(varType))
			{
				ShPtr<ArrayType> arrayType = cast<ArrayType>(varType);
				ShPtr<Type>	elemType1 = arrayType->getContainedType();
				uint64_t elelen2 = DetermineTypeSize(elemType1);
				if (elelen2 == 1)
				{
					flag = byte_flag();
				}
				else if (elelen2 == 2)
				{
					flag = word_flag();
				}
				else if (elelen2 == 4)
				{
					flag = dword_flag();
				}
				else if (elelen2 == 6)
				{
					flag = word_flag();
					flag2 = word_flag();
				}
				else if (elelen2 == 8)
				{
					flag = qword_flag();
				}if (elelen == 6)
				{
					uint64_t dim = elelen/ elelen2;
					std::string filename2 = filename + "_";
					add_struc_member(sptr, filename.c_str(), BADADDR, flag, nullptr, dim*(elelen2 - 2));
					add_struc_member(sptr, filename2.c_str(), BADADDR, flag2, nullptr, dim*(elelen2 - 4));
				}
				else {
					add_struc_member(sptr, filename.c_str(), BADADDR, flag, nullptr, elelen);
				}
			}
			else {

				if (elelen == 1)
				{
					flag = byte_flag();
				}
				else if (elelen == 2)
				{
					flag = word_flag();
				}
				else if (elelen == 4)
				{
					flag = dword_flag();
				}
				else if (elelen == 6)
				{
					flag = word_flag();
					flag2 = word_flag();
				}
				else if (elelen == 8)
				{
					flag = qword_flag();
				}
				if (elelen == 6)
				{
					std::string filename2 = filename+"_";
					add_struc_member(sptr, filename.c_str(), BADADDR, flag, nullptr, elelen-2);
					add_struc_member(sptr, filename2.c_str(), BADADDR, flag2, nullptr, elelen-4);
				}
				else {
					add_struc_member(sptr, filename.c_str(), BADADDR, flag, nullptr, elelen);
				}
			}
			return elelen;
		}

		std::string IDAStructWriter::genNameForUnnamedStruct(const StructTypeVector &usedStructTypes) {
			std::string structName;
			// Create new names until we find a name without a clash.
			do {
				structName = "struct" + std::to_string(++unnamedStructCounter);
				for (const auto &type : usedStructTypes) {
					if (cast<StructType>(type)->getName() == structName) {
						// We have found a clash, so try a different name.
						structName.clear();
						break;
					}
				}
			} while (structName.empty());
			return structName;
		}
		bool IDAStructWriter::emitTargetCode(ShPtr<Module> module)
		{
			this->module = module;

			llvm::Module* m = (llvm::Module*)this->module->getLLVMModule();

			_config = bin2llvmir::ConfigProvider::getConfig(m);

			bin2llvmir::FileImage* image;
			if (!bin2llvmir::FileImageProvider::getFileImage(m, image))
			{
				ERROR_MSG("[ABORT] config file is not available"<<std::endl);

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
				ea_t vtaddr = vt.getAddress();

				llvm::SmallVector<retdec::common::Address, 256> paddrs;
				for (auto& item : vt.items)
				{
					ea_t AddrFunc = item.getTargetFunctionAddress().getValue();
					retdec::plugin::addvtbl2fns(vtaddr, AddrFunc);
					llvm::Function* func = _config->getLlvmFunction(AddrFunc);
					if (func)
					{
						llvm::Instruction& checkAsmProgramCounter = func->getEntryBlock().front();
						ea_t AddrFunc2 = GetPairFunctionAddress(&checkAsmProgramCounter);
						if (AddrFunc2)
						{
							retdec::plugin::addvtbl2fns(vtaddr, AddrFunc2);
						}
					}
				}

				retdec::plugin::addvtbl2fns2seg(vtaddr);
			}

			for (std::pair<const common::Address, std::vector<common::Address>> p : _config->getConfig().vtbl2func)
			{

				llvmir2hll::Address vtaddr = p.first;
				llvm::SmallVector<retdec::common::Address, 256> paddrs;
				for (common::Address compareAddrFunc : p.second)
				{
					ea_t AddrFunc = compareAddrFunc.getValue();
					retdec::plugin::addvtbl2fns(vtaddr, AddrFunc);
					llvm::Function* func = _config->getLlvmFunction(AddrFunc);
					if (func)
					{
						llvm::Instruction& checkAsmProgramCounter = func->getEntryBlock().front();
						ea_t AddrFunc2 = GetPairFunctionAddress(&checkAsmProgramCounter);
						if (AddrFunc2)
						{
							retdec::plugin::addvtbl2fns(vtaddr, AddrFunc2);
						}
					}
				}
				retdec::plugin::addvtbl2fns2seg(vtaddr);
			}
			ShPtr<UsedTypes> usedTypes(UsedTypesVisitor::getUsedTypes(module));
			StructTypeVector usedStructTypes(StructTypesSorter::sort(
				usedTypes->getStructTypes()));
			for (const auto &type : usedStructTypes) {
				std::string structName(type->hasName() ?
					type->getName() : genNameForUnnamedStruct(usedStructTypes));
				structNames[type] = structName;
			}
			// Make sure all structures have a name.
			for (const auto &type : usedStructTypes)
			{
				emitStructIDA(type);
			}

			return true;
		}

		uint64_t IDAStructWriter::GetPairFunctionAddress(llvm::Instruction* insert_before)
		{
			if (insert_before)
			{
				int opc = insert_before->getOpcode();
				auto* store_inst = llvm::dyn_cast<llvm::StoreInst>(insert_before);
				if (store_inst)
				{
					auto val = store_inst->getValueOperand();
					auto ptr = store_inst->getPointerOperand();
					std::string AsmProgramCounterName = ptr->getName();
					if (AsmProgramCounterName.find("asm_program_counter") != std::string::npos)
					{
						llvm::Instruction* checkcall = insert_before->getNextNode();
						if (llvm::CallInst* callfun = llvm::dyn_cast_or_null<llvm::CallInst>(checkcall))
						{

							auto* funcReal = callfun->getCalledFunction();
							if (funcReal)
							{
								uint64_t startReal = _config->getFunctionAddress(funcReal);

								return startReal;
							}

						}
					}
				}
				llvm::CallInst* callfun = llvm::dyn_cast_or_null<llvm::CallInst>(insert_before);
				if (callfun)
				{
					auto* funcReal = callfun->getCalledFunction();
					if (funcReal)
					{
						uint64_t startReal = _config->getFunctionAddress(funcReal);

						return startReal;
					}
				}
			}
			return 0;
		}



		void IDAStructWriter::emitStructIDA(ShPtr<StructType> structType)
		{
			tid_t strucval = 0;

			std::regex vtblreg("Class_vtable_(.+)_type");
			auto i = structNames.find(structType);
			if (i != structNames.end()) {
				std::string rawname = i->second;				
				uint64_t vtbladdr = 0;
				std::cmatch  results;
				std::regex express(vtblreg);
				if (std::regex_search(rawname.c_str(), results, express))
				{
					if (results.size() == 2)
					{
						std::string vtbstr = results[1];
						vtbladdr = strtoull(vtbstr.c_str(), 0, 16);
					}
				}
				strucval = get_struc_id(rawname.c_str());
				if (strucval != BADADDR)
				{
					struc_t* sptr = get_struc(strucval);

					ea_t sz = get_struc_size(sptr);
					if (sz > 0)
					{
						del_struc_members(sptr, 0, sz);
					} 
				//	del_struc(sptr);
				}else
				{
					strucval = add_struc(BADADDR, rawname.c_str());
				}				
				Address field_offset = 0;
				const StructType::ElementTypes& elements = structType->getElementTypes();
				for (StructType::ElementTypes::size_type i = 0; i < elements.size(); ++i) {

					ShPtr<Type> elemType(elements.at(i));
					uint64_t elelen = emitVarWithType(Variable::create("field_" + field_offset.toHexString(), elemType), strucval, field_offset.getValue());
					field_offset += elelen;
				}

				INFO_MSG("Create Vtable Struct : " << rawname << " ,Size :"<< field_offset << std::endl);
			}
			return;
		}


		void IDAStructWriter::visit(ShPtr<Variable> var)
		{

		}
	}
}