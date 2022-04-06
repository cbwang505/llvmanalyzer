
#include "config.h"
#include "place.h"
#include "retdec.h"
#include "ui.h"




namespace retdec {
	namespace plugin {
		static const char* funcSelect_ah_actionName = "retdec:ActionFunctionSelect";
		static const char* funcVtable_ah_actionName = "retdec:ActionVtableSelect";
		static const char* funcMsg_ah_actionName = "retdec:AFXMSGMAPENTRYSelect";
		std::shared_ptr<retdec::config::Config> getGlobalConfing();
		//
		//==============================================================================
		// fullDecompilation_ah_t
		//==============================================================================
		//

		fullDecompilation_ah_t::fullDecompilation_ah_t(RetDec& p)
			: plg(p)
		{

		}

		int idaapi fullDecompilation_ah_t::activate(action_activation_ctx_t*)
		{
			plg.fullDecompilation();
			return false;
		}

		action_state_t idaapi fullDecompilation_ah_t::update(action_update_ctx_t*)
		{
			return AST_ENABLE_ALWAYS;
		}

		//
		//==============================================================================
		// jump2asm_ah_t
		//==============================================================================
		//

		jump2asm_ah_t::jump2asm_ah_t(RetDec& p)
			: plg(p)
		{

		}

		int idaapi jump2asm_ah_t::activate(action_activation_ctx_t* ctx)
		{
			auto* place = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
				ctx->widget,
				false, // mouse
				nullptr, // x
				nullptr // y
			));
			if (place == nullptr)
			{
				return false;
			}

