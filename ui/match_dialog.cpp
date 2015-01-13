#include "config.hpp"
#include "shared_resources.hpp"
#include "match_dialog.hpp"
#include "matcher.hpp"

#include <iostream>

#include <nana/gui/place.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/progress.hpp>
#include <nana/filesystem/fs_utility.hpp>
#include <nana/filesystem/file_iterator.hpp>

using namespace thuosu;
using namespace nana;

struct match_dialog::impl
{
	thuosu::filesystem::path src_file;
};

match_dialog::match_dialog()
	: _impl{ new match_dialog::impl{} }
{ }

match_dialog::~match_dialog()
{ }

void match_dialog::src_file(const thuosu::filesystem::path & file)
{
	_impl->src_file = file;
}

const thuosu::filesystem::path & match_dialog::src_file() const
{
	return _impl->src_file;
}

#if THUOSU_WINDOWS
#define WIN32_LEAN_AND_MIN
#include <windows.h>
#endif

void match_dialog::show_dialog(const nana::form & parent)
{
	static internationalization i18n{};
	color_t bg_color = make_rgb(240, 240, 240);
	form fm{ parent.handle(), API::make_center(parent.handle(), 560, 200), appearance{ true, true, true, true, false, false, false } };
	fm.icon(main_icon());
	fm.caption(i18n("Match") + L"...");

#if THUOSU_WINDOWS
	HWND hwnd = reinterpret_cast<HWND>(API::root(fm));
	SetWindowLong(hwnd, GWL_EXSTYLE, (GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_APPWINDOW) | WS_EX_DLGMODALFRAME);
#endif

	fm.background(bg_color);
	
	label lbl_src{ fm }, lbl_dst{ fm };
	textbox txt_src{ fm }, txt_dst{ fm };
	button btn_dst{ fm }, btn_ok{ fm }, btn_cancel{ fm };
	progress prg{ fm };
	lbl_src.caption(i18n("Source file") + L": ");
	lbl_src.background(bg_color);
	lbl_src.text_align(align::left, align_v::center);
	
	txt_src.multi_lines(false);
	txt_src.editable(false);
	txt_src.caption(_impl->src_file);
	
	lbl_dst.caption(i18n("Destination file") + L": ");
	lbl_dst.background(bg_color);
	lbl_dst.text_align(align::left, align_v::center);

	txt_dst.multi_lines(false);
	txt_dst.editable(false);
	
	btn_dst.caption(L"...");
	btn_dst.background(bg_color);
	btn_dst.events().click([&](){
		static std::wstring init_path;
		filebox fb{ fm, true };
		if (init_path != L"")
			fb.init_path(init_path);
		fb.add_filter(i18n("Music file") + L" (*.wav;*.mp3;*.ogg)", L"*.wav;*.mp3;*.ogg");
		fb.add_filter(i18n("All files") + L" (*.*)", L"*.*");
		if (fb())
		{
			txt_dst.caption(fb.file());
			init_path = fb.path();
			btn_ok.enabled(true);
		}
	});

	btn_ok.i18n("Ok");
	btn_ok.background(bg_color);
	btn_ok.events().click([&](){
		(msgbox{ fm, L"info" } << L"partial implemented.").icon(msgbox::icon_information)();
		try
		{
			match_music(_impl->src_file, txt_dst.caption());
		}
		catch (const std::exception & exc)
		{
			std::cout << exc.what() << std::endl;
		}
	});

	btn_cancel.i18n("Cancel");
	btn_cancel.background(bg_color);
	btn_cancel.events().click([&](){
		fm.close();
	});

	prg.unknown(true);
	timer tim{};
	tim.interval(50);
	tim.elapse([&](){
		prg.inc();
	});

	place pl{ fm };
	pl.div("margin=20 vertical"
		"<src weight=24 arrange=[variable, 400]>"
		"<weight=20>"
		"<dst weight=24 arrange=[variable, 375, 25]>"
		"<weight=20>"
		"<prg weight=10>"
		"<weight=20>"
		"<weight=30 <><ok weight=100><weight=20><cancel weight=100>>"
		);
	pl["src"] << lbl_src << txt_src;
	pl["dst"] << lbl_dst << txt_dst << btn_dst;
	pl["prg"] << prg;
	pl["ok"] << btn_ok;
	pl["cancel"] << btn_cancel;
	pl.field_visible("prg", false);
	pl.collocate();
	API::modal_window(fm);
}