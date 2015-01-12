/*!
* THUOSU Library Defination
*
* \brief 
*
*/

#ifndef THUOSU_UI_SETTINGS_HPP
#define THUOSU_UI_SETTINGS_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include "filesystem.hpp"
#include "utils.hpp"

namespace thuosu
{
	class settings final
	{
		struct impl;
		std::shared_ptr<impl> p_impl;
	public:

		settings();
		settings(const filesystem::path & path, bool auto_save = false);

		~settings();

		bool has_key(const std::wstring & key) const;
		const std::wstring & operator[] (const std::wstring & key) const;
		std::wstring & operator[] (const std::wstring & key);

		void save() const;
		void save(const filesystem::path & filename);

		std::unordered_map<std::wstring, std::wstring> key_values() const;
	};

	inline settings & get_settings()
	{
		static settings instance{};
		return instance;
	}

	inline settings & global_settings()
	{
		static settings g_settings{ executable_directory() / filesystem::path{ L"settings.ini" }, true };
		return g_settings;
	}
}

#endif // THUOSU_UI_SETTINGS_HPP