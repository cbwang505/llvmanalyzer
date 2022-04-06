#include <memory>
#include <retdec/retdec/retdec.h>
#include <retdec/utils/binary_path.h>
#include "utils.h"
#include <retdec/config/config.h>
#include <retdec/utils/filesystem.h>
#include <retdec/utils/time.h>

#include "config.h"
#include "place.h"
#include "retdec.h"
#include "function.h"
#include "LLVMIDALogger.h"
#include "PlainCodeView.h"
#include "stdafx.h"
hexdsp_t *hexdsp;
#define Function_Global_Decompilation 0
namespace retdec {
	namespace plugin {

		static std::shared_ptr<RetDec> InstanceObject = nullptr;
		static std::map<ea_t, std::vector<ea_t>> vtbl2fns;
		static std::map<ea_t, std::vector<ea_t>> vtbl2fnsAnalyze;
		ida_dll_data  const std::string pluginName = "LLVMAnalyzer";
		ida_dll_data  const std::string pluginVersion = "1.0";
		RetDec::RetDec()
		{

		}

		ida_dll_data void  set_hexrays_plugin(hexdsp_t *hexdsp_from)
		{
			hexdsp = hexdsp_from;
		}

		ida_dll_data void addvtbl2fns(ea_t vtbl, ea_t fn)
		{
			auto kv = vtbl2fns.find(vtbl);
			if (kv == vtbl2fns.end())
			{
				std::vector<ea_t> fns = { fn };
				vtbl2fns.emplace(vtbl, fns);
			}
			else
			{
				for (auto addr : kv->second)
				{
					if (addr == fn)
					{
						return;
					}
				}
				kv->second.push_back(fn);
			}
		}
		ida_dll_data void addvtbl2fnsanalyze(ea_t vtbl, ea_t fn)
		{
			auto kv = vtbl2fnsAnalyze.find(vtbl);
			if (kv == vtbl2fnsAnalyze.end())
			{
				std::vector<ea_t> fns = { fn };
				vtbl2fnsAnalyze.emplace(vtbl, fns);
			}
			else
			{
				for (auto addr : kv->second)
				{
					if (addr == fn)
					{
						return;
					}
				}
				kv->second.push_back(fn);
			}
		}
		ida_dll_data void addvtbl2fns2seg(ea_t vtbl)
		{

			auto kv = vtbl2fns.find(vtbl);
			if (kv != vtbl2fns.end())
			{
				if (kv->second.size() == 0)
				{
					return;
				}
			}
			else
			{
				return;
			}
			const char* llvmseg = "_llvm";
			segment_t* seg = get_segm_by_name(llvmseg);
			assert(seg);
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
				for (; i <= vtblcount; i++)
				{
					ea_t vtbllookup = vtblstart->ea_vtbl;
					if (vtbllookup == 0)
					{
						vtblstart->ea_vtbl = vtbl;
						ea_t funcstart = seg->start_ea + LLVMDataTableRoutineReserveSize + (vtbldata->vtbl_count*LLVMDataTableItemReserveSize);
						vtblstart->func_vtbl = funcstart;

						if (kv != vtbl2fns.end())
						{
							for (int j = 0; j < kv->second.size(); j++)
							{
								funcdata[j] = kv->second[j];
							}
						}
						put_bytes(funcstart, funcdata, LLVMDataTableItemReserveSize);
						vtbldata->vtbl_count++;
						break;
					}
					if (vtbl == vtbllookup)
					{
						ea_t funcstart = vtblstart->func_vtbl;
						if (kv != vtbl2fns.end())
						{
							for (int j = 0; j < kv->second.size(); j++)
							{
								funcdata[j] = kv->second[j];
							}
						}
						put_bytes(funcstart, funcdata, LLVMDataTableItemReserveSize);
						break;
					}

					vtblstart++;
				}
				put_bytes(seg->start_ea, vtbldata, LLVMDataTableRoutineReserveSize);
				free(funcdata);
				free(vtbldata);
			}
		}
		void idaapi  reconstruct_type_cb(vdui_t* ud)
		{
			if (!ud)
			{
				return;
			}
			vdui_t &vu = *ud;
			cfuncptr_t cfunc = vu.cfunc;
			for (auto& kv : vtbl2fns)
			{
				ea_t vtaddrreal = kv.first;
				for (ea_t fnit : kv.second)
				{
					if (fnit == cfunc->entry_ea)
					{
						qstring rawname;

#ifdef __EA64__
						rawname.sprnt("Class_vtable_%llx_type", vtaddrreal);
#else // __EA64__
						rawname.sprnt("Class_vtable_%x_type", vtaddrreal);
#endif // __EA64__

						tid_t strucval = get_struc_id(rawname.c_str());
						if (strucval)
						{
							tinfo_t new_type = create_typedef(rawname.c_str());
							tinfo_t new_type_ptr = make_pointer(new_type);
							for (lvar_t& vr : *cfunc->get_lvars())
							{
								qstring nm = vr.name;
								vr.set_lvar_type(new_type_ptr);
								vu.refresh_view(false);
								return;
							}
						}
						break;
					}

				}

			}
			return;
		}
		static const char* funcSelect_ah_actionName = "retdec:ActionFunctionSelect";
		ida_dll_data  int idaapi my_hexrays_cb_t(void *ud, hexrays_event_t event, va_list va)
		{
			switch (event)
			{
			case hxe_open_pseudocode:
			{
				vdui_t* vu = va_arg(va, vdui_t *);
				reconstruct_type_cb(vu);
				break;
			}
			case hxe_populating_popup:
			{
				auto widget = va_arg(va, TWidget *);
				const auto popup = va_arg(va, TPopupMenu *);
				auto &vu = *va_arg(va, vdui_t *);
				// add new command to the popup menu
				attach_action_to_popup(vu.ct, popup, funcSelect_ah_actionName);
			}
			default:
			{
				break;
			}
			}
			return false;
		}

