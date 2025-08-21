#pragma once
#include "include.h"

struct ProcessInfo {
	STARTUPINFO SGsi;
	PROCESS_INFORMATION SGpi;
	DWORD dwBaseAddress;

	~ProcessInfo() {
		CloseHandle(SGpi.hThread);
		CloseHandle(SGpi.hProcess);
	}
};

namespace main_paths {
	inline fs::path original_exe_path;
	inline fs::path exe_path;
	inline fs::path work_dir_path;
	inline fs::path data01_dir_path;
	inline fs::path data02_dir_path;
	inline fs::path mods_dir_path;
	inline fs::path save_file_path;
	inline fs::path sal_file_path;
};
namespace main_paths_str {
	inline std::string original_exe_path;
	inline std::string exe_path;
	inline std::string work_dir_path;
	inline std::string data01_dir_path;
	inline std::string data02_dir_path;
	inline std::string mods_dir_path;
	inline std::string save_file_path;
	inline std::string sal_file_path;
};
inline ProcessInfo SGProccesInfo{};
