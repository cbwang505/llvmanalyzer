
#ifndef RETDEC_UI_H
#define RETDEC_UI_H

#include "utils.h"
#include "stdafx.h"
namespace retdec {
	namespace plugin {
		
			class RetDec;
			ida_dll_data std::shared_ptr<RetDec>  RetDec_Global_Instance();



			struct functionanalysis_ah_t : public action_handler_t
			{
				RetDec& plg;
				functionanalysis_ah_t(RetDec& p);
				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;

			};

			struct functionanalysisSelect_ah_t : public action_handler_t
			{
				RetDec& plg;
				functionanalysisSelect_ah_t(RetDec& p);
				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;

			};


			struct ida_dll_data fullDecompilation_ah_t : public action_handler_t
			{

				RetDec& plg;
				fullDecompilation_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			struct ida_dll_data jump2asm_ah_t : public action_handler_t
			{

				RetDec& plg;
				jump2asm_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			struct ida_dll_data copy2asm_ah_t : public action_handler_t
			{


				RetDec& plg;
				copy2asm_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			struct ida_dll_data funcComment_ah_t : public action_handler_t
			{

				RetDec& plg;
				funcComment_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			struct ida_dll_data funcSelect_ah_t : public action_handler_t
			{

				RetDec& plg;
				funcSelect_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};
			struct ida_dll_data funcVtable_ah_t : public action_handler_t
			{

				RetDec& plg;
				funcVtable_ah_t(RetDec& p);
				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			struct ida_dll_data renameGlobalObj_ah_t : public action_handler_t
			{

				RetDec& plg;
				renameGlobalObj_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			struct ida_dll_data openXrefs_ah_t : public action_handler_t
			{

				RetDec& plg;
				openXrefs_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			struct ida_dll_data openCalls_ah_t : public action_handler_t
			{

				RetDec& plg;
				openCalls_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			struct ida_dll_data changeFuncType_ah_t : public action_handler_t
			{
				RetDec& plg;
				changeFuncType_ah_t(RetDec& p);

				virtual int idaapi activate(action_activation_ctx_t*) override;
				virtual action_state_t idaapi update(action_update_ctx_t*) override;
			};

			ida_dll_data bool idaapi cv_double(TWidget* cv, int shift, void* ud);
			ida_dll_data void idaapi cv_adjust_place(TWidget* v, lochist_entry_t* loc, void* ud);
			ida_dll_data int idaapi cv_get_place_xcoord(
				TWidget* v,
				const place_t* pline,
				const place_t* pitem,
				void* ud
			);
			ida_dll_data void idaapi cv_location_changed(
				TWidget *v,
				const lochist_entry_t* was,
				const lochist_entry_t* now,
				const locchange_md_t& md,
				void* ud
			);

			static const custom_viewer_handlers_t ui_handlers(
				nullptr,             // keyboard
				nullptr,             // popup
				nullptr,             // mouse_moved
				nullptr,             // click
				cv_double,           // dblclick
				nullptr,             // current position change
				nullptr,             // close
				nullptr,             // help
				cv_adjust_place,     // adjust_place
				cv_get_place_xcoord, // get_place_xcoord
				cv_location_changed, // location_changed
				nullptr              // can_navigate
			);
		}
	}

#endif
