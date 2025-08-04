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
};
inline ProcessInfo SGProccesInfo{};