		bool runDecompilation(
			retdec::config::Config& configout, uint64_t segaddr = 0,
			std::string* output = nullptr, std::vector<std::uint8_t>* outBuff = nullptr)
		{
			try
			{
				;
				//std::ostringstream oss;
				std::ofstream oss;
				auto idaPath = retdec::utils::getThisBinaryDirectoryPath();
				auto configlogPath = idaPath;
				configlogPath.append("plugins");
				configlogPath.append("retdec");
				configlogPath.append("log");
				if (!fs::is_directory(configlogPath))
				{
					fs::create_directories(configlogPath);
				}
				std::string logfilename = retdec::utils::getCurrentDate() + "-" + retdec::utils::getCurrentTime2() + ".log";
				configlogPath.append(logfilename);
				oss.open(configlogPath, std::ofstream::out);
				retdec::utils::io::LLVMIDAOutStream idastm(oss);
				retdec::utils::io::Logger::Ptr loggerInfo(new retdec::utils::io::LLVMIDALogger(idastm, true));
				retdec::utils::io::Logger::Ptr loggerDebug(new retdec::utils::io::LLVMIDALogger(idastm, true));
				retdec::utils::io::Logger::Ptr loggerError(new retdec::utils::io::LLVMIDALogger(idastm, true));

				auto rc = retdec::decompile_ida(configout, std::tie(loggerInfo, loggerDebug, loggerError), segaddr, output, outBuff);
				if (rc != 0)
				{
					throw std::runtime_error(
						"decompilation error code = " + std::to_string(rc)
					);
				}
				std::ostringstream finmsg;
				if (rc == 0)
				{
					finmsg << "Successfully Finished Analyze Function With Return Code : = > ";
				}
				else
				{
					finmsg << "Analyze Function Failed With Return Code : = > ";;
				}
				finmsg << std::hex << rc << std::endl;
				INFO_MSG(finmsg.str());
				oss.flush();
				oss.close();
				//INFO_MSG(oss.str());
			}
			catch (const std::runtime_error& e)
			{
				WARNING_GUI("Decompilation exception: " << e.what() << std::endl);
				return true;
			}
			catch (...)
			{
				WARNING_GUI("Decompilation exception: unknown" << std::endl);
				return true;
			}

			return false;
		}

