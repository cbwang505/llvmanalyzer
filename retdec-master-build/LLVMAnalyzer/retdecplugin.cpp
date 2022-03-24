#include "stdafx.h"
#include "retdecida.h"
#include "ui.h"
#include "FunctionAnalysis.h"
#include "FunctionContainer.h"
#include "LLVMAnalyzer.h"
hexdsp_t *hexdsp;
static const std::string pluginID = "Niii";
static const std::string pluginProducer = "Niii Software";
static const std::string pluginCopyright = "Copyright 2022 " + pluginProducer;
static const std::string pluginURL = "https://github.com/cbwang505";
static const std::string pluginHotkey = "Ctrl-Shift+U";
/*static const std::string pluginBuildDate =std::move( retdec_utils_getCurrentDate());*/



plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION,
	PLUGIN_PROC,                    // plugin flags
	retdec::plugin::init,                            // initialize fnc
	retdec::plugin::term,                         // terminate fnc
	retdec::plugin::run_plugin,                         // invoke fnc
	pluginCopyright.data(), // long plugin comment
	pluginURL.data(),       // multiline plugin help
	retdec::plugin::pluginName.data(),      // the preferred plugin short name
	pluginHotkey.data()     // the preferred plugin hotkey

};
namespace retdec {
	namespace plugin {
		
			

			static volatile bool Pligin_Initialized = false;

			static const char* fullDecompilation_ah_actionName = "retdec:ActionFullDecompilation";
			static const char* fullDecompilation_ah_actionLabel = "Auto Analyze Program";
			static const char* fullDecompilation_ah_actionHotkey = "Ctrl+Shift+D";
			static const char* fullDecompilation_ah_menupath = "File/Produce file/Full decompilation program";

			static const char* jump2asm_ah_actionName = "retdec:ActionJump2Asm";
			static const char* jump2asm_ah_actionLabel = "Jump to assembly";
			static const char* jump2asm_ah_actionHotkey = "A";

			static const char* copy2asm_ah_actionName = "retdec:ActionCopy2Asm";
			static const char* copy2asm_ah_actionLabel = "Copy to assembly";
			static const char* copy2asm_ah_actionHotkey = "";

			static const char* funcComment_ah_actionName = "retdec:ActionFunctionComment";
			static const char* funcComment_ah_actionLabel = "Edit func comment";
			static const char* funcComment_ah_actionHotkey = ";";

			static const char* funcSelect_ah_actionName = "retdec:ActionFunctionSelect";
			static const char* funcSelect_ah_actionLabel = "Add function to Analyze";
			static const char* funcSelect_ah_actionHotkey = "Ctrl+Shift+A";

		    static const char* funcVtable_ah_actionName = "retdec:ActionVtableSelect";
			static const char* funcVtable_ah_actionLabel = "Add virtual table to Analyze";
			static const char* funcVtable_ah_actionHotkey = "Ctrl+Shift+T";

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


			static const char* functionanalysisSelect_ah_actionName = "retdec:Show Selected Function";
			static const char* functionanalysisSelect_ah_actionLabel = "Selected function to decompilation";
			static const char* functionanalysisSelect_ah_actionHotkey = "Ctrl+Shift+Q";
			static const char* functionanalysisSelect_ah_menupath = "Edit/Functions/Analyze Selected Function";


			static const char* functionanalysis_ah_actionName = "retdec:Analyze Function";
			static const char* functionanalysis_ah_actionLabel = "Analyze function to decompilation";
			static const char* functionanalysis_ah_actionHotkey = "Ctrl+Shift+W";
			static const char* functionanalysis_ah_menupath = "Edit/Functions/Create Analyze Function";



			static const char* clearSegment_ah_actionName = "retdec:Clear Segment";
			static const char* clearSegment_ah_actionLabel = "Clear segment with zeroed bytes...";
			static const char* clearSegment_ah_actionHotkey = "Alt+C";
			static const char* clearSegment_ah_menupath = "Edit/Segments/Clear Segment";

