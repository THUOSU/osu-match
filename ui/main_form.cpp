#include "main_form.hpp"
#include "config.hpp"
#define THUOSU_BEATMAP_INFO_LIMIT 1000
#include "io/beatmap_info.hpp"
#include "map_info.hpp"
#include "shared_resources.hpp"
#include "utils.hpp"
#include "settings.hpp"
#include "match_dialog.hpp"
#include "matcher.hpp"

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
using map_info_ptr = shared_ptr < map_info >;

inline listbox::oresolver & operator << (listbox::oresolver & ore, const map_info_ptr & info)
{
	auto bg_color = info->matched ? 0xaaffaa : 0xffffff;
	for (const auto & str : { info->artist, info->title, wstring(info->relative_path()) })
		ore << listbox::cell(str, bg_color, 0);
	return ore;
}

struct main_form::impl : nana::noncopyable, nana::nonmovable
{
	unordered_map<wstring, map_info_ptr> info_dict;
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

	impl(const filesystem::path & osu_dir_path)
		: fm{ init_rect() },
		lst_songs{ fm }, lbl_songs{ fm }, txt_search{ fm }, btn_search{ fm }, chk_matched{ fm },
		pl{ fm }
	{
		map_info::osu_dir = osu_dir_path;
		load_osu_db(osu_dir_path / filesystem::path{ L"osu!.db" });
		
		initialize_widgets();
		register_event_handlers();

		for (auto & kv : info_dict)
		{
			const auto & value = kv.second;
			auto & item = lst_songs.at(0).append(value);
			item.value(value);
		}
		
		global_settings()[L"dir"] = osu_dir_path;
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
				auto item = lst_songs.at(0).at(selected.front().item);
				const auto & info = item.value<map_info_ptr>();
				dialog.src_file(info->full_path());
				dialog.show_dialog(fm);
				info->matched = matcher::has_matched(info->full_path());
				item.resolve_from(info);
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
				const auto & p = info.relative_path();
				if (info_dict.find(p) == info_dict.end())
					info_dict[p] = make_shared<map_info>(info);
			}
			for (auto & kv : info_dict)
				kv.second->matched = matcher::has_matched(kv.second->full_path());
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
				if (matched_only && !value->matched)
					continue;
				if (all_of(keys.begin(), keys.end(), [&value](const wstring& key){ return value->match_key(key); }))
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

main_form::main_form(const filesystem::path & osu_dir_path)
	: _impl{ new main_form::impl{ osu_dir_path } }
{ }

main_form::~main_form()
{ }

void main_form::show()
{
	_impl->show();
}