			jumpto(place->toea(), 0, UIJMP_ACTIVATE | UIJMP_IDAVIEW);
			return false;
		}

		action_state_t idaapi jump2asm_ah_t::update(action_update_ctx_t* ctx)
		{
			return ctx->widget == plg.custViewer
				? AST_ENABLE_FOR_WIDGET : AST_DISABLE_FOR_WIDGET;
		}

		//
		//==============================================================================
		// copy2asm_ah_t
		//==============================================================================
		//

		copy2asm_ah_t::copy2asm_ah_t(RetDec& p)
			: plg(p)
		{

		}

		int idaapi copy2asm_ah_t::activate(action_activation_ctx_t*)
		{
			static const char* text = "Copying pseudocode to disassembly"
				" will destroy existing comments.\n"
				"Do you want to continue?";
			if (ask_yn(ASKBTN_NO, text) == ASKBTN_YES)
			{
				for (auto& p : plg.fnc->toLines())
				{
					ea_t addr = p.second;
					auto& line = p.first;

					delete_extra_cmts(addr, E_PREV);
					bool anteriorCmt = true;
					add_extra_cmt(addr, anteriorCmt, "%s", line.c_str());
				}

				// Focus to IDA view.
				auto* place = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
					plg.custViewer,
					false, // mouse
					nullptr, // x
					nullptr // y
				));
				if (place != nullptr)
				{
					jumpto(place->toea(), 0, UIJMP_ACTIVATE | UIJMP_IDAVIEW);
				}
			}
			return false;
		}

		action_state_t idaapi copy2asm_ah_t::update(action_update_ctx_t* ctx)
		{
			return ctx->widget == plg.custViewer
				? AST_ENABLE_FOR_WIDGET : AST_DISABLE_FOR_WIDGET;
		}

		//
		//==============================================================================
		// funcComment_ah_t
		//==============================================================================
		//

		funcComment_ah_t::funcComment_ah_t(RetDec& p)
			: plg(p)
		{

		}

		int idaapi funcComment_ah_t::activate(action_activation_ctx_t*)
		{
			auto* fnc = plg.fnc ? plg.fnc->fnc() : nullptr;
			if (fnc == nullptr)
			{
				return false;
			}

			qstring qCmt;
			get_func_cmt(&qCmt, fnc, false);

			qstring buff;
			if (ask_text(
				&buff,
				MAXSTR,
				qCmt.c_str(),
				"Please enter function comment (max %d characters)",
				MAXSTR))
			{
				set_func_cmt(fnc, buff.c_str(), false);
				plg.selectiveDecompilationAndDisplay(fnc->start_ea, true);
			}

			return false;
		}

		std::string defaultTypeString();



		void addfunctiontoanalyze(func_t* func, RetDec& plg, ea_t fakevtbl = 0)
		{
			if (func)
			{
				insn_t inst;
				int ret = decode_insn(&inst, func->start_ea);
				if (ret > 0)
				{
					op_t addrop = inst.ops[0];
					if (addrop.addr)
					{
						flags_t fg = get_flags(addrop.addr);
						if (is_func(fg))
						{

							func_t* func1 = get_func(addrop.addr);
							addfunctiontoanalyze(func1, plg, fakevtbl);
						}
					}
				}
				qstring qFncName;
				get_func_name(&qFncName, func->start_ea);
				std::string fncName = qFncName.c_str();
				std::replace(fncName.begin(), fncName.end(), '.', '_');

				retdec::common::Function ccFnc(fncName);
				ccFnc.setStart(func->start_ea);
				ccFnc.setEnd(func->end_ea);
				// TODO: return type is always set to default: ugly, make it better.
				ccFnc.returnType.setLlvmIr(defaultTypeString());

				qstring qCmt;
				if (get_func_cmt(&qCmt, func, false) > 0)
				{
					ccFnc.setComment(qCmt.c_str());
				}

				qstring qDemangled;
				if (demangle_name(&qDemangled, fncName.c_str(), MNG_SHORT_FORM) > 0)
				{
					ccFnc.setDemangledName(qDemangled.c_str());
				}
				auto* findfunc = plg.decfuncs.getFunctionByStartAddress(func->start_ea);
				if (!findfunc)
				{
					if (fakevtbl)
					{
						addvtbl2fnsanalyze(fakevtbl, func->start_ea);
					}
					qstring finmsg;
					finmsg.sprnt("Add Function :=> %s To Analyze\n", qFncName.c_str());
					INFO_MSG(finmsg.c_str());
					plg.decfuncs.insert(ccFnc);
				}
				else
				{
					qstring finmsg;
					finmsg.sprnt("Already Have Added Function :=> %s To Analyze\n", qFncName.c_str());
					INFO_MSG(finmsg.c_str());
				}
			}
		}
		action_state_t idaapi funcComment_ah_t::update(action_update_ctx_t* ctx)
		{
			return ctx->widget == plg.custViewer
				? AST_ENABLE_FOR_WIDGET : AST_DISABLE_FOR_WIDGET;
		}

		funcSelect_ah_t::funcSelect_ah_t(RetDec& p) : plg(p)
		{
		}

		int funcSelect_ah_t::activate(action_activation_ctx_t* ctx)
		{
			twidget_type_t viewtype = ctx->widget_type;
			if (viewtype == BWN_DISASM)
			{
				func_t* func = nullptr;
				flags_t fg = get_flags(ctx->cur_ea);
				if (is_func(fg))
				{
					func = get_func(ctx->cur_ea);
				}
				else
				{
					fg = get_flags(ctx->cur_extracted_ea);
					if (is_func(fg))
					{
						func = get_func(ctx->cur_extracted_ea);
					}
				}

				addfunctiontoanalyze(func, plg);
				return false;
			}
			if (viewtype == BWN_PSEUDOCODE)
			{
				if (ctx->cur_func)
				{
					addfunctiontoanalyze(ctx->cur_func, plg);
					return false;

				}
			}
			if (viewtype == BWN_FUNCS)
			{
				if (ctx->chooser_selection.size())
				{
					for (int idx : ctx->chooser_selection)
					{
						func_t* func = getn_func(idx);
						addfunctiontoanalyze(func, plg);
					}
					return false;

				}
			}
			return false;
		}

		action_state_t funcSelect_ah_t::update(action_update_ctx_t* ctx)
		{
			switch (ctx->widget_type) {
			case BWN_FUNCS:
			{
				attach_action_to_popup(ctx->widget, nullptr, funcSelect_ah_actionName);
				return AST_ENABLE_FOR_WIDGET;
			}
			case BWN_DISASM:
			case BWN_PSEUDOCODE:
			{
				return AST_ENABLE_FOR_WIDGET;
			}
			default:
				return AST_DISABLE_FOR_WIDGET;
			}
			return AST_DISABLE_FOR_WIDGET;
		}


		//---------------------------------------------------------------------------
