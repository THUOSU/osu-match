#include "main_form.hpp"
#include "config.hpp"
#define THUOSU_BEATMAP_INFO_LIMIT 1000
#include "io/beatmap_info.hpp"
#include "map_info.hpp"
#include "shared_resources.hpp"
#include "utils.hpp"
#include "match_dialog.hpp"

#include <iostream>
#include <locale>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cwctype>
#include <cstdint>
#include <unordered_map>

#include <nana/any.hpp>
#include <nana/gui.hpp>
#include <nana/internationalization.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/progress.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/checkbox.hpp>

using namespace thuosu;
using namespace std;
using namespace nana;

inline nana::listbox::oresolver & operator << (nana::listbox::oresolver & ore, const thuosu::map_info & info)
{
	auto bg_color = info.matched ? 0xaaffaa : 0xffffff;
	for (const auto & str : { info.artist, info.title, info.full_path() })
		ore << nana::listbox::cell(str, bg_color, 0);
	return ore;
}

struct main_form::impl : nana::noncopyable, nana::nonmovable
{
	unordered_map<wstring, map_info> info_dict;
	form fm;
	listbox lst_songs;
	label lbl_songs;
	textbox txt_search;
	button btn_search;
	checkbox chk_matched;
	place pl;

	static rectangle init_rect()
	{
		auto size = API::screen_size();
		return API::make_center(size.width / 2, size.height / 2);
	}

	impl(const filesystem::path & osudb_path)
		: fm{ init_rect() },
		lst_songs{ fm }, lbl_songs{ fm }, txt_search{ fm }, btn_search{ fm }, chk_matched{ fm },
		pl{ fm }
	{
		load_osu_db(osudb_path);
		initialize_widgets();
		register_event_handlers();
		
		////////////////////////// for test! ////////////////////////
		srand(static_cast<unsigned int>(time(nullptr)));
		for (auto & kv : info_dict)
		{
			auto & value = kv.second;
			value.matched = rand() % 10 == 0;
			auto & item = lst_songs.at(0).append(value);
			item.value(value);
		}
	}
	
	void initialize_widgets()
	{
		static internationalization i18n{};
		auto bg_color = 0xefefef;

		fm.icon(main_icon());
		fm.caption(i18n("form_title"));
		fm.background(bg_color);

		auto width = fm.size().width;
		lst_songs.append_header(i18n("Artist"), width / 6);
		lst_songs.append_header(i18n("Title"), width / 3);
		lst_songs.append_header(i18n("File"), width / 2);

		lbl_songs.caption(i18n("Songs") + L":");
		lbl_songs.background(bg_color);
		lbl_songs.text_align(align::left, align_v::center);

		chk_matched.caption(i18n("Matched only"));
		chk_matched.background(bg_color);

		txt_search.multi_lines(false);

		btn_search.background(bg_color);

		pl.div("margin=10 vertical"
			"<weight=22 <search arrange=[variable, 120, 100, 22]> >"
			"<weight=10>"
			"<lst>"
			);
		pl["lst"] << lst_songs;
		pl["search"] << lbl_songs << chk_matched << txt_search << btn_search;
		pl.collocate();
	}
	
	void register_event_handlers()
	{
		lst_songs.events().dbl_click([&](const nana::arg_mouse & e) {
			static match_dialog dialog{};
			auto selected = lst_songs.selected();
			if (selected.size() > 0)
			{
				const auto & info = lst_songs.at(0).at(selected.front().item).value<map_info>();
				dialog.src_file(info.full_path());
				wcout << dialog.src_file() << endl;
				dialog.show_dialog(fm);
			}
		});
		auto filter = bind(&impl::filter_maps, this);
		chk_matched.events().click(filter);
		txt_search.events().key_press([this](const arg_keyboard & arg){
			if (arg.key == '\r')
				filter_maps();
		});
		btn_search.events().click(filter);
	}

	void load_osu_db(const filesystem::path & osudb_path)
	{
		vector<map_info> list;
		cout << "loading osu!.db..." << endl;
		{
			performance_timer timer{};
			list = io::load_beatmap_info<map_info>(osudb_path);
			cout << list.size() << " beatmap(s) loaded." << endl;
		}
		cout << "regrouping..." << endl;
		{
			performance_timer timer{};
			for (auto & info : list)
			{
				const auto & full_path = info.full_path();
				if (info_dict.find(full_path) == info_dict.end())
					info_dict[full_path] = info;
			}
		}
	}
	
	void filter_maps()
	{
		lst_songs.clear();
		cout << "filtering..." << endl;
		{
			lst_songs.auto_draw(false);
			bool matched_only = chk_matched.checked();
			performance_timer timer{};
			auto keys = split(txt_search.caption());
			for (const auto & kv : info_dict)
			{
				auto & value = kv.second;
				if (matched_only && !value.matched)
					continue;
				if (all_of(keys.begin(), keys.end(), [&value](const wstring& key){ return value.match(key); }))
				{
					auto & item = lst_songs.at(0).append(value);
					item.value(value);
				}
			}
			lst_songs.auto_draw(true);
		}
	}

	void show()
	{
		fm.show();
	}
};

main_form::main_form(const filesystem::path & osudb_path)
	: _impl{ new main_form::impl{ osudb_path } }
{ }

main_form::~main_form()
{ }

void main_form::show()
{
	_impl->show();
}