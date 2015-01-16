#ifndef THUOSU_MESSAGEBOX_HPP
#define THUOSU_MESSAGEBOX_HPP

#include <memory>
#include <nana/gui/msgbox.hpp>
#include <nana/internationalization.hpp>

namespace thuosu
{
	enum class msgbox_button
	{
		ok,
		ok_cancel,
		yes_no,
		yes_no_cancel,
	};

	enum class msgbox_result
	{
		ok,
		cancel,
		yes,
		no
	};

	///////// not implemented yet /////////
	enum class msgbox_icon
	{
		none,
		information,
		error,
	};

	msgbox_result show_msgbox(nana::window parent,
		const std::wstring & message = L"",
		const std::wstring & title = L"",
		msgbox_button msg_btn = msgbox_button::ok,
		msgbox_icon msg_icon = msgbox_icon::none);
}

#endif