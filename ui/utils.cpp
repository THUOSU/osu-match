#include "config.hpp"
#include "utils.hpp"

#include <string>
#include <memory>

#if THUOSU_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

using namespace std;
namespace thuosu
{
	using namespace filesystem;
	path find_osu_path()
	{
		DWORD pcbData;
		if (RegGetValueW(HKEY_CLASSES_ROOT, L"osu!\\shell\\open\\command", NULL, RRF_RT_REG_SZ, NULL, NULL, &pcbData) != ERROR_SUCCESS)
			return path{};
		unique_ptr<wchar_t[]> buf{ new wchar_t[pcbData] };
		if (RegGetValueW(HKEY_CLASSES_ROOT, L"osu!\\shell\\open\\command", NULL, RRF_RT_REG_SZ, NULL, buf.get(), &pcbData) != ERROR_SUCCESS)
			return path{};
		wstring pa{ buf.get() };
		auto first = pa.find_first_of(L'\"');
		if (first == pa.npos)
			return path{};
		auto second = pa.find_first_of(L'\"', first + 1);
		if (second == pa.npos)
			return path{};
		path path{ pa.substr(first + 1, second - first - 1) };
		return path.parent_path();
	}

	const path & executable_filename()
	{
		static path exe_file = [](){
			wchar_t buf[FILENAME_MAX + 1];
			GetModuleFileNameW(0, buf, FILENAME_MAX + 1);
			return path{ buf };
		}();
		return exe_file;
	}

	const path & executable_directory()
	{
		static path exe_dir = executable_filename().parent_path();
		return exe_dir;
	}

	bool ensure_single_instance()
	{
		struct single_instance_guard
		{
			HANDLE h_mutex = 0;
			bool is_single = false;
			single_instance_guard()
			{
				auto uuid = L"356540b6-9bc6-11e4-811f-a4db30289c9b";
				HANDLE handle = CreateMutexExW(NULL, uuid, 0, MUTEX_ALL_ACCESS);
				is_single = (GetLastError() != ERROR_ALREADY_EXISTS);
			}
			single_instance_guard(const single_instance_guard&) = delete;
			single_instance_guard & operator= (const single_instance_guard&) = delete;
			~single_instance_guard()
			{
				if (h_mutex)
					CloseHandle(h_mutex);
			}
		};
		static single_instance_guard guard{};
		return guard.is_single;
	}

	performance_timer::performance_timer()
	{
		LARGE_INTEGER i;
		QueryPerformanceCounter(&i);
		this->tick = i.QuadPart;
	}

	performance_timer::~performance_timer()
	{
		LARGE_INTEGER i, f;
		QueryPerformanceCounter(&i);
		QueryPerformanceFrequency(&f);
		std::cout << "performance: " << static_cast<double>(i.QuadPart - this->tick) / f.QuadPart * 1000 << " ms" << std::endl;
	}

	int execute(const std::wstring & command, const std::wstring & param)
	{
		DWORD exitCode;
		SHELLEXECUTEINFOW ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = command.c_str();        
		ShExecInfo.lpParameters = param.c_str();   
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL; 
		ShellExecuteExW(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		GetExitCodeProcess(ShExecInfo.hProcess, &exitCode);
		return static_cast<int>(exitCode);
	}
}

#else

namespace thuosu
{
	using namespace filesystem;
	path find_osu_path()
	{
		return path{};
	}
}

#endif