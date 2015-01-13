#include "shared_resources.hpp"
#include "utils.hpp"

using namespace nana;

namespace thuosu
{

	const paint::image & main_icon()
	{
		static auto icon = paint::image{ executable_filename() };
		return icon;
	}

}