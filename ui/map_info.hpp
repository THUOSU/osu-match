#ifndef THUOSU_MATCH_MAP_INFO_HPP
#define THUOSU_MATCH_MAP_INFO_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

namespace thuosu
{
	struct map_info
	{
		std::wstring artist, artist_romanized, title, title_romanized, location, music_file, source;
		std::vector<std::wstring> tags;
		bool matched;

		std::wstring full_path() const
		{
			return location + L'\\' + music_file;
		}

		bool match(const std::wstring & key) const
		{
			if (is_sub(artist, key)
				|| is_sub(artist_romanized, key)
				|| is_sub(title, key)
				|| is_sub(title_romanized, key)
				|| is_sub(source, key)
				)
				return true;
			if (tags.size() == 0)
				return false;
			return std::all_of(tags.begin(), tags.end(), [&key](const std::wstring & str){ return is_sub(str, key); });
		}

	private:
		static bool is_sub(const std::wstring & src, const std::wstring & key)
		{
			return std::search(src.begin(), src.end(), key.begin(), key.end(), [](wchar_t c1, wchar_t c2){ return std::towupper(c1) == std::towupper(c2); })
				!= src.end();
		}
	};
	template <typename Reader>
	inline void get_tags(Reader & reader, map_info & info)
	{
		info.tags = split(reader.read<std::wstring>());
	}
}

#endif