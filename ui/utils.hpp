#ifndef THUOSU_MATCH_UTILS
#define THUOSU_MATCH_UTILS

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>

#include "filesystem.hpp"

namespace thuosu
{
	template <typename Char>
	struct whitespace;

	template <>
	struct whitespace < char > { const static char value = ' '; };

	template <>
	struct whitespace < wchar_t > { const static wchar_t value = L' '; };

	template <typename Char>
	std::vector<std::basic_string<Char>> split(const std::basic_string<Char> str, Char splitter = whitespace<Char>::value)
	{
		using namespace std;

		vector<basic_string<Char>> res;
		basic_stringstream<Char> stream{ str };
		basic_string<Char> s;
		while (getline(stream, s, splitter))
			res.push_back(move(s));
		return res;
	}

	template <typename Char>
	inline std::basic_string<Char> & ltrim(std::basic_string<Char> & s, Char c = whitespace<Char>::value)
	{
		auto pos = s.find_first_not_of(c);
		if (pos != s.npos)
			s.erase(0, pos);
		return s;
	}

	template <typename Char>
	inline std::basic_string<Char> & rtrim(std::basic_string<Char> & s, Char c = whitespace<Char>::value)
	{
		auto pos = s.find_last_not_of(c);
		if (pos != s.npos)
			s.erase(pos + 1);
		return s;
	}

	template <typename Char>
	inline std::basic_string<Char> &trim(std::basic_string<Char> & s, Char c = whitespace<Char>::value)
	{
		return ltrim(rtrim(s, c), c);
	}

	const filesystem::path & executable_filename();

	const filesystem::path & executable_directory();

	//!\brief try find osu path (through registry on windows)
	filesystem::path find_osu_path();

	bool ensure_single_instance();

	struct performance_timer
	{
		performance_timer();
		~performance_timer();
	private:
		std::int64_t tick;
	};
}
#endif // THUOSU_MATCH_UTILS