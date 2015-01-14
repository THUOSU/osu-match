#include "matcher.hpp"
#include "filesystem.hpp"

#include <exception>
#include <system_error>

using namespace thuosu::filesystem;

inline static path mbak_path(const path & src)
{
	path mbak_file{ src };
	mbak_file.replace_extension(mbak_file.extension() + L".mbak");
	return mbak_file;
}

namespace thuosu
{
	namespace matcher
	{
		bool has_matched(const path & music_file)
		{
			return is_regular_file(mbak_path(music_file));
		}

		void match_music(const path & src, const path & dst)
		{
			auto mbak_file = mbak_path(src);
			if (!exists(mbak_file))
				rename(src, mbak_file);

			/////////////// not implemented ////////////////////
			copy_file(dst, src, copy_option::overwrite_if_exists);
		}

		void recover(const path & music_file)
		{
			path mbak_file{ music_file };
			mbak_file.replace_extension(mbak_file.extension() + L".mbak");
			if (exists(music_file))
				remove(music_file);
			rename(mbak_file, music_file);
		}
	}
}
