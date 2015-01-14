#ifndef THUOSU_MATCHER_HPP
#define THUOSU_MATCHER_HPP

#include "filesystem.hpp"

namespace thuosu
{
	namespace matcher
	{
		bool has_matched(const filesystem::path & music_file);

		void match_music(const filesystem::path & src, const filesystem::path & dst);

		void recover(const filesystem::path & music_file);
	}
}

#endif // THUOSU_MATCHER_HPP