/*!
* THUOSU Library Defination
*
* \brief
*
*/

#ifndef THUOSU_MATCH_DIALOG_HPP
#define THUOSU_MATCH_DIALOG_HPP

#include <memory>
#include <utility>
#include <nana/gui/widgets/form.hpp>

namespace thuosu
{
	class match_dialog final : nana::noncopyable, nana::nonmovable
	{
		struct impl;
		std::unique_ptr<impl> _impl;
	public:
		match_dialog();
		~match_dialog();

		void src_file(const std::wstring & file);
		const std::wstring & src_file() const;

		void show_dialog(const nana::form & parent);
	};
}

#endif // THUOSU_MATCH_DIALOG_HPP