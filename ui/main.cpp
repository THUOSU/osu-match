#include "config.hpp"
#include "main_form.hpp"
#include "filesystem.hpp"
#include "settings.hpp"
#include "utils.hpp"
#include "filesystem.hpp"

#include <iostream>
#include <exception>
#include <string>
#include <unordered_map>
#include <utility>
#include <codecvt>
#include <locale>
#include <nana/gui.hpp>
#include <nana/gui/filebox.hpp>

nana::internationalization i18n{};

void load_i18n()
{
	using namespace std;
	using namespace thuosu;
	using namespace filesystem;
	vector<pair<string, wstring>> default_i18n{
		{ "please_select_osu", L"Please select osu!.exe" },
		{ "form_title", L"osu! Matcher" },
		{ "db_not_found", L"osu!.db not found" }
	};
	for (const auto & kv : default_i18n)
		i18n.set(kv.first, kv.second);

	auto lang_file = executable_directory() / path{ L"lang" };
	if (is_regular_file(lang_file))
	{
		wstring_convert<codecvt_utf8<wchar_t>> converter{};
		auto key_values = settings{ lang_file }.key_values();
		for (const auto & kv : key_values)
			i18n.set(converter.to_bytes(kv.first), kv.second);
	}
}

#if defined(_DEBUG)
#if THUOSU_WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
int wmain()
#else
#if THUOSU_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
#else
#error "platform not supported."
#endif

#endif
{
	// check memory leaks
#if defined(_DEBUG) && THUOSU_WINDOWS
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#endif

	using namespace thuosu;
	using namespace filesystem;

	std::locale::global(std::locale{ "" });

	try
	{
		load_i18n();

		auto osu_path = find_osu_path();
		if (osu_path.empty())
		{
			if (!global_settings().has_key(L"dir"))
			{
				if (osu_path.empty())
				{
					nana::filebox fb{ nullptr, true };
					fb.add_filter(L"*.exe", L"*.exe");
					fb.title(i18n("please_select_osu"));
					if (!fb())
						return 0;
					if (!is_regular_file(osu_path / path{ L"osu!.db" }))
					{
						(nana::msgbox{ i18n("form_title") } << i18n("db_not_found"))();
						return 0;
					}
				}
			}
		}
		main_form fm{ osu_path / path{ L"osu!.db" } };
		fm.show();
		nana::exec();

		global_settings()[L"dir"] = osu_path;
	}
	catch (const std::exception & exc)
	{
		std::cout << exc.what() << std::endl;
	}

	return 0;
}