			ida_module_data std::string retdec_utils_getCurrentDate();
			ida_dll_data std::shared_ptr<RetDec>  RetDec_Global_Instance();
			ida_dll_data void  set_hexrays_plugin(hexdsp_t *hexdsp_from);
			ida_dll_data int idaapi my_hexrays_cb_t(void *ud, hexrays_event_t event, va_list va);
			ida_dll_data bool register_global_ui_callback();
			ida_dll_data bool un_register_global_ui_callback();
			ida_dll_data void registerPluginPlace(const plugin_t& PLUGIN);



			int idaapi init(void)
			{

				std::shared_ptr<RetDec> ctxInst = RetDec_Global_Instance();
				return  PLUGIN_OK;
			}
			void idaapi term()
			{
				std::shared_ptr<RetDec> ctxInst = RetDec_Global_Instance();
				ctxInst->Global_Term_Plugin();
				INFO_MSG(pluginName << " version " << pluginVersion << " unloaded OK" << std::endl)
			}

			bool register_global_llvm_seg()
			{
				const char* llvmseg = "_llvm";
				ea_t maxsegend = 0;
				segment_t* seg = get_segm_by_name(llvmseg);
				if (seg)
				{

					vtbl_seg* vtbldata = (vtbl_seg*)malloc(LLVMDataTableRoutineReserveSize);
					ea_t* funcdata = (ea_t*)malloc(LLVMDataTableItemReserveSize);
					memset(vtbldata, 0, LLVMDataTableRoutineReserveSize);
					memset(funcdata, 0, LLVMDataTableItemReserveSize);
					get_bytes(vtbldata, LLVMDataTableRoutineReserveSize, seg->start_ea);

					ea_t vtblcount = vtbldata->vtbl_count;
					int i = 0;
					vtbl_seg_item* vtblstart = &(vtbldata->vtbl_item[0]);
					for (; i < vtblcount; i++)
					{
						ea_t vtbllookup = vtblstart->ea_vtbl;
						if (vtbllookup == 0)
						{
							break;
						}else if(vtblstart->func_vtbl)
						{
							get_bytes(funcdata, LLVMDataTableItemReserveSize, vtblstart->func_vtbl);
							for (ea_t* funcstart = funcdata; *funcstart != 0; funcstart++)
							{
								addvtbl2fns(vtbllookup, *funcstart);
							}
						}
						vtblstart++;
					}						
					free(funcdata);
					free(vtbldata);
					return true;
				}
				else
				{
					for (int i = 0; i < get_segm_qty(); i++)
					{
						segment_t* segtmp = getnseg(i);

						if (segtmp->end_ea > maxsegend)
						{
							maxsegend = segtmp->end_ea;
						}

					}
					maxsegend = ALIGN(maxsegend, LLVMDataTableReserveSize);
					ea_t maxsegendend = maxsegend + LLVMDataTableReserveSize + LLVMFunctionTableReserveSize;
					return add_segm(0, maxsegend, maxsegendend, llvmseg, "CODE");
				}
			}

			bool idaapi run_plugin(size_t arg)
			{
				if (!hexdsp)
				{
					bool retcheck = init_hexrays_plugin();
					if (!retcheck)
					{
						ERROR_MSG("Failed to init hexrays plugin" << std::endl)
						return  false;
					}
					assert(hexdsp);
					if(!register_global_llvm_seg())
					{
						ERROR_MSG("Failed to init llvm seg" << std::endl)
					}
					set_hexrays_plugin(hexdsp);
				}
				std::shared_ptr<RetDec> ctxInst = RetDec_Global_Instance();
				return 	ctxInst->Global_Run_Plugin(arg);
			}


			
			void delfuncplugin(uint64_t addr)
			{
				std::shared_ptr<RetDec> ctxInst = RetDec_Global_Instance();
				for (auto& func : ctxInst->decfuncs)
				{					
					uint64_t addrfunc = func.getStart();
					if(addr==addrfunc)
					{
						ctxInst->decfuncs.erase(func);
						break;
					}
				}
			}


