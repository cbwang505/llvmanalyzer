#pragma once

#include "llvmanalyzer_global.h"
#include "utils.h"
namespace retdec {
	namespace plugin {

		class RetDec;

		struct  LLVMAnalyzer : public action_handler_t
		{
		public:
			RetDec& plg;
			LLVMAnalyzer(RetDec& p);
			virtual int idaapi activate(action_activation_ctx_t*) override;
			virtual action_state_t idaapi update(action_update_ctx_t*) override;
		};
	}
}