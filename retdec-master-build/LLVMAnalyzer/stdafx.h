#ifndef RETDEC_STDAFX_H
#define RETDEC_STDAFX_H
#pragma once
#include <pro.h>

#include "targetver.h"
#include <string>

#ifdef PLUGIN_SUBMODULE
#define ida_dll_data __declspec(dllexport)
#else
#define ida_dll_data __declspec(dllimport)
#endif
namespace retdec {
	namespace plugin {

		extern ida_dll_data  const std::string  pluginName;
		extern  ida_dll_data const std::string  pluginVersion;
		int idaapi init(void);
		void idaapi term();
		bool idaapi run_plugin(size_t arg);
		ida_dll_data void addvtbl2fns(ea_t vtbl, ea_t fn);
		ida_dll_data void addvtbl2fns2seg(ea_t vtbl);

	}
}

#endif