		static bool idaapi moveBufferToSegment(LLVMFunctionTable* tbl, std::vector<std::uint8_t>& outBuff, int i)
		{
			char tmp[_MAX_PATH];
			segment_t* seg = getnseg(i);
			size_t len = outBuff.size();
			if (len)
			{
				if (tbl->num_func == 0)
				{
					tbl->raw_fuc[tbl->num_func].start_fun = seg->start_ea + LLVMFunctionTableReserveSize + LLVMDataTableReserveSize;
				}
				else
				{
					tbl->raw_fuc[tbl->num_func].start_fun = tbl->raw_fuc[tbl->num_func - 1].end_fun;
				}
				tbl->raw_fuc[tbl->num_func].len_func = len;
				tbl->raw_fuc[tbl->num_func].end_fun = ALIGN((tbl->raw_fuc[tbl->num_func].start_fun + len), LLVMFunctionItemAlignSize);
				put_bytes(tbl->raw_fuc[tbl->num_func].start_fun, outBuff.data(), len);
				qsnprintf(tmp, _MAX_PATH, "The corresponding function from %x is assign to range :=> %x to %x,use 'Create Function' shortcut to analyze\n", tbl->raw_fuc[tbl->num_func].org_fun, tbl->raw_fuc[tbl->num_func].start_fun, tbl->raw_fuc[tbl->num_func].end_fun);
				tbl->num_func++;
				put_bytes(seg->start_ea + LLVMDataTableReserveSize, tbl, LLVMFunctionTableReserveSize);
				INFO_MSG(tmp)
					return true;
			}
			else
			{
				return  false;
			}
		}

		static int idaapi threadFuncDecompilation(void* ud)
		{
			RetDec* that = static_cast<RetDec*>(ud);
			std::string output;
			std::string* out = &output;
			std::shared_ptr<retdec::config::Config> config_ptr = that->config;
			bool ret = true;
			if (config_ptr)
			{
				if (that->decompileSegment)
				{
					std::vector<std::uint8_t> outBuff;
					segment_t* seg = getnseg(that->decompileSegment);

					LLVMFunctionTable* tbl(reinterpret_cast<LLVMFunctionTable*>(malloc(LLVMFunctionTableReserveSize)));
					memset(tbl, 0, LLVMFunctionTableReserveSize);
					get_bytes(tbl, LLVMFunctionTableReserveSize, seg->start_ea + LLVMDataTableReserveSize);
					//int num_func=get_dword(seg->start_ea);
					if (tbl->num_func == 0)
					{
						tbl->raw_fuc[tbl->num_func].start_fun = seg->start_ea + LLVMFunctionTableReserveSize;
					}
					else
					{
						tbl->raw_fuc[tbl->num_func].start_fun = tbl->raw_fuc[tbl->num_func - 1].end_fun;
					}
					tbl->raw_fuc[tbl->num_func].org_fun = that->decompiledFunction->start_ea;
					ret = runDecompilation(*config_ptr, tbl->raw_fuc[tbl->num_func].start_fun, out, &outBuff);
					if (!ret)
					{
						if (moveBufferToSegment(tbl, outBuff, that->decompileSegment))
						{
							outBuff.clear();
						}
						free(tbl);
						that->fnc2code[that->decompiledFunction].code = output;
						PlainCodeView* code_view = new PlainCodeView(*that);
						execute_sync(*code_view, MFF_FAST);
					}
					else
					{
						free(tbl);
					}

				}
				else
				{
					ret = runDecompilation(*config_ptr, 0, out);
					if (!ret)
					{
						that->fnc2code[that->decompiledFunction].code = output;
						std::ofstream oss;
						std::string outcfile = config_ptr->parameters.getOutputFile();
						oss.open(outcfile, std::ofstream::out);
						oss << output;
						oss.flush();
						oss.close();
						PlainCodeView* code_view = new PlainCodeView(*that);
						execute_sync(*code_view, MFF_FAST);
					}
				}

				return ret;
			}
			else
			{
				return true;
			}
		}


		ida_dll_data std::string retdec_utils_getCurrentDate()
		{
			return retdec::utils::getCurrentDate();
		}

		ssize_t idaapi ui_callback(void *user_data, int code, va_list va);

		ida_dll_data bool register_global_ui_callback()
		{
			return hook_to_notification_point(HT_UI, ui_callback);
		}