// VTBL code parsing
//---------------------------------------------------------------------------

		bool get_text_disasm(ea_t ea, qstring& rv) {
			rv.clear();

			if (!generate_disasm_line(&rv, ea))
				return false;

			tag_remove(&rv);

			return true;
		}

		static bool check_vtable_load_instruction(ea_t ea_code) {

			qstring dism;
			if (!get_text_disasm(ea_code, dism))
				return false;

			if (dism.find("mov ") == 0 && dism.find(" offset ") != dism.npos)
				return true;

			if (dism.find("lea") == 0)
				return true;

			return false;
		}

		//---------------------------------------------------------------------------
		// Try to find vtable at the specified address
		//---------------------------------------------------------------------------
		static bool get_vtbl_info(ea_t ea_address, VTBL_info_t &vtbl_info)
		{
			flags_t flags = get_flags(ea_address);
			if (has_xref(flags) && has_any_name(flags) && (isEa(flags) || is_unknown(flags))) {
				bool is_move_xref = false;

				ea_t ea_code_ref = get_first_dref_to(ea_address);
				if (ea_code_ref && ea_code_ref != BADADDR) {
					do {
						if (is_code(get_flags(ea_code_ref)) && check_vtable_load_instruction(ea_code_ref)) {
							is_move_xref = true;
							break;
						}

						ea_code_ref = get_next_dref_to(ea_address, ea_code_ref);

					} while (ea_code_ref && ea_code_ref != BADADDR);
				}

				if (is_move_xref) {
					memset(&vtbl_info, 0, sizeof(VTBL_info_t));

					get_ea_name(&vtbl_info.vtbl_name, ea_address);

					ea_t ea_start = vtbl_info.ea_begin = ea_address;

					while (true) {
						flags_t index_flags = get_flags(ea_address);
						if (!(isEa(index_flags) || is_unknown(index_flags)))
							break;

						ea_t ea_index_value = getEa(ea_address);
						if (!ea_index_value || ea_index_value == BADADDR)
							break;

						if (ea_address != ea_start && has_xref(index_flags))
							break;

						flags_t value_flags = get_flags(ea_index_value);
						if (!is_code(value_flags)) {
							break;
						}
						else {
							if (is_unknown(index_flags)) {
#ifndef __EA64__
								create_dword(ea_address, sizeof(ea_t));
#else
								create_qword(ea_address, sizeof(ea_t));
#endif
							}
						}

						ea_address += sizeof(ea_t);
					}

					if ((vtbl_info.methods = ((ea_address - ea_start) / sizeof(ea_t))) > 0) {
						vtbl_info.ea_end = ea_address;
						return true;
					}
				}
			}

			return false;
		}

		//---------------------------------------------------------------------------
		// Try to find and parse vtable at the specified address
		//---------------------------------------------------------------------------
		static void process_vtbl(ea_t &ea_sect, RetDec& plg)
		{
			VTBL_info_t vftable_info_t;
			// try to parse vtable at this address
			if (get_vtbl_info(ea_sect, vftable_info_t))
			{
				ea_sect = vftable_info_t.ea_end;

				if (vftable_info_t.methods >= 1) {
					for (ea_t ea_start = vftable_info_t.ea_begin; ea_start < vftable_info_t.ea_end; ea_start += sizeof(ea_t))
					{
						ea_t ea_func = getEa(ea_start);
						flags_t fg = get_flags(ea_func);
						if (is_func(fg))
						{
							func_t* func = get_func(ea_func);

							addfunctiontoanalyze(func, plg);
						}
					}
					INFO_MSG("Found Vtable Methods Count :=> " << vftable_info_t.methods << " To Analyze\n");
					ea_sect = vftable_info_t.ea_end;
					return;
				}
				else
				{
					INFO_MSG("No Vtable Methods To Analyze\n");
				}
			}
			else
			{
				INFO_MSG("No Vtable Methods To Analyze\n");
			}

			return;
		}


		funcVtable_ah_t::funcVtable_ah_t(RetDec& p) : plg(p)
		{
		}

		int funcVtable_ah_t::activate(action_activation_ctx_t* ctx)
		{
			twidget_type_t viewtype = ctx->widget_type;
			if (viewtype == BWN_DISASM)
			{


				process_vtbl(ctx->cur_ea, plg);
			}
			return false;
		}

		action_state_t funcVtable_ah_t::update(action_update_ctx_t* ctx)
		{
			switch (ctx->widget_type) {
			case BWN_DISASM:
			{
				return AST_ENABLE_FOR_WIDGET;
			}
			default:
				return AST_DISABLE_FOR_WIDGET;
			}
			return AST_DISABLE_FOR_WIDGET;
		}

		funcAfxMsgMapEntry_ah_t::funcAfxMsgMapEntry_ah_t(RetDec& p) : plg(p)
		{
		}


		tid_t CreateAfxMsgMapEntryStruct()
		{
			qstring rawname = "AFX_MSGMAP_ENTRY";
			tid_t strucval = get_struc_id(rawname.c_str());
			if (strucval != BADADDR)
			{
				return strucval;
			}
			else
			{
				strucval = add_struc(BADADDR, rawname.c_str());
				struc_t* sptr = get_struc(strucval);
				flags_t flag = dword_flag();

#ifdef __EA64__
				flags_t flag2 = qword_flag();
#else // __EA64__
				flags_t flag2 = dword_flag();
#endif // __EA64__

				add_struc_member(sptr, "nMessage", BADADDR, flag, nullptr, 4);
				add_struc_member(sptr, "nCode", BADADDR, flag, nullptr, 4);
				add_struc_member(sptr, "nID", BADADDR, flag, nullptr, 4);
				add_struc_member(sptr, "nLastID", BADADDR, flag, nullptr, 4);
				add_struc_member(sptr, "nSig", BADADDR, flag, nullptr, 4);
				add_struc_member(sptr, "pfn", BADADDR, flag2, nullptr, sizeof(ea_t));
				return strucval;
			}
		}
		

		int funcAfxMsgMapEntry_ah_t::activate(action_activation_ctx_t* ctx)
		{
			twidget_type_t viewtype = ctx->widget_type;
			if (viewtype == BWN_DISASM)
			{

				tid_t strucval = CreateAfxMsgMapEntryStruct();
				std::size_t sctheadlen = 20;
				std::size_t sctlen = sctheadlen + sizeof(ea_t);
				for (ea_t start = ctx->cur_ea; start < BADADDR; start += sctlen)
				{
					ea_t ea_func = getEa(start + sctheadlen);
					flags_t fgsct = get_flags(start);
					flags_t fg = get_flags(ea_func);
					if (is_struct(fgsct))
					{
						opinfo_t opi;
						bool ok = get_opinfo(&opi, start, 0, fgsct);
						if (ok)
						{
							if (opi.tid == strucval)
							{
								if (is_func(fg))
								{
									func_t* func = get_func(ea_func);
									addfunctiontoanalyze(func, plg, ctx->cur_ea);
									continue;
								}
								else
								{
									break;
								}

							}
							else
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
					else {
						if (is_func(fg))
						{
							func_t* func = get_func(ea_func);
							addfunctiontoanalyze(func, plg, ctx->cur_ea);
							if (!create_struct(start, sctlen, strucval))
							{
								break;
							}
						}
						else
						{
							if (ea_func == 0 & start != ctx->cur_ea)
							{
								create_struct(start, sctlen, strucval);

							}
							break;
						}
					}
				}
			}
			return false;
		}

		action_state_t funcAfxMsgMapEntry_ah_t::update(action_update_ctx_t* ctx)
		{
			switch (ctx->widget_type) {
			case BWN_DISASM:
			{
				return AST_ENABLE_FOR_WIDGET;
			}
			default:
				return AST_DISABLE_FOR_WIDGET;
			}
			return AST_DISABLE_FOR_WIDGET;
		}

		//
		//==============================================================================
		// renameGlobalObj_ah_t
		//==============================================================================
		//

		renameGlobalObj_ah_t::renameGlobalObj_ah_t(RetDec& p)
			: plg(p)
		{

		}

		int idaapi renameGlobalObj_ah_t::activate(action_activation_ctx_t* ctx)
		{
			auto* place = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
				ctx->widget,
				false, // mouse
				nullptr, // x
				nullptr // y
			));
			auto* token = place ? place->token() : nullptr;
			if (token == nullptr)
			{
				return false;
			}

			std::string askString;
			ea_t addr = BADADDR;
			if (token->kind == Token::Kind::ID_FNC)
			{
				askString = "Please enter function name";
				addr = plg.getFunctionEa(token->value);
			}
			else if (token->kind == Token::Kind::ID_GVAR)
			{
				askString = "Please enter global variable name";
				addr = plg.getGlobalVarEa(token->value);
			}
			if (addr == BADADDR)
			{
				return false;
			}

			qstring qNewName = token->value.c_str();
			if (!ask_str(&qNewName, HIST_IDENT, "%s", askString.c_str())
				|| qNewName.empty())
			{
				return false;
			}
			std::string newName = qNewName.c_str();
			if (newName == token->value)
			{
				return false;
			}

			if (set_name(addr, newName.c_str()) == false)
			{
				return false;
			}

			std::string oldName = token->value;
			plg.modifyFunctions(token->kind, oldName, newName);
			std::shared_ptr<retdec::config::Config> cfg = getGlobalConfing();
			fillConfig(*cfg);

			return false;
		}

		action_state_t idaapi renameGlobalObj_ah_t::update(action_update_ctx_t* ctx)
		{
			return ctx->widget == plg.custViewer
				? AST_ENABLE_FOR_WIDGET : AST_DISABLE_FOR_WIDGET;
		}

		//
		//==============================================================================
		// openXrefs_ah_t
		//==============================================================================
		//

		openXrefs_ah_t::openXrefs_ah_t(RetDec& p)
			: plg(p)
		{

		}

		int idaapi openXrefs_ah_t::activate(action_activation_ctx_t* ctx)
		{
			auto* place = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
				ctx->widget,
				false, // mouse
				nullptr, // x
				nullptr // y
			));
			auto* token = place ? place->token() : nullptr;
			if (token == nullptr)
			{
				return false;
			}

			ea_t ea = BADADDR;
			if (token->kind == Token::Kind::ID_FNC)
			{
				ea = plg.getFunctionEa(token->value);
			}
			else if (token->kind == Token::Kind::ID_GVAR)
			{
				ea = plg.getGlobalVarEa(token->value);
			}
			if (ea == BADADDR)
			{
				return false;
			}

			open_xrefs_window(ea);
			return false;
		}

		action_state_t idaapi openXrefs_ah_t::update(action_update_ctx_t* ctx)
		{
			return ctx->widget == plg.custViewer
				? AST_ENABLE_FOR_WIDGET : AST_DISABLE_FOR_WIDGET;
		}

		//
		//==============================================================================
		// openCalls_ah_t
		//==============================================================================
		//

		openCalls_ah_t::openCalls_ah_t(RetDec& p)
			: plg(p)
		{

		}

		int idaapi openCalls_ah_t::activate(action_activation_ctx_t* ctx)
		{
			auto* place = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
				ctx->widget,
				false, // mouse
				nullptr, // x
				nullptr // y
			));
			auto* token = place ? place->token() : nullptr;
			if (token == nullptr)
			{
				return false;
			}

			ea_t ea = BADADDR;
			if (token->kind == Token::Kind::ID_FNC)
			{
				ea = plg.getFunctionEa(token->value);
			}
			else if (token->kind == Token::Kind::ID_GVAR)
			{
				ea = plg.getGlobalVarEa(token->value);
			}
			if (ea == BADADDR)
			{
				return false;
			}

			open_calls_window(ea);
			return false;
		}

		action_state_t idaapi openCalls_ah_t::update(action_update_ctx_t* ctx)
		{
			return ctx->widget == plg.custViewer
				? AST_ENABLE_FOR_WIDGET : AST_DISABLE_FOR_WIDGET;
		}

		//
		//==============================================================================
		// changeFuncType_ah_t
		//==============================================================================
		//

		changeFuncType_ah_t::changeFuncType_ah_t(RetDec& p)
			: plg(p)
		{

		}

		int idaapi changeFuncType_ah_t::activate(action_activation_ctx_t* ctx)
		{
			auto* place = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
				ctx->widget,
				false, // mouse
				nullptr, // x
				nullptr // y
			));
			auto* token = place ? place->token() : nullptr;
			if (token == nullptr)
			{
				return false;
			}

			func_t* fnc = nullptr;
			if (token->kind == Token::Kind::ID_FNC)
			{
				fnc = plg.getIdaFunction(token->value);
			}
			if (fnc == nullptr)
			{
				return false;
			}

			qstring buf;
			int flags = PRTYPE_1LINE | PRTYPE_SEMI;
			if (!print_type(&buf, fnc->start_ea, flags))
			{
				qstring qFncName;
				get_func_name(&qFncName, fnc->start_ea);;
				WARNING_GUI("Cannot change declaration for: "
					<< qFncName.c_str() << "\n"
				);
			}

			std::string askString = "Please enter type declaration:";

			qstring qNewDeclr = buf;
			if (!ask_str(&qNewDeclr, HIST_IDENT, "%s", askString.c_str())
				|| qNewDeclr.empty())
			{
				return false;
			}

			if (apply_cdecl(nullptr, fnc->start_ea, qNewDeclr.c_str()))
			{
				plg.selectiveDecompilationAndDisplay(fnc->start_ea, true);
			}
			else
			{
				WARNING_GUI("Cannot change declaration to: "
					<< qNewDeclr.c_str() << "\n"
				);
			}

			return false;
		}

		action_state_t idaapi changeFuncType_ah_t::update(action_update_ctx_t* ctx)
		{
			return ctx->widget == plg.custViewer
				? AST_ENABLE_FOR_WIDGET : AST_DISABLE_FOR_WIDGET;
		}

		//
		//==============================================================================
		// on_event
		//==============================================================================
		//
		bool isAfxMsgMapEntryStruct(ea_t start)
		{
			std::size_t sctheadlen = 20;
			ea_t ea_code = getEa(start);
			flags_t fg_code = get_flags(ea_code);
			if (has_xref(fg_code)|| is_func(fg_code))
			{
				return false;
			}
			ea_t ea_func = getEa(start + sctheadlen);
			flags_t fg = get_flags(ea_func);
			if (is_func(fg))
			{
				return true;
			}

			return false;
		}
		/**
		 * User interface hook.
		 */
		ssize_t idaapi ui_callback(void *user_data, int code, va_list va)
		{

			std::shared_ptr<RetDec> ctxInst = RetDec_Global_Instance();
			static const char* fullDecompilation_ah_actionName = "retdec:ActionFullDecompilation";
			static const char* fullDecompilation_ah_actionLabel = "Create C file RetDec...";
			static const char* fullDecompilation_ah_actionHotkey = "Ctrl+Shift+D";


			static const char* jump2asm_ah_actionName = "retdec:ActionJump2Asm";
			static const char* jump2asm_ah_actionLabel = "Jump to assembly";
			static const char* jump2asm_ah_actionHotkey = "A";

			static const char* copy2asm_ah_actionName = "retdec:ActionCopy2Asm";
			static const char* copy2asm_ah_actionLabel = "Copy to assembly";
			static const char* copy2asm_ah_actionHotkey = "";

			static const char* funcComment_ah_actionName = "retdec:ActionFunctionComment";
			static const char* funcComment_ah_actionLabel = "Edit func comment";
			static const char* funcComment_ah_actionHotkey = ";";

			static const char* renameGlobalObj_ah_actionName = "retdec:RenameGlobalObj";
			static const char* renameGlobalObj_ah_actionLabel = "Rename global object";
			static const char* renameGlobalObj_ah_actionHotkey = "R";

			static const char* openXrefs_ah_actionName = "retdec:OpenXrefs";
			static const char* openXrefs_ah_actionLabel = "Open xrefs";
			static const char* openXrefs_ah_actionHotkey = "X";


			static const char* openCalls_ah_actionName = "retdec:OpenCalls";
			static const char* openCalls_ah_actionLabel = "Open calls";
			static const char* openCalls_ah_actionHotkey = "C";

			static const char* changeFuncType_ah_actionName = "retdec:ChangeFuncType";
			static const char* changeFuncType_ah_actionLabel = "Change function type";
			static const char* changeFuncType_ah_actionHotkey = "T";

			switch (code)
			{
				// IDA is populating the RetDec menu (right-click menu) for a widget.
				// We can attach action to popup - i.e. create menu on the fly.
			case ui_populating_widget_popup:
			{

				// Continue only if event was triggered in our widget.
				TWidget* view = va_arg(va, TWidget*);
				TPopupMenu* popup = va_arg(va, TPopupMenu*);
				if (view)
				{
					twidget_type_t viewtype = get_widget_type(view);
					if (viewtype == BWN_DISASM)
					{
						ea_t ea_address = get_screen_ea();
						ea_t ea_func = getEa(ea_address);
						flags_t fg = get_flags(ea_address);
						flags_t fg1 = get_flags(ea_func);
						if (is_func(fg)| is_func(fg1))
						{
							attach_action_to_popup(view, popup, funcSelect_ah_actionName);
						}					
						
						VTBL_info_t vftable_info_t;
						if (get_vtbl_info(ea_address, vftable_info_t))
						{
							if (vftable_info_t.methods >= 1) {
								attach_action_to_popup(view, popup, funcVtable_ah_actionName);
							}
						}
					
						if (isAfxMsgMapEntryStruct(ea_address))
						{
							attach_action_to_popup(view, popup, funcMsg_ah_actionName);
						}
						return false;
					}
				}
				if (view != ctxInst->custViewer && view != ctxInst->codeViewer)
				{
					return false;
				}

				auto* place = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
					view,
					false, // mouse
					nullptr, // x
					nullptr // y
				));
				if (place == nullptr)
				{
					return false;
				}

				auto* token = place->token();
				if (token == nullptr)
				{
					return false;
				}

				func_t* tfnc = nullptr;
				if (token->kind == Token::Kind::ID_FNC
					&& (tfnc = ctxInst->getIdaFunction(token->value)))
				{
					attach_action_to_popup(
						view,
						popup,
						renameGlobalObj_ah_actionName
					);
					attach_action_to_popup(
						view,
						popup,
						openXrefs_ah_actionName
					);
					attach_action_to_popup(
						view,
						popup,
						openCalls_ah_actionName
					);

					if (ctxInst->fnc->fnc() == tfnc)
					{
						attach_action_to_popup(
							view,
							popup,
							changeFuncType_ah_actionName
						);
					}

					attach_action_to_popup(view, popup, "-");
				}
				else if (token->kind == Token::Kind::ID_GVAR)
				{
					attach_action_to_popup(
						view,
						popup,
						renameGlobalObj_ah_actionName
					);
					attach_action_to_popup(
						view,
						popup,
						openXrefs_ah_actionName
					);
					attach_action_to_popup(view, popup, "-");
				}

				attach_action_to_popup(
					view,
					popup,
					jump2asm_ah_actionName
				);
				attach_action_to_popup(
					view,
					popup,
					copy2asm_ah_actionName
				);
				attach_action_to_popup(
					view,
					popup,
					funcComment_ah_actionName
				);

				break;
			}


			/*
			case ui_get_lines_rendering_info:
			{
				auto* demoSyncGroup = get_synced_group(custViewer);
				if (demoSyncGroup == nullptr)
				{
					return false;
				}

				auto* demoPlace = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
						custViewer,
						false, // mouse
						nullptr, // x
						nullptr // y
				));
				if (demoPlace == nullptr)
				{
					return false;
				}
				auto eas = demoPlace->fnc()->yx_2_eas(demoPlace->yx());

				lines_rendering_output_t* out = va_arg(va, lines_rendering_output_t*);
				TWidget* view = va_arg(va, TWidget*);
				lines_rendering_input_t* info = va_arg(va, lines_rendering_input_t*);

				if (view == nullptr || info->sync_group != demoSyncGroup)
				{
					return false;
				}

				for (auto& sl : info->sections_lines)
				for (auto& l : sl)
				{
					if (eas.count(l->at->toea()))
					{
						out->entries.push_back(new line_rendering_output_entry_t(
							l,
							LROEF_FULL_LINE,
							0xff000000 + 0x90ee90
						));
					}
				}

				break;
			}
			*/

			// TWidget is being closed.
			case ui_widget_invisible:
			{
				TWidget* view = va_arg(va, TWidget*);
				if (view != ctxInst->custViewer && view != ctxInst->codeViewer)
				{
					return false;
				}

				//unhook_event_listener(HT_UI, this);
				ctxInst->custViewer = nullptr;
				ctxInst->codeViewer = nullptr;
				break;
			}
			}

			return false;
		}

		//
		//==============================================================================
		// cv handlers
		//==============================================================================
		//

		/**
		 * Called whenever the user moves the cursor around (mouse, keyboard).
		 * Fine-tune 'loc->place()' according to the X position.
		 *
		 * Without this, retdec_place_t's X position would not change when cursor moves
		 * around.
		 * Changing the position triggers some actions. E.g. retdec_place_t::print().
		 *
		 * custom_viewer_adjust_place_t
		 */
		ida_module_data void idaapi cv_adjust_place(TWidget* v, lochist_entry_t* loc, void* ud)
		{
			auto* plc = static_cast<retdec_place_t*>(loc->place());
			auto* fnc = plc->fnc();

			retdec_place_t nplc(
				fnc,
				fnc->adjust_yx(YX(
					plc->y(),
					loc->renderer_info().pos.cx
				)));

			if (plc->compare(&nplc) != 0) // not equal
			{
				loc->set_place(nplc);
			}
		}

		ida_module_data bool idaapi cv_double(TWidget* cv, int shift, void* ud)
		{
			RetDec* plg = static_cast<RetDec*>(ud);
			auto* place = dynamic_cast<retdec_place_t*>(get_custom_viewer_place(
				cv,
				false, // mouse
				nullptr, // x
				nullptr // y
			));
			if (place == nullptr)
			{
				return false;
			}

			auto* token = place->token();
			if (token == nullptr || token->kind != Token::Kind::ID_FNC)
			{
				return false;
			}
			auto fncName = token->value;

			auto* fnc = plg->getIdaFunction(fncName);
			if (fnc == nullptr)
			{
				INFO_MSG("function \"" << fncName << "\" not found in IDA functions\n");
				return false;
			}

			jumpto(fnc->start_ea, -1, UIJMP_ACTIVATE);

			return true;
		}

		/**
		 * custom_viewer_location_changed_t
		 */
		ida_module_data void idaapi cv_location_changed(
			TWidget* v,
			const lochist_entry_t* was,
			const lochist_entry_t* now,
			const locchange_md_t& md,
			void* ud)
		{
			RetDec* ctx = static_cast<RetDec*>(ud);

			auto* oldp = dynamic_cast<const retdec_place_t*>(was->place());
			auto* newp = dynamic_cast<const retdec_place_t*>(now->place());
			if (oldp->compare(newp) == 0) // equal
			{
				return;
			}

			if (oldp->fnc() != newp->fnc())
			{
				retdec_place_t min(newp->fnc(), newp->fnc()->min_yx());
				retdec_place_t max(newp->fnc(), newp->fnc()->max_yx());
				set_custom_viewer_range(ctx->custViewer, &min, &max);
				ctx->fnc = newp->fnc();
			}
		}

		/**
		 * custom_viewer_get_place_xcoord_t
		 */
		ida_module_data int idaapi cv_get_place_xcoord(
			TWidget* v,
			const place_t* pline,
			const place_t* pitem,
			void* ud)
		{
			auto* mpline = static_cast<const retdec_place_t*>(pline);
			auto* mpitem = static_cast<const retdec_place_t*>(pitem);

			if (mpline->y() != mpitem->y())
			{
				return -1; // not included
			}
			// i.e. mpline->y() == mpitem->y()
			else if (mpitem->x() == 0)
			{
				return -2; // points to entire line
			}
			else
			{
				return mpitem->x(); // included at coordinate
			}
		}
	}
}
