#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <exception>
#include <codecvt>

#include "settings.hpp"
#include "filesystem.hpp"
#include "utils.hpp"

using namespace thuosu;
using namespace std;
using namespace filesystem;

struct settings::impl
{

	//! \brief utf-8 string <-> wstring converter
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> & converter()
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_w_converter{};
		return utf8_w_converter;
	}

	unordered_map<wstring, wstring> map;
	path filename;
	bool auto_save = false;

	void load(const path & filename)
	{
		ifstream file{ filename };
		if (file)
		{
			string str;
			while (getline(file, str))
			{
				trim(str);
				if (str == "")
					continue;
				auto pos = str.find_first_of("=");
				if (pos == str.npos)
					continue;
				auto key = converter().from_bytes(str.substr(0, pos));
				auto value = converter().from_bytes(str.substr(pos + 1));
				map[trim(key)] = trim(value);
			}
		}
		this->filename = filename;
	}

	void save() const
	{
		ofstream file{ filename };
		if (!file)
			throw runtime_error{ "cannot open write file" };
		for (const auto & kv : map)
			file << converter().to_bytes(kv.first) << " = " << converter().to_bytes(kv.second) << endl;
	}

	void save(const path & filename)
	{
		ofstream file{ filename };
		if (!file)
			throw runtime_error{ "cannot open write file" };
		for (const auto & kv : map)
			file << converter().to_bytes(kv.first) << " = " << converter().to_bytes(kv.second) << endl;
		this->filename = filename;
	}

	wstring operator[] (const wstring & key) const
	{
		auto iter = map.find(key);
		if (iter != map.end())
			return iter->second;
		return L"";
	}

	wstring & operator[] (const wstring & key)
	{
		return map[key];
	}
};

settings::settings()
	: p_impl{ make_shared<settings::impl>() }
{}

settings::settings(const path & filename, bool auto_save)
	: settings{}
{
	p_impl->load(filename);
	p_impl->auto_save = auto_save;
}

settings::~settings()
{
	if (p_impl->auto_save)
	{
		try
		{
			save();
		}
		catch (...)
		{
		}
	}
}

void settings::save() const
{
	p_impl->save();
}

void settings::save(const path & filename)
{
	p_impl->save(filename);
}

bool settings::has_key(const wstring & key) const
{
	return p_impl->map.find(key) != p_impl->map.end();
}

const wstring & settings::operator[] (const wstring & key) const
{
	return (*p_impl)[key];
}

wstring & settings::operator[] (const wstring & key)
{
	return (*p_impl)[key];
}

unordered_map<wstring, wstring> settings::key_values() const
{
	return p_impl->map;
}