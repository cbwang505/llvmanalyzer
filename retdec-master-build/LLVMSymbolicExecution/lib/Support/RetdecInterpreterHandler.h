#pragma once
#include <llvm/IR/Module.h>
#include <retdec/bin2llvmir/providers/config.h>
#include <retdec/common/address.h>
#include <retdec/config/config.h>
#include "klee/Expr/Expr.h"
#include "klee/Core/Interpreter.h"

namespace klee {
	
	
	class RetdecInterpreterHandler :public InterpreterHandler
	{
	public:
		RetdecInterpreterHandler(llvm::Module* m);
		llvm::raw_ostream& getInfoStream() const override;
		std::string getOutputFilename(const std::string& filename) override;
		std::unique_ptr<llvm::raw_fd_ostream> openOutputFile(const std::string& filename) override;
		void incPathsCompleted() override;
		void incPathsExplored(std::uint32_t num) override;
		void processTestCase(const klee::ExecutionState& state, const char* err, const char* suffix) override;
		bool IsOverlapped(POverlappedEmulatedType hasType, POverlappedEmulatedType hasType2);
		void setCurrentVatbleStructType(klee::PVatbleStructType classtpFrom);
		void setThisPointerMemoryObject(klee::MemoryObject *mo) override;
		 void instrumentMemoryOperation(bool isWrite,const klee::MemoryObject *mo, uint64_t offset, uint64_t width) override;
		bool FinalizeStructType();
	private:
		std::unique_ptr<llvm::raw_ostream> m_infoFile;
		MemoryObject * that;
		retdec::bin2llvmir::Config* _config = nullptr;
		PVatbleStructType classtp = nullptr;
		llvm::Module* _module = nullptr;

	};
};