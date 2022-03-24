#pragma once
#include "utils.h"
namespace retdec {
	namespace plugin {
		
			class RetDec;
			class PlainCodeView : public exec_request_t
			{
			private:
				RetDec& plg;
			public:
				~PlainCodeView();
				PlainCodeView(RetDec& p);
				int execute() override;
			};
		}
	}
