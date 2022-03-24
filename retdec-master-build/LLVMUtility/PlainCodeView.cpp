#include "stdafx.h"
#include "PlainCodeView.h"
#include "retdecida.h"
namespace retdec {
	namespace plugin {
		void idaapi ct_close(TWidget* cv, void* ud)
		{
			RetDec* that = static_cast<RetDec*>(ud);
			if (that&&that->codeViewer)
			{
				//close_widget(that->codeViewer, 0);
				that->custViewer = nullptr;
				that->codeViewer = nullptr;
			}
		}

		/// @name Callbacks reacting on custom viewer.
		/// @{
		bool idaapi ct_keyboard(TWidget* cv, int key, int shift, void* ud)
		{
			return false;
		}

		bool idaapi ct_double(TWidget* cv, int shift, void* ud)
		{
			return false;
		}

		/*void idaapi ct_close(TWidget* cv, void* ud);*/
		/// @}

		void registerPermanentActions();
		ssize_t idaapi ui_callback(void* ud, int notification_code, va_list va);

		/// @name Functions working with GUI from threads.
		/// @{
		int idaapi showDecompiledCode(void *ud);
		/// @}

		/**
		 * All the handlers for our custom view.
		 */
		static const custom_viewer_handlers_t handlers(
			ct_keyboard, // keyboard
			//nullptr,     // keyboard
			nullptr,     // mouse_moved
			nullptr,     // click	
			ct_double,   // dblclick
			//nullptr,     // dblclick
			nullptr,     // current position change
		//	nullptr,     // close
			ct_close,     // close
			nullptr,     // help
			nullptr,     // adjust_place
			nullptr,
			nullptr,
			nullptr
		);

		PlainCodeView::~PlainCodeView()
		{
			if (plg.codeViewer != nullptr)
			{
				close_widget(plg.codeViewer, 0);
				plg.custViewer = nullptr;
				plg.codeViewer = nullptr;
			}
		}

		PlainCodeView::PlainCodeView(RetDec & p) :plg(p)
		{
		}

		int PlainCodeView::execute()
		{

			char viewcodename[_MAX_PATH];

			if (plg.codeViewer != nullptr)
			{
				close_widget(plg.codeViewer, 0);
				plg.custViewer = nullptr;
				plg.codeViewer = nullptr;
				return 0;
			}

			// Without setting both x and y in render info, the current line gets
			// displayed as the first line in the viewer. Which is not nice because we
			// don't see the context before it. It is better if it is somewhere in the
			// middle of the viewer.

			func_t* fnc = plg.decompiledFunction;
			auto& contents = plg.fnc2code[fnc].idaCode;
			auto& code = plg.fnc2code[fnc].code;
			std::istringstream f(code);
			std::string line;
			contents.clear();
			while (std::getline(f, line))
			{
				contents.push_back(simpleline_t(line.c_str()));
			}

			simpleline_place_t minPlace;
			simpleline_place_t curPlace = minPlace;
			simpleline_place_t maxPlace(contents.size() - 1);
			if (fnc->start_ea)
			{
				qsnprintf(viewcodename, _MAX_PATH, "sub_%x", fnc->start_ea);
			}
			else {
				qsnprintf(viewcodename, _MAX_PATH, "Decompiled Code Full");
			}
			plg.custViewer = create_custom_viewer(
				viewcodename,        // title
				&minPlace,              // first location of the viewer
				&maxPlace,              // last location of the viewer
				&curPlace,              // set current location
				nullptr,                // renderer information (can be NULL)
				&contents,              // contents of viewer
				&handlers,              // handlers for the viewer (can be NULL)
				&plg,
				nullptr       // parent widget
			);
			set_view_renderer_type(plg.custViewer, TCCRT_FLAT);
			plg.codeViewer = create_code_viewer(plg.custViewer);
			set_code_viewer_is_source(plg.codeViewer);
			display_widget(plg.codeViewer, WOPN_TAB | WOPN_RESTORE | WOPN_NOT_CLOSED_BY_ESC);


			return 0;
		}
	}
}