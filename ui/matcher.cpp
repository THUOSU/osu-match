#include "matcher.hpp"
#include "filesystem.hpp"
#include "utils.hpp"

#include <exception>
#include <cstdlib>
#include <string>

using namespace thuosu::filesystem;

inline static path mbak_path(const path & src)
{
	path mbak_file{ src };
	mbak_file.replace_extension(mbak_file.extension() + L".mbak");
	return mbak_file;
}

inline static std::wstring to_filename_string(const path & src)
{
	auto str = static_cast<std::wstring>(src);
	thuosu::trim(str);
	if (str.front() != L'\"')
		return L'\"' + str + L'\"';
	return str;
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
			static auto match_exe = executable_directory() / L"osumatch.exe";
			if (!is_regular_file(match_exe))
				throw std::runtime_error{ "missing osumatch.exe" };
			auto mbak_file = mbak_path(src);
			if (src == dst || mbak_file == dst)
				throw std::invalid_argument{ "invalid file name" };
			if (!(is_regular_file(src) || is_regular_file(mbak_file)) || !is_regular_file(dst))
				throw std::invalid_argument{ "file not exists" };
			if (!exists(mbak_file))
				rename(src, mbak_file);

			int exit_code = execute(
				match_exe,
				to_filename_string(dst)
				+ L' ' + to_filename_string(mbak_file) 
				+ L' ' + to_filename_string(src)
			);
			std::cout << "osumatch.exe: " << exit_code << std::endl;
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
