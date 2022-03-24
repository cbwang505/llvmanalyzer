#ifndef RETDEC_LLVMIR2HLL_IDA_WRITER_H
#define RETDEC_LLVMIR2HLL_IDA_WRITER_H

#include <llvm/IR/Function.h>
#include <retdec/common/vtable.h>
#include <retdec/llvmir2hll/support/factory.h>
#include <retdec/llvmir2hll/ir/variable.h>
#include "retdec/llvmir2hll/hll/hll_writer_factory.h"
#include <retdec/llvmir2hll/hll/hll_writer.h>
#include "retdec/bin2llvmir/optimizations/decoder/decoder.h"
#include "utils.h"

namespace retdec {
	namespace llvmir2hll {
		
		class IDAStructWriter : public HLLWriter
		{




		public:
			~IDAStructWriter() override;
			virtual std::string getId() const override;
			static ShPtr<HLLWriter> create(llvm::raw_ostream &out,
				const std::string& outputFormat);
			virtual bool emitTargetCode(ShPtr<Module> module) override;
			uint64_t GetPairFunctionAddress(llvm::Instruction* insert_before);			
			uint64_t emitVarWithType(ShPtr<Variable> var, tid_t strucval, Address field_offset);
			std::string genNameForUnnamedStruct(const StructTypeVector& usedStructTypes);
			void emitStructIDA(ShPtr<StructType> structType);
			void visit(ShPtr<GlobalVarDef> varDef) override
			{
			}

			void visit(ShPtr<Function> func) override
			{
			}

			void visit(ShPtr<AssignStmt> stmt) override
			{
			}

			void visit(ShPtr<BreakStmt> stmt) override
			{
			}

			void visit(ShPtr<CallStmt> stmt) override
			{
			}

			void visit(ShPtr<ContinueStmt> stmt) override
			{
			}

			void visit(ShPtr<EmptyStmt> stmt) override
			{
			}

			void visit(ShPtr<ForLoopStmt> stmt) override
			{
			}

			void visit(ShPtr<UForLoopStmt> stmt) override
			{
			}

			void visit(ShPtr<GotoStmt> stmt) override
			{
			}

			void visit(ShPtr<IfStmt> stmt) override
			{
			}

			void visit(ShPtr<ReturnStmt> stmt) override
			{
			}

			void visit(ShPtr<SwitchStmt> stmt) override
			{
			}

			void visit(ShPtr<UnreachableStmt> stmt) override
			{
			}

			void visit(ShPtr<VarDefStmt> stmt) override
			{
			}

			void visit(ShPtr<WhileLoopStmt> stmt) override
			{
			}

			void visit(ShPtr<AddOpExpr> expr) override
			{
			}

			void visit(ShPtr<AddressOpExpr> expr) override
			{
			}

			void visit(ShPtr<AndOpExpr> expr) override
			{
			}

			void visit(ShPtr<ArrayIndexOpExpr> expr) override
			{
			}

			void visit(ShPtr<AssignOpExpr> expr) override
			{
			}

			void visit(ShPtr<BitAndOpExpr> expr) override
			{
			}

			void visit(ShPtr<BitOrOpExpr> expr) override
			{
			}

			void visit(ShPtr<BitShlOpExpr> expr) override
			{
			}

			void visit(ShPtr<BitShrOpExpr> expr) override
			{
			}

			void visit(ShPtr<BitXorOpExpr> expr) override
			{
			}

			void visit(ShPtr<CallExpr> expr) override
			{
			}

			void visit(ShPtr<CommaOpExpr> expr) override
			{
			}

			void visit(ShPtr<DerefOpExpr> expr) override
			{
			}

			void visit(ShPtr<DivOpExpr> expr) override
			{
			}

			void visit(ShPtr<EqOpExpr> expr) override
			{
			}

			void visit(ShPtr<GtEqOpExpr> expr) override
			{
			}

			void visit(ShPtr<GtOpExpr> expr) override
			{
			}

			void visit(ShPtr<LtEqOpExpr> expr) override
			{
			}

			void visit(ShPtr<LtOpExpr> expr) override
			{
			}

			void visit(ShPtr<ModOpExpr> expr) override
			{
			}

			void visit(ShPtr<MulOpExpr> expr) override
			{
			}

			void visit(ShPtr<NegOpExpr> expr) override
			{
			}

			void visit(ShPtr<NeqOpExpr> expr) override
			{
			}

			void visit(ShPtr<NotOpExpr> expr) override
			{
			}

			void visit(ShPtr<OrOpExpr> expr) override
			{
			}

			void visit(ShPtr<StructIndexOpExpr> expr) override
			{
			}

			void visit(ShPtr<SubOpExpr> expr) override
			{
			}

			void visit(ShPtr<TernaryOpExpr> expr) override
			{
			}

			void visit(ShPtr<Variable> var) override;

			void visit(ShPtr<BitCastExpr> expr) override
			{
			}

			void visit(ShPtr<ExtCastExpr> expr) override
			{
			}

			void visit(ShPtr<FPToIntCastExpr> expr) override
			{
			}

			void visit(ShPtr<IntToFPCastExpr> expr) override
			{
			}

			void visit(ShPtr<IntToPtrCastExpr> expr) override
			{
			}

			void visit(ShPtr<PtrToIntCastExpr> expr) override
			{
			}

			void visit(ShPtr<TruncCastExpr> expr) override
			{
			}

			void visit(ShPtr<ConstArray> constant) override
			{
			}

			void visit(ShPtr<ConstBool> constant) override
			{
			}

			void visit(ShPtr<ConstFloat> constant) override
			{
			}

			void visit(ShPtr<ConstInt> constant) override
			{
			}

			void visit(ShPtr<ConstNullPointer> constant) override
			{
			}

			void visit(ShPtr<ConstString> constant) override
			{
			}

			void visit(ShPtr<ConstStruct> constant) override
			{
			}

			void visit(ShPtr<ConstSymbol> constant) override
			{
			}

			void visit(ShPtr<ArrayType> type) override
			{
			}

			void visit(ShPtr<FloatType> type) override
			{
			}

			void visit(ShPtr<IntType> type) override
			{
			}

			void visit(ShPtr<PointerType> type) override
			{
			}

			void visit(ShPtr<StringType> type) override
			{
			}

			void visit(ShPtr<StructType> type) override
			{
			}

			void visit(ShPtr<FunctionType> type) override
			{
			}

			void visit(ShPtr<VoidType> type) override
			{
			}

			void visit(ShPtr<UnknownType> type) override
			{
			}
		private:
			std::vector<const common::Vtable*> vtable;
			retdec::bin2llvmir::Config* _config ;
			using StructTypeNameMap = std::map<ShPtr<StructType>, std::string>;
		protected:
			virtual std::string getCommentPrefix() override
			{
				return nullptr;
			}

			IDAStructWriter(llvm::raw_ostream &out,
				const std::string& outputFormat = "");
			StructTypeNameMap structNames;
			std::string rawbuf = "";
			std::size_t unnamedStructCounter = 0;
		};



	}
}
#endif