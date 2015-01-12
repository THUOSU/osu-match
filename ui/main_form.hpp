/*!
* THUOSU Library Defination
*
* \brief
*
*/

#ifndef THUOSU_MAIN_FORM_HPP
#define THUOSU_MAIN_FORM_HPP

#include <memory>
#include <utility>
#include <nana/traits.hpp>
#include "filesystem.hpp"

namespace thuosu
{
	class main_form final : nana::noncopyable, nana::nonmovable
	{
		struct impl;
		std::unique_ptr<impl> _impl;
	public:
		main_form(const filesystem::path & osudb_path);
		~main_form();
		
		void show();
	};
}

#endif //THUOSU_MAIN_FORM_HPP