		ida_dll_data bool un_register_global_ui_callback()
		{
			return unhook_from_notification_point(HT_UI, ui_callback);
		}


		ida_dll_data Function* RetDec::selectiveDecompilationToSegment(ea_t ea, int seg, ea_t start, ea_t end, bool redecompile, bool regressionTests)
		{
			if (isRelocatable() && inf_get_min_ea() != 0)
			{
				WARNING_GUI("RetDec plugin can selectively decompile only "
					"relocatable objects loaded at 0x0.\n"
					"Rebase the program to 0x0 or use full decompilation."
				);
				return nullptr;
			}
			show_wait_box("Decompiling...");
			func_t* f = get_func(ea);
			if (f == nullptr)
			{
				WARNING_MSG("Function is not exists in ida ,try to create function.\n");
				f = new func_t(ea, BADADDR);
				add_func_ex(f);
			}

			if (!redecompile)
			{
				auto it = fnc2fnc.find(f);
				if (it != fnc2fnc.end())
				{
					return &it->second;
				}
			}
			if (!config)
			{
				config = std::make_shared<retdec::config::Config>();
			}

			auto idaPath = retdec::utils::getThisBinaryDirectoryPath();
			auto configPath = idaPath;
			configPath.append("plugins");
			configPath.append("retdec");
			configPath.append("decompiler-config.json");
			if (fs::exists(configPath))
			{
				//retdec::config::Config cfg = retdec::config::Config::fromFile(configPath.string());
				config->readJsonFile(configPath.string());
				config->parameters.fixRelativePaths(idaPath.string());
			}


			if (fillConfig(*config))
			{
				return nullptr;
			}

			std::set<ea_t> selectedFncs;
			/*std::string output;
			std::string* out = &output;
			config->parameters.setOutputFormat("json");
			*/
			//
			config->parameters.setOutputFormat("c");
			if (start&&end)
			{
				retdec::common::AddressRange r(start, end);
				config->parameters.selectedRanges.insert(r);
				config->parameters.setIsSelectedDecodeOnly(true);
			}

			selectedFncs.insert(f->start_ea);
			//std::string func = "sub_40F5A0";
			qstring func;
			size_t func_len = get_func_name(&func, ea);
			if (func_len)
			{
				config->parameters.selectedFunctions.insert(func.c_str());
			}


			decompiledFunction = f;
			decompileSegment = seg;

			config->parameters.setIsVerboseOutput(true);
			config->parameters.setIsKeepAllFunctions(true);
			if (regressionTests)
			{

				config->parameters.setOutputFormat("plain");
				config->parameters.setOutputFile(config->parameters.getInputFile() + ".c");

			}

			hide_wait_box();
			qthread_create(threadFuncDecompilation, static_cast<void*>(this));

			/*if (runDecompilation(*config, out))
			{

				return nullptr;
			}*/


			std::vector<Token> ts;
			if (ts.empty())
			{
				return nullptr;
			}
			return &(fnc2fnc[f] = Function(f, ts));
		}

