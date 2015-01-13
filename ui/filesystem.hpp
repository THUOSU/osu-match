#ifndef THUOSU_FILESYSTEM_HPP
#define THUOSU_FILESYSTEM_HPP

#include "config.hpp"


#if THUOSU_WINDOWS
#include <filesystem>

namespace thuosu
{
	namespace filesystem
	{
		namespace sys = std::tr2::sys;

		using path = sys::wpath;
		using sys::filesystem_error;
		using directory_entry = sys::wdirectory_entry;
		using directory_iterator = sys::wdirectory_iterator;
		using recursive_directory_iterator = sys::wrecursive_directory_iterator;
		// missing perms
		using sys::file_status;
		using sys::space_info;
		using sys::copy_option;
		// missing directory_options
		// missing file_time_type
		// missing absolute;
		// missing canonical;
		// missing copy;
		using sys::copy_file;
		// missing copy_symlink;
		using sys::create_directory;
		using sys::create_directories;
		using sys::create_hard_link;
		using sys::create_symlink;
		// missing create_directory_symlink;
		inline path current_path() { return sys::current_path<path>(); }
		inline void current_path(const path & p) { sys::current_path(p); }
		using sys::exists;
		using sys::equivalent;
		using sys::file_size;
		// missing hard_link_count;
		// missing is_block_file;
		// missing is_character_file;
		using sys::is_directory;
		using sys::is_empty;
		// missing is_fifo;
		using sys::is_other;
		using sys::is_regular_file;
		// missing is_socket;
		using sys::is_symlink;
		// missing temp_directory_path;
		using sys::swap;
		using sys::operator==;
		using sys::operator!=;
		using sys::operator<;
		using sys::operator<=;
		using sys::operator>;
		using sys::operator>=;
		using sys::operator/;
		using sys::operator<<;
		using sys::operator>>;
		// missing u8path;

		inline void rename(const path & old_path, const path & new_path)
		{
			auto code = sys::_Rename(old_path.string().c_str(), new_path.string().c_str());
			if (code != 0)
				throw filesystem_error{ "cannot rename file." };
		}


		using sys::remove;
		using sys::remove_all;
		using sys::remove_directory;
		using sys::remove_filename;

		inline path operator / (const path & p1, const path::string_type & p2)
		{
			return p1 / path{ p2 };
		}
	}
}

#else
#error file system not supported

#endif

#endif // THUOSU_FILESYSTEM_HPP