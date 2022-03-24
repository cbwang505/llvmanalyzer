#pragma once

#include <memory>
#include <vector>
#include <retdec/common/function.h>
#include <retdec/config/config.h>

#include "token.h"

#define ALIGN(x,a)    (((x)+(a)-1)&~(a-1))

#define LLVMDataTableReserveSize 0x2000
#define LLVMDataTableRoutineReserveSize 0x40
#define LLVMDataTableItemReserveSize 0x80
#define LLVMDataTableItemMaxCount 0x50
#define LLVMFunctionTableReserveSize 0x500
#define LLVMFunctionItemReserveSize 0x500
#define LLVMFunctionItemAlignSize 0x100

namespace retdec {
	namespace plugin {



		// VTBL 
		struct VTBL_info_t
		{
			qstring vtbl_name;
			ea_t ea_begin;
			ea_t ea_end;
			asize_t methods;
		};

		struct vtbl_seg_item
		{
			ea_t ea_vtbl;
			ea_t func_vtbl;
		};
		struct vtbl_seg
		{
			ea_t vtbl_count;
			vtbl_seg_item vtbl_item[0];

		};
		struct LLVMFunctionItem
		{
			int len_func;
			ea_t org_fun;
			ea_t start_fun;
			ea_t end_fun;
		};
		struct LLVMFunctionTable
		{
			int num_func;
			LLVMFunctionItem raw_fuc[1];
		};


		class FunctionInfo
		{
		public:
			std::string code;
			strvec_t idaCode;
		};

		ida_dll_data class Function;
		/**
		 * Plugin's global data.
		 */

		class RetDec
		{

			// Inherited.
			//
		public:
			RetDec();
			virtual ~RetDec();

			bool idaapi run(size_t);
			ssize_t idaapi on_event(void *user_data, int code, va_list va);

			// Plugin information.
			//



			/*/// Plugin information showed in the About box.
			addon_info_t pluginInfo;
			int pluginRegNumber = -1;*/

			// Decompilation.
			//
		public:

			static std::shared_ptr<RetDec>  GetInstance();

			ida_dll_data  bool fullDecompilation();
			Function* selectiveDecompilation(
				ea_t ea,
				bool redecompile,
				bool regressionTests = false
			);
			ida_dll_data Function* selectiveDecompilationToSegment(
				ea_t ea,
				int seg,
				ea_t start = 0,
				ea_t end = 0,
				bool redecompile = false,
				bool regressionTests = false
			);
			ida_dll_data bool manyFunctionDecompilation(retdec::common::FunctionContainer functionslist);

			ida_dll_data Function* selectiveDecompilationAndDisplay(ea_t ea, bool redecompile);
			void displayFunction(Function* f, ea_t ea);

			void modifyFunctions(
				Token::Kind k,
				const std::string& oldVal,
				const std::string& newVal
			);
			void modifyFunction(
				func_t* f,
				Token::Kind k,
				const std::string& oldVal,
				const std::string& newVal
			);

			ea_t getFunctionEa(const std::string& name);
			func_t* getIdaFunction(const std::string& name);
			ea_t getGlobalVarEa(const std::string& name);

			bool idaapi Global_Run_Plugin(size_t arg);
			bool idaapi Global_Term_Plugin();

			/// Currently displayed function.
			Function* fnc = nullptr;


			func_t* decompiledFunction;
			int decompileSegment;
			/// All the decompiled functions.
			std::map<func_t*, Function> fnc2fnc;
			std::map<func_t*, FunctionInfo> fnc2code;
			/// Decompilation config.
			std::shared_ptr<retdec::config::Config> config;
			retdec::common::FunctionContainer decfuncs;
			// UI.
			//
		public:
			TWidget* custViewer = nullptr;
			TWidget* codeViewer = nullptr;

		};

	}
}