		ida_dll_data bool RetDec::manyFunctionDecompilation(retdec::common::FunctionContainer functionslist)
		{

			if (isRelocatable() && inf_get_min_ea() != 0)
			{
				WARNING_GUI("RetDec plugin can selectively decompile only "
					"relocatable objects loaded at 0x0.\n"
					"Rebase the program to 0x0 or use full decompilation."
				);
				return false;
			}
			std::string defaultOut = getInputPath() + ".c";

			char *tmp = ask_file(                // Returns: file name
				true,                        // bool for_saving
				defaultOut.data(),           // const char *default_answer
				"%s",                        // const char *format
				"Save decompiled file"
			);
			if (tmp == nullptr) // canceled
			{
				return false;
			}
			show_wait_box("Decompiling...");
			std::string out = tmp;

			if (!config)
			{
				config = std::make_shared<retdec::config::Config>();
			}

			auto idaPath = retdec::utils::getThisBinaryDirectoryPath();
			auto configPath = idaPath;
			configPath.append("plugins");
			configPath.append("retdec");
			configPath.append("decompiler-config.json");
			if (fs::exists(configPath))
			{
				//retdec::config::Config cfg = retdec::config::Config::fromFile(configPath.string());
				config->readJsonFile(configPath.string());
				config->parameters.fixRelativePaths(idaPath.string());
			}


			if (fillConfig(*config, out))
			{
				return false;
			}
			config->vtbl2func.clear();
			for (const common::Function& func : functionslist)
			{
				for (std::pair<ea_t, std::vector<ea_t>> kv : vtbl2fnsAnalyze)
				{
					for (ea_t addr : kv.second)
					{
						if (addr == func.getStart())
						{
							retdec::common::Address vtbl(kv.first);
							if (!config->vtbl2func.count(vtbl))
							{
								std::vector<retdec::common::Address> fns;
								for (ea_t fn : kv.second)
								{
									retdec::common::Address fnaddr(fn);
									fns.push_back(fnaddr);
								}
								config->vtbl2func.emplace(vtbl, fns);
							}
						}
					}
				}
			}
			std::set<ea_t> selectedFncs;

			//config->parameters.setOutputFormat("json");
			config->parameters.setOutputFormat("c");
			for (auto& func : functionslist)
			{

				retdec::common::AddressRange r(func.getStart(), func.getEnd());
				config->parameters.selectedRanges.insert(r);
				config->parameters.selectedFunctions.insert(func.getName());
			}


			config->parameters.setIsSelectedDecodeOnly(true);
			config->parameters.setIsVerboseOutput(true);
			config->parameters.setIsKeepAllFunctions(true);


			config->parameters.setOutputFormat("plain");
			//config->parameters.setOutputFile(config->parameters.getInputFile() + ".c");	
			decompileSegment = 0;
			decompiledFunction = new func_t(Function_Global_Decompilation);
			hide_wait_box();
			qthread_create(threadFuncDecompilation, static_cast<void*>(this));
			//runDecompilation(*config);	;

			return true;


		}
		Function* RetDec::selectiveDecompilation(
			ea_t ea,
			bool redecompile,
			bool regressionTests)
		{
			if (isRelocatable() && inf_get_min_ea() != 0)
			{
				WARNING_GUI("RetDec plugin can selectively decompile only "
					"relocatable objects loaded at 0x0.\n"
					"Rebase the program to 0x0 or use full decompilation."
				);
				return nullptr;
			}
			show_wait_box("Decompiling...");
			func_t* f = get_func(ea);
			if (f == nullptr)
			{
				WARNING_GUI("Function must be selected by the cursor.\n");
				return nullptr;
			}

			if (!redecompile)
			{
				auto it = fnc2fnc.find(f);
				if (it != fnc2fnc.end())
				{
					return &it->second;
				}
			}
			if (!config)
			{
				config = std::make_shared<retdec::config::Config>();
			}

			auto idaPath = retdec::utils::getThisBinaryDirectoryPath();
			auto configPath = idaPath;
			configPath.append("plugins");
			configPath.append("retdec");
			configPath.append("decompiler-config.json");
			if (fs::exists(configPath))
			{
				//retdec::config::Config cfg = retdec::config::Config::fromFile(configPath.string());
				config->readJsonFile(configPath.string());
				config->parameters.fixRelativePaths(idaPath.string());
			}


			if (fillConfig(*config))
			{
				return nullptr;
			}

			std::set<ea_t> selectedFncs;
			std::string output;
			std::string* out = &output;

			//config->parameters.setOutputFormat("json");
			config->parameters.setOutputFormat("c");
			retdec::common::AddressRange r(f->start_ea, f->end_ea);
			config->parameters.selectedRanges.insert(r);

			qstring func;
			size_t func_len = get_func_name(&func, ea);
			if (func_len)
			{
				config->parameters.selectedFunctions.insert(func.c_str());
			}
			selectedFncs.insert(f->start_ea);
			config->parameters.setIsSelectedDecodeOnly(true);
			config->parameters.setIsVerboseOutput(true);
			config->parameters.setIsKeepAllFunctions(true);
			if (regressionTests)
			{

				config->parameters.setOutputFormat("plain");
				config->parameters.setOutputFile(config->parameters.getInputFile() + ".c");
				out = nullptr;
			}

			hide_wait_box();
			qthread_create(threadFuncDecompilation, static_cast<void*>(this));

			/*if (runDecompilation(*config, out))
			{

				return nullptr;
			}*/


			if (out == nullptr)
			{
				return nullptr;
			}

			auto ts = parseTokens(output, f->start_ea);
			if (ts.empty())
			{
				return nullptr;
			}
			return &(fnc2fnc[f] = Function(f, ts));
		}

		ida_dll_data Function* RetDec::selectiveDecompilationAndDisplay(ea_t ea, bool redecompile)
		{
			auto* f = selectiveDecompilation(ea, redecompile);
			if (f)
			{
				displayFunction(f, ea);
			}
			return f;
		}

		void RetDec::displayFunction(Function* f, ea_t ea)
		{
			fnc = f;

			retdec_place_t min(fnc, fnc->min_yx());
			retdec_place_t max(fnc, fnc->max_yx());
			retdec_place_t cur(fnc, fnc->ea_2_yx(ea));

			TWidget* widget = find_widget(pluginName.c_str());
			if (widget != nullptr)
			{
				set_custom_viewer_range(custViewer, &min, &max);
				jumpto(custViewer, &cur, cur.x(), cur.y());
				bool take_focus = true;
				activate_widget(custViewer, take_focus);
				return;
			}

			// Without setting both x and y in render info, the current line gets
			// displayed as the first line in the viewer. Which is not nice because we
			// don't see the context before it. It is better if it is somewhere in the
			// middle of the viewer.
			renderer_info_t rinfo;
			rinfo.rtype = TCCRT_FLAT;
			rinfo.pos.cx = cur.x();
			rinfo.pos.cy = cur.y();

			custViewer = create_custom_viewer(
				pluginName.c_str(),        // title
				&min,         // minplace
				&max,         // maxplace
				&cur,         // curplace
				&rinfo,       // rinfo
				this,         // ud
				&ui_handlers, // handlers
				this,         // cvhandlers_ud
				nullptr       // parent widget
			);
			set_view_renderer_type(custViewer, TCCRT_FLAT);

			codeViewer = create_code_viewer(custViewer);
			set_code_viewer_is_source(codeViewer);
			display_widget(codeViewer, WOPN_TAB | WOPN_RESTORE | WOPN_NOT_CLOSED_BY_ESC);

			return;
		}

		std::shared_ptr<RetDec>  RetDec::GetInstance()
		{
			if (!InstanceObject)
			{

				InstanceObject = std::make_shared<RetDec>();


			}
			return InstanceObject;
		}

		ida_dll_data std::shared_ptr<RetDec>  RetDec_Global_Instance()
		{
			return RetDec::GetInstance();
		}


		ida_dll_data bool RetDec::fullDecompilation()
		{
			if (isRelocatable() && inf_get_min_ea() != 0)
			{
				WARNING_GUI("RetDec plugin can selectively decompile only "
					"relocatable objects loaded at 0x0.\n"
					"Rebase the program to 0x0 or use full decompilation."
				);
				return false;
			}
			std::string defaultOut = getInputPath() + ".c";

			char *tmp = ask_file(                // Returns: file name
				true,                        // bool for_saving
				defaultOut.data(),           // const char *default_answer
				"%s",                        // const char *format
				"Save decompiled file"
			);
			if (tmp == nullptr) // canceled
			{
				return false;
			}
			show_wait_box("Decompiling...");
			std::string out = tmp;

			INFO_MSG("Selected file: " << out << "\n");


			if (!config)
			{
				config = std::make_shared<retdec::config::Config>();
			}

			auto idaPath = retdec::utils::getThisBinaryDirectoryPath();
			auto configPath = idaPath;
			configPath.append("plugins");
			configPath.append("retdec");
			configPath.append("decompiler-config.json");
			if (fs::exists(configPath))
			{
				//retdec::config::Config cfg = retdec::config::Config::fromFile(configPath.string());
				config->readJsonFile(configPath.string());
				config->parameters.fixRelativePaths(idaPath.string());
			}

			if (fillConfig(*config, out))
			{
				return false;
			}
			config->parameters.setOutputFormat("c");

			decompileSegment = 0;
			//std::string func = "sub_40F5A0";
			//config->parameters.selectedFunctions.insert(func);
			config->parameters.setIsKeepAllFunctions(true);
			config->parameters.setIsVerboseOutput(true);
			//	config->parameters.setIsSelectedDecodeOnly(true);
			hide_wait_box();
			decompiledFunction = new func_t(Function_Global_Decompilation);
			qthread_create(threadFuncDecompilation, static_cast<void*>(this));
			//runDecompilation(*config);	;

			return true;
		}

		bool idaapi RetDec::run(size_t arg)
		{
			if (!auto_is_ok())
			{
				INFO_MSG("RetDec plugin cannot run because the initial autoanalysis"
					" has not been finished.\n");
				return false;
			}

			// ordinary selective decompilation
			//
			if (arg == 0)
			{
				auto* cv = get_current_viewer();
				bool redecompile = cv == custViewer || cv == codeViewer;
				return selectiveDecompilationAndDisplay(get_screen_ea(), redecompile);
			}
			// ordinary full decompilation
			//
			else if (arg == 1)
			{
				return fullDecompilation();
			}
			// regression tests selective decompilation
			// function to decompile is marked by "<retdec_select>" string in comment
			//
			else if (arg == 2)
			{
				for (unsigned i = 0; i < get_func_qty(); ++i)
				{
					qstring qCmt;
					func_t *fnc = getn_func(i);
					if (get_func_cmt(&qCmt, fnc, false) <= 0)
					{
						continue;
					}

					std::string cmt = qCmt.c_str();;
					if (cmt.find("<retdec_select>") != std::string::npos)
					{
						auto r = selectiveDecompilation(
							fnc->start_ea,
							false, // redecompile
							true); // regressionTests
						return r;
					}
				}
				return true;
			}
			// regression tests full decompilation
			//
			else if (arg == 3)
			{
				return fullDecompilation();
			}
			else
			{
				WARNING_GUI(pluginName << " version " << pluginVersion
					<< " cannot handle argument '" << arg << "'.\n"
				);
				return false;
			}

			return true;
		}

		RetDec::~RetDec()
		{
			//unhook_event_listener(HT_UI, this);
		}

		void RetDec::modifyFunctions(
			Token::Kind k,
			const std::string& oldVal,
			const std::string& newVal)
		{
			for (auto& p : fnc2fnc)
			{
				modifyFunction(p.first, k, oldVal, newVal);
			}
		}

		void RetDec::modifyFunction(
			func_t* f,
			Token::Kind k,
			const std::string& oldVal,
			const std::string& newVal)
		{
			auto fIt = fnc2fnc.find(f);
			if (fIt == fnc2fnc.end())
			{
				return;
			}
			Function& F = fIt->second;

			std::vector<Token> newTokens;

			for (auto& t : F.getTokens())
			{
				if (t.second.kind == k && t.second.value == oldVal)
				{
					newTokens.emplace_back(Token(k, t.second.ea, newVal));
				}
				else
				{
					newTokens.emplace_back(t.second);
				}
			}

			fIt->second = Function(f, newTokens);
		}

		ea_t RetDec::getFunctionEa(const std::string& name)
		{
			// USe config.
			auto* f = config->functions.getFunctionByName(name);
			if (f && f->getStart().isDefined())
			{
				return f->getStart();
			}

			// Use IDA.
			for (unsigned i = 0; i < get_func_qty(); ++i)
			{
				func_t* f = getn_func(i);
				qstring qFncName;
				get_func_name(&qFncName, f->start_ea);
				if (qFncName.c_str() == name)
				{
					return f->start_ea;
				}
			}

			return BADADDR;
		}

		func_t* RetDec::getIdaFunction(const std::string& name)
		{
			auto ea = getFunctionEa(name);
			return ea != BADADDR ? get_func(ea) : nullptr;
		}

		ea_t RetDec::getGlobalVarEa(const std::string& name)
		{
			auto* g = config->globals.getObjectByName(name);
			if (g && g->getStorage().getAddress())
			{
				return g->getStorage().getAddress();
			}
			return BADADDR;
		}




		std::shared_ptr<retdec::config::Config> getGlobalConfing()
		{
			std::shared_ptr<RetDec> ctxInst = RetDec_Global_Instance();
			return ctxInst->config;
		}
	}
}