			bool idaapi RetDec::Global_Run_Plugin(size_t arg)
			{

				static fullDecompilation_ah_t fullDecompilation_ah(*this);
				static const action_desc_t fullDecompilation_ah_desc = ACTION_DESC_LITERAL(
					fullDecompilation_ah_actionName,
					fullDecompilation_ah_actionLabel,
					&fullDecompilation_ah,
					fullDecompilation_ah_actionHotkey,
					nullptr,
					63
				);

				static jump2asm_ah_t jump2asm_ah(*this);
				static const action_desc_t jump2asm_ah_desc = ACTION_DESC_LITERAL(
					jump2asm_ah_actionName,
					jump2asm_ah_actionLabel,
					&jump2asm_ah,

					jump2asm_ah_actionHotkey,
					"jump2asm_ah_t",
					53
				);

				static copy2asm_ah_t copy2asm_ah(*this);
				static const action_desc_t copy2asm_ah_desc = ACTION_DESC_LITERAL(
					copy2asm_ah_actionName,
					copy2asm_ah_actionLabel,
					&copy2asm_ah,

					copy2asm_ah_actionHotkey,
					nullptr,
					-1
				);

				static funcComment_ah_t funcComment_ah(*this);
				static const action_desc_t funcComment_ah_desc = ACTION_DESC_LITERAL(
					funcComment_ah_actionName,
					funcComment_ah_actionLabel,
					&funcComment_ah,
					funcComment_ah_actionHotkey,
					nullptr,
					-1
				);
				static funcSelect_ah_t  funcSelect_ah(*this);
				static const action_desc_t  funcSelect_ah_desc = ACTION_DESC_LITERAL(
					funcSelect_ah_actionName,
					funcSelect_ah_actionLabel,
					&funcSelect_ah,
					funcSelect_ah_actionHotkey,
					nullptr,
					37
				);
				static funcVtable_ah_t  funcVtable_ah(*this);
				static const action_desc_t  funcVtable_ah_desc = ACTION_DESC_LITERAL(
					funcVtable_ah_actionName,
					funcVtable_ah_actionLabel,
					&funcVtable_ah,
					funcVtable_ah_actionHotkey,
					nullptr,
					37
				);

				static 	renameGlobalObj_ah_t renameGlobalObj_ah(*this);
				static const action_desc_t renameGlobalObj_ah_desc = ACTION_DESC_LITERAL(
					renameGlobalObj_ah_actionName,
					renameGlobalObj_ah_actionLabel,
					&renameGlobalObj_ah,
					renameGlobalObj_ah_actionHotkey,
					nullptr,
					-1
				);

				static openXrefs_ah_t openXrefs_ah(*this);
				static const action_desc_t openXrefs_ah_desc = ACTION_DESC_LITERAL(
					openXrefs_ah_actionName,
					openXrefs_ah_actionLabel,
					&openXrefs_ah,
					openXrefs_ah_actionHotkey,
					nullptr,
					-1
				);

				static openCalls_ah_t openCalls_ah(*this);
				static const action_desc_t openCalls_ah_desc = ACTION_DESC_LITERAL(
					openCalls_ah_actionName,
					openCalls_ah_actionLabel,
					&openCalls_ah,
					openCalls_ah_actionHotkey,
					nullptr,
					-1
				);

				static	changeFuncType_ah_t changeFuncType_ah(*this);
				static	const action_desc_t changeFuncType_ah_desc = ACTION_DESC_LITERAL(
					changeFuncType_ah_actionName,
					changeFuncType_ah_actionLabel,
					&changeFuncType_ah,
					changeFuncType_ah_actionHotkey,
					nullptr,
					-1
				);


				static functionanalysis_ah_t functionanalysis_ah(*this);
				static const action_desc_t functionanalysis_ah_desc = ACTION_DESC_LITERAL(
					functionanalysis_ah_actionName,
					functionanalysis_ah_actionLabel,
					&functionanalysis_ah,
					functionanalysis_ah_actionHotkey,
					nullptr,
					41
				);
				static functionanalysisSelect_ah_t functionanalysisSelect_ah(*this);
				static const action_desc_t functionanalysisSelect_ah_desc = ACTION_DESC_LITERAL(
					functionanalysisSelect_ah_actionName,
					functionanalysisSelect_ah_actionLabel,
					&functionanalysisSelect_ah,
					functionanalysisSelect_ah_actionHotkey,
					nullptr,
					42
				);

				static LLVMAnalyzer clearSegment_ah(*this);
				static const action_desc_t clearSegment_ah_desc = ACTION_DESC_LITERAL(
					clearSegment_ah_actionName,
					clearSegment_ah_actionLabel,
					&clearSegment_ah,
					clearSegment_ah_actionHotkey,
					nullptr,
					32
				);



				if (!register_action(fullDecompilation_ah_desc)
					|| !attach_action_to_menu(
						fullDecompilation_ah_menupath,
						fullDecompilation_ah_actionName,
						SETMENU_APP))
				{
					ERROR_MSG("Failed to register: " << fullDecompilation_ah_actionName);
				}

				if (!register_action(clearSegment_ah_desc)
					|| !attach_action_to_menu(
						clearSegment_ah_menupath,
						clearSegment_ah_actionName,
						SETMENU_APP))
				{
					ERROR_MSG("Failed to register: " << clearSegment_ah_actionName);
				}


				if (!register_action(functionanalysis_ah_desc)
					|| !attach_action_to_menu(
						functionanalysis_ah_menupath,
						functionanalysis_ah_actionName,
						SETMENU_APP))
				{
					ERROR_MSG("Failed to register: " << functionanalysis_ah_actionName);
				}

				if (!register_action(functionanalysisSelect_ah_desc)
					|| !attach_action_to_menu(
						functionanalysisSelect_ah_menupath,
						functionanalysisSelect_ah_actionName,
						SETMENU_APP))
				{
					ERROR_MSG("Failed to register: " << functionanalysisSelect_ah_actionName);
				}

				register_action(jump2asm_ah_desc);
				register_action(copy2asm_ah_desc);
				register_action(funcComment_ah_desc);
				register_action(funcSelect_ah_desc);
				register_action(funcVtable_ah_desc);
				register_action(renameGlobalObj_ah_desc);
				register_action(openCalls_ah_desc);
				register_action(openXrefs_ah_desc);
				register_action(changeFuncType_ah_desc);

				registerPluginPlace(PLUGIN);

				//hook_event_listener(HT_UI, this);
				if (!register_global_ui_callback())
				{
					ERROR_MSG("Failed to regist global_ui_callback")
				}

				install_hexrays_callback(my_hexrays_cb_t, nullptr);
				INFO_MSG(pluginName << " Plugin (Version " << pluginVersion << ") Successfully Loaded , Power By GuoliSec Team <guolisec@guolisec.com>\n")
				Pligin_Initialized = true;
				return true;
			}

			bool RetDec::Global_Term_Plugin()
			{
				if (Pligin_Initialized)
				{
					Pligin_Initialized = false;
					saveIdaDatabase(false, "");
					detach_action_from_menu(clearSegment_ah_menupath, clearSegment_ah_actionName);
					unregister_action(clearSegment_ah_actionName);

					detach_action_from_menu(fullDecompilation_ah_menupath, fullDecompilation_ah_actionName);
					unregister_action(fullDecompilation_ah_actionName);

					detach_action_from_menu(functionanalysis_ah_menupath, functionanalysis_ah_actionName);
					unregister_action(functionanalysis_ah_actionName);

					detach_action_from_menu(functionanalysisSelect_ah_menupath, functionanalysisSelect_ah_actionName);
					unregister_action(functionanalysisSelect_ah_actionName);

					unregister_action(jump2asm_ah_actionName);
					unregister_action(copy2asm_ah_actionName);
					unregister_action(funcComment_ah_actionName);
					unregister_action(funcSelect_ah_actionName);
					unregister_action(funcVtable_ah_actionName);
					unregister_action(renameGlobalObj_ah_actionName);
					unregister_action(openCalls_ah_actionName);
					unregister_action(openXrefs_ah_actionName);
					unregister_action(changeFuncType_ah_actionName);
					un_register_global_ui_callback();
					//remove_hexrays_callback(my_hexrays_cb_t, nullptr);
					term_hexrays_plugin();
				}
				return true;
			}
			
			functionanalysisSelect_ah_t::functionanalysisSelect_ah_t(RetDec& p) : plg(p)
			{

			}
			int functionanalysisSelect_ah_t::activate(action_activation_ctx_t*)
			{
				std::unique_ptr<FunctionContainer> function_Select(new FunctionContainer(nullptr));
				function_Select->SetupPlugin(plg, delfuncplugin);
				if (function_Select->exec() == QDialog::Accepted)
				{
					function_Select->FinalizeSelectedFunctions();
					size_t funcsount = function_Select->selfuncs.size();
					if (funcsount)
					{
						qstring finmsg;
						finmsg.sprnt("Selected Function Count :=> %d To Analyze,Selecting Output File ...\n", funcsount);
						INFO_MSG(finmsg.c_str())
						plg.manyFunctionDecompilation(function_Select->selfuncs);
					}
				}
				return false;
			}

			action_state_t functionanalysisSelect_ah_t::update(action_update_ctx_t*)
			{

				return AST_ENABLE_ALWAYS;
			}

			functionanalysis_ah_t::functionanalysis_ah_t(RetDec& p) : plg(p)
			{

			}

			int functionanalysis_ah_t::activate(action_activation_ctx_t*)
			{
				char tmp[_MAX_PATH];
				ea_t hexea = get_screen_ea();
				qstring buff;
				ea_t start = 0;
				ea_t end = 0;
				QMap<int, QString> seg_map;
				for (int i = 0; i < get_segm_qty(); i++)
				{
					segment_t* seg = getnseg(i);
					if (get_visible_segm_name(&buff, seg) <= 0)
					{
						continue;
					}
					seg_map.insert(i, QString(buff.c_str()));

				}
				std::unique_ptr<FunctionAnalysis> function_analysis(new FunctionAnalysis(nullptr));
				function_analysis->set_function_start_addr(QString::number(hexea, 16));
				function_analysis->init_seg_range(seg_map);
				if (function_analysis->exec() == QDialog::Accepted)
				{

					seg_map.clear();
					int i = function_analysis->get_seg();
					segment_t* seg = getnseg(i);
					if (get_visible_segm_name(&buff, seg) > 0)
					{
						bool has_range = function_analysis->get_range(&start, &end);
						if (has_range)
						{
							::qsnprintf(tmp, _MAX_PATH, "Start to Analyze Function At:=> %x To Segment:=> %s From Range:=> %x - %x\n", hexea, buff.c_str(), start, end);

						}
						else
						{
							::qsnprintf(tmp, _MAX_PATH, "Start to Analyze Function At:=> %x To Segment:=> %s \n", hexea, buff.c_str());
						}
						INFO_MSG(tmp);
						plg.selectiveDecompilationToSegment(hexea, i, start, end, false, false);
						hide_wait_box();
						return false;
					}
					INFO_MSG("Error Analyze Function\n")
				}
				else
				{
					INFO_MSG("Canceled Analyze Function\n")
				}
				return false;
			}

			action_state_t functionanalysis_ah_t::update(action_update_ctx_t*)
			{

				return AST_ENABLE_ALWAYS;
			}
		}
	}
