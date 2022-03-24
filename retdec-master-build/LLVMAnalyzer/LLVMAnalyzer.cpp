#include "LLVMAnalyzer.h"

#define DELIT_ALL DELIT_SIMPLE|DELIT_EXPAND|DELIT_DELNAMES

namespace retdec {
	namespace plugin {

		LLVMAnalyzer::LLVMAnalyzer(RetDec& p) : plg(p)
		{
		}


		int LLVMAnalyzer::activate(action_activation_ctx_t*)
		{
			char tmp[_MAX_PATH];
			ea_t hexea = get_screen_ea();
			qstring buff;
			for (int i = 0; i < get_segm_qty(); i++)
			{
				segment_t* seg = getnseg(i);
				if (get_visible_segm_name(&buff, seg) > 0)
				{
					if (seg->start_ea == hexea)
					{
						std::size_t len = seg->end_ea - seg->start_ea;
						del_items(seg->start_ea, DELIT_ALL, len);
						while (hexea < seg->end_ea)
						{
							put_dword(hexea, 0);
							hexea += 4;
						}
						::qsnprintf(tmp, _MAX_PATH, " Segment:=> %s Start  At:=> %x Successfully  Cleared\n", buff.c_str(), hexea);
						INFO_MSG(tmp);
					}
				}

			}

			return false;
		}

		action_state_t LLVMAnalyzer::update(action_update_ctx_t*)
		{

			return AST_ENABLE_ALWAYS;
		}
	}
}