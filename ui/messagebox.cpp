#include "messagebox.hpp"
#include "config.hpp"
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/place.hpp>
#include <nana/internationalization.hpp>
#include <string>

#if THUOSU_WINDOWS
#define WIN32_LEAN_AND_MIN
#include <windows.h>
#endif

namespace thuosu
{
	msgbox_result show_msgbox(nana::window parent,
		const std::wstring & message,
		const std::wstring & title,
		msgbox_button msg_btn,
		msgbox_icon msg_icon)
	{
		using namespace nana;
		static internationalization i18n{};
		color_t bg_color = make_rgb(240, 240, 240);

		form fm{ parent, API::make_center(parent, 300, 160), appearance{ true, true, false, true, false, false, false } };
		fm.caption(title);
		fm.background(bg_color);

#if THUOSU_WINDOWS
		HWND hwnd = reinterpret_cast<HWND>(API::root(fm));
		SetWindowLong(hwnd, GWL_EXSTYLE, (GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_APPWINDOW) | WS_EX_DLGMODALFRAME);
#endif
		
		label lbl{ fm };
		lbl.caption(message);
		lbl.background(bg_color);
		
		place pl{ fm };
		pl.div("margin=20 vert <txt weight=80> <weight=10> <btn weight=30 <><btn gap=5>>");
		pl["txt"] << lbl;

		msgbox_result result;
		button btn_ok, btn_cancel, btn_yes, btn_no;
		auto make_ok = [&](){
			btn_ok.create(fm);
			btn_ok.i18n("Ok");
			btn_ok.events().click([&](){ result = msgbox_result::ok; fm.close(); });
			pl["btn"] << btn_ok;
		};
		auto make_yes = [&](){
			btn_yes.create(fm);
			btn_yes.i18n("Yes");
			btn_yes.events().click([&](){ result = msgbox_result::yes; fm.close(); });
			pl["btn"] << btn_yes;
		};
		auto make_no = [&](){
			btn_no.create(fm);
			btn_no.i18n("No");
			btn_no.events().click([&](){ result = msgbox_result::no; fm.close(); });
			pl["btn"] << btn_no;
		};
		auto make_cancel = [&](){
			btn_cancel.create(fm);
			btn_cancel.i18n("Cancel");
			btn_cancel.events().click([&](){ result = msgbox_result::cancel; fm.close(); });
			pl["btn"] << btn_cancel;
		};
		
		switch (msg_btn)
		{
		case msgbox_button::ok:
			result = msgbox_result::ok;
			make_ok();
			break;
		case msgbox_button::ok_cancel:
			result = msgbox_result::cancel;
			make_ok(); make_cancel();
			break;
		case msgbox_button::yes_no:
			result = msgbox_result::no;
			make_yes(); make_no();
			break;
		case msgbox_button::yes_no_cancel:
			result = msgbox_result::cancel;
			make_yes(); make_no(); make_cancel();
			break;
		}
		pl.collocate();
		
		API::modal_window(fm);
		return result;
	}
}