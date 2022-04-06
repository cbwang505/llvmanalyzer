#include "RetdecInterpreterHandler.h"

#include <retdec/bin2llvmir/analyses/symbolic_tree.h>
#include <retdec/llvmir2hll/ir/struct_type.h>

#include "klee/Support/ErrorHandling.h"
#include "klee/Support/FileHandling.h"
#include "retdec/utils/binary_path.h"
#include "retdec/utils/filesystem.h"

namespace klee {
	class MemoryObject;

	RetdecInterpreterHandler::RetdecInterpreterHandler(llvm::Module* m)
	{
		_module = m;
		_config = retdec::bin2llvmir::ConfigProvider::getConfig(_module);
		m_infoFile = openOutputFile("info.dump");
	}

	llvm::raw_ostream& RetdecInterpreterHandler::getInfoStream() const
	{
		return *m_infoFile;
	}

	std::string RetdecInterpreterHandler::getOutputFilename(const std::string& filename)
	{
		fs::path filePathInput = _config->getConfig().parameters.getOutputFile();
		fs::path parent_path = filePathInput.parent_path();
		parent_path.append(filename);
		return parent_path.string();
	}

	std::unique_ptr<llvm::raw_fd_ostream> RetdecInterpreterHandler::openOutputFile(const std::string& filename)
	{
		std::string Error;
		std::string filePath = getOutputFilename(filename);
		return 	klee::klee_open_output_file(filePath, Error);
	}

	void RetdecInterpreterHandler::incPathsCompleted()
	{
	}

	void RetdecInterpreterHandler::incPathsExplored(std::uint32_t num)
	{
	}

	void RetdecInterpreterHandler::processTestCase(const klee::ExecutionState& state, const char* err, const char* suffix)
	{
		if (err)
		{
			klee_warning(err);
		}
	}

	bool RetdecInterpreterHandler::IsOverlapped(POverlappedEmulatedType hasType, POverlappedEmulatedType hasType2)
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


	void RetdecInterpreterHandler::setCurrentVatbleStructType(klee::PVatbleStructType classtpFrom)
	{
		classtp = classtpFrom;
	}
	void RetdecInterpreterHandler::setThisPointerMemoryObject(MemoryObject *mo)
	{
		that = mo;
	}
	void RetdecInterpreterHandler::instrumentMemoryOperation(bool isWrite, const MemoryObject *mo, uint64_t offset, uint64_t width)
	{
		if (mo->address == that->address)
		{

			if (isWrite)
			{
				klee_message("This point struct [operation:=>store] offset :=> %d , length :=> %d", offset, width);
			}
			else
			{
				klee_message("This point struct [operation:=>load ] offset :=> %d , length :=> %d", offset, width);
			}

			bool samestruct = false;
			for (auto hasType : *classtp->typesct)
			{
				if (hasType->offset == offset && hasType->offset + hasType->length == offset + width)
				{
					samestruct = true;
				}


			}
			if (!samestruct)
			{
				llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), width);
				POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
					offset,width,tp
				};
				classtp->typesct->emplace_back(typeEmu);
			}
		}
	};
	bool  SortNoOverlappedStruct(POverlappedEmulatedType x, POverlappedEmulatedType y)
	{
		return y->offset > x->offset;

	}
	bool RetdecInterpreterHandler::FinalizeStructType()
	{
		PVatbleStructType classtpFinal = classtp;
		if (classtpFinal&&classtpFinal->typesct->size())
		{

			std::vector<llvm::Type *> ElementsStruct;
			llvm::SmallVector<POverlappedEmulatedType, 256> NoOverlappedStruct;
			llvm::SmallVector<POverlappedEmulatedType, 256> NoOverlappedStructFinal;
			llvm::SmallVector<POverlappedEmulatedType, 256> NoOverlappedStructFinalRet;
			llvm::SmallVector<POverlappedEmulatedType, 256> NoOverlappedStructFinalRetProcess;
			llvm::SmallVector<POverlappedEmulatedType, 256> OverlappedStruct;
			std::map<POverlappedEmulatedType, llvm::SmallVector<POverlappedEmulatedType, 256>> ProcessOverlappedStruct;
			uint64_t minoffset = 4096;
			for (auto& hasType : *classtpFinal->typesct)
			{
				if (hasType->offset <= minoffset)
				{
					minoffset = hasType->offset;
				}
			}
			if (minoffset > 0)
			{
				if (minoffset > 0x1000)
				{
					return false;
				}
				uint64_t ptrwidth = Context::get().getPointerWidth();
				if (minoffset <= ptrwidth)
				{
					llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), minoffset);
					POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
			0,minoffset,tp
					};
					NoOverlappedStructFinal.emplace_back(typeEmu);

				}
				else if (minoffset > ptrwidth)
				{
					uint64_t minpadlen = ptrwidth;
					llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), minpadlen);
					uint64_t arrCount = minoffset / minpadlen;
					if (arrCount == 1)
					{
						POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
				0,minpadlen,tp
						};
						NoOverlappedStructFinal.emplace_back(typeEmu);
						std::size_t  lengthRemain = minoffset - minpadlen;
						if (lengthRemain > 0)
						{
							uint64_t offsetRemain = minpadlen;
							llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), lengthRemain);

							POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
							offsetRemain,lengthRemain,tp
							};
							NoOverlappedStructFinal.emplace_back(typeEmu);
						}

					}
					else
					{
						llvm::Type* tpArr = llvm::ArrayType::get(tp, arrCount);
						uint64_t tpArrLen = minpadlen * arrCount;
						POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
							 0,tpArrLen,tpArr
						};
						NoOverlappedStructFinal.emplace_back(typeEmu);
						std::size_t lengthRemain = minoffset - tpArrLen;
						if (lengthRemain > 0)
						{
							uint64_t offsetRemain = tpArrLen;
							llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), lengthRemain);
							POverlappedEmulatedType typeEmu2 = new OverlappedEmulatedType{
							offsetRemain,lengthRemain,tp
							};
							NoOverlappedStructFinal.emplace_back(typeEmu2);
						}
					}
				}
				else
				{
					llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), minoffset);
					POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
					0,minoffset,tp
					};
					NoOverlappedStruct.emplace_back(typeEmu);
				}
			}
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
						llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), length);

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
						if (maxProcold < maxProc&&maxProcold>max)
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
							llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), lengthRemain);

							POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
							offsetRemain,lengthRemain,tp
							};
							NoOverlappedStructFinal.emplace_back(typeEmu);
						}
						continue;
					}
					else
					{
						llvm::Type* tpArr = llvm::ArrayType::get(hasType->typefield, arrCount);
						uint64_t tpArrLen = hasType->length*arrCount;
						POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
							 hasType->offset,tpArrLen,tpArr
						};
						NoOverlappedStructFinal.emplace_back(typeEmu);
						std::size_t lengthRemain = paddingLen - tpArrLen;
						if (lengthRemain > 0)
						{
							uint64_t offsetRemain = hasType->offset + tpArrLen;
							llvm::Type* tp = llvm::Type::getIntNTy(_module->getContext(), lengthRemain);
							POverlappedEmulatedType typeEmu2 = new OverlappedEmulatedType{
							offsetRemain,lengthRemain,tp
							};
							NoOverlappedStructFinal.emplace_back(typeEmu2);
						}
						continue;
					}

				}
			}
			if (NoOverlappedStructFinal.size())
			{
				std::map<POverlappedEmulatedType, int> arrToFix;
				std::sort(NoOverlappedStructFinal.begin(), NoOverlappedStructFinal.end(), SortNoOverlappedStruct);
				uint64_t nextprocessoffset = 0;
				for (auto& hasType : NoOverlappedStructFinal)
				{

					bool isArray = false;
					bool isArrayFrom = false;
					uint64_t arrCount = 1;
					bool startProcess = false;
					if (hasType->offset >= nextprocessoffset)
					{
						uint64_t dim0 = 0;
						uint64_t elementLength0 = hasType->length;
						llvm::Type* elementType = nullptr;
						if (llvm::ArrayType* tpArr0 = dyn_cast_or_null<llvm::ArrayType>(hasType->typefield))
						{
							isArrayFrom = true;
							elementType = tpArr0->getElementType();
							elementLength0 = _module->getDataLayout().getTypeSizeInBits(elementType);
							dim0 = tpArr0->getNumElements();
							arrCount = dim0;
						}
						else
						{
							if (hasType->length >= 32)
							{
								NoOverlappedStructFinalRet.emplace_back(hasType);
								nextprocessoffset = hasType->offset + hasType->length;
								continue;
							}
						}
						for (auto& hasType1 : NoOverlappedStructFinal)
						{
							if (hasType1->offset == hasType->offset + hasType->length)
							{
								startProcess = true;
							}

							if (startProcess&&hasType1->offset > hasType->offset)
							{
								//合并同时是数组情况
								if (llvm::ArrayType* tpArr = dyn_cast_or_null<llvm::ArrayType>(hasType1->typefield))
								{
									uint64_t elementLength = _module->getDataLayout().getTypeSizeInBits(tpArr->getElementType());
									uint64_t dim = tpArr->getNumElements();
									if (elementLength == elementLength0)
									{
										isArray = true;
										arrCount += dim;
										nextprocessoffset = hasType1->offset + hasType1->length;
									}
									else
									{
										//不是重叠的数组情况,就是下一个要处理的数组
										nextprocessoffset = hasType1->offset;
										break;
									}

								}
								else if (hasType1->length == elementLength0)
								{
									//是重叠的数组情况
									isArray = true;
									arrCount++;
									nextprocessoffset = hasType1->offset + hasType1->length;
								}
								else
								{
									//不是重叠的数组情况,就是下一个要处理的数组
									nextprocessoffset = hasType1->offset;
									break;
								}
							}
						}
						if (isArray)
						{
							if (isArrayFrom)
							{
								POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
								 hasType->offset,hasType->length,elementType
								};
								arrToFix.emplace(typeEmu, arrCount);
							}
							else {
								arrToFix.emplace(hasType, arrCount);
							}
						}
						else
						{
							//没有重叠isArrayFrom还是放入原有数组
							NoOverlappedStructFinalRet.emplace_back(hasType);
						}
					}
				}
				std::sort(NoOverlappedStructFinalRet.begin(), NoOverlappedStructFinalRet.end(), SortNoOverlappedStruct);
				if (NoOverlappedStructFinalRet.size())
				{
					for (auto& hasType : NoOverlappedStructFinalRet)
					{
						NoOverlappedStructFinalRetProcess.emplace_back(hasType);
						for (auto& kv : arrToFix)
						{
							if (kv.first->offset == hasType->offset + hasType->length)
							{
								llvm::Type* tpArr = llvm::ArrayType::get(kv.first->typefield, kv.second);
								uint64_t tpArrLen = kv.first->length*kv.second;
								POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
									 kv.first->offset,tpArrLen,tpArr
								};
								NoOverlappedStructFinalRetProcess.emplace_back(typeEmu);
							}
						}
					}
				}
				else
				{
					for (auto& kv : arrToFix)
					{

						llvm::Type* tpArr = llvm::ArrayType::get(kv.first->typefield, kv.second);
						uint64_t tpArrLen = kv.first->length*kv.second;
						POverlappedEmulatedType typeEmu = new OverlappedEmulatedType{
							 kv.first->offset,tpArrLen,tpArr
						};
						NoOverlappedStructFinalRetProcess.emplace_back(typeEmu);
					}
				}

				if (NoOverlappedStructFinalRetProcess.size())
				{
					std::sort(NoOverlappedStructFinalRetProcess.begin(), NoOverlappedStructFinalRetProcess.end(), SortNoOverlappedStruct);
					for (auto& hasType : NoOverlappedStructFinalRetProcess)
					{
						ElementsStruct.emplace_back(hasType->typefield);
					}
					classtpFinal->sct->setBody(ElementsStruct);
					return true;
				}
			}
		}
		return false;
	}
}