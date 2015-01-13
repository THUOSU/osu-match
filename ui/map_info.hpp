#ifndef THUOSU_MATCH_MAP_INFO_HPP
#define THUOSU_MATCH_MAP_INFO_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

#include "filesystem.hpp"
#include "matcher.hpp"

namespace thuosu
{
	struct map_info
	{
		static filesystem::path osu_dir;

		std::wstring artist, artist_romanized, title, title_romanized, source;
		filesystem::path location, music_file;
		std::vector<std::wstring> tags;
		
		bool matched() const
		{
			return has_matched(osu_dir / location / music_file);
		}

		filesystem::path full_path() const
		{
			return location / music_file;
		}

		bool match_key(const std::wstring & key) const
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
	template <typename Reader>
	inline void get_location(Reader & reader, map_info & info)
	{
		info.location = reader.read<std::wstring>();
	}
	template <typename Reader>
	inline void get_music_file(Reader & reader, map_info & info)
	{
		info.music_file = reader.read<std::wstring>();
	}
	
	filesystem::path map_info::osu_dir;
}

#endif