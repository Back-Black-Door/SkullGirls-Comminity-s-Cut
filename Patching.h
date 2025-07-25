#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <tchar.h>

namespace PachingUtils {
	const DWORD getppid();
	const DWORD GetModuleBaseAddress(DWORD pID);
	const BOOL PidNameTest(DWORD processId, const std::string& moduleName);
	const PVOID CreateNewPage(HANDLE& ThreadHande, HANDLE& ProcessHandle);
	const bool NewsBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const bool GFSValidathionBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const bool SalValidathionBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const bool ChangeDataDirectoryFirstTime(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const bool ChangeSal(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const void Debugger(DEBUG_EVENT& DebugEv, const HANDLE& ThreadHande, const HANDLE& ProcessHandle);
}

struct ProcessInfo {
	STARTUPINFO SGsi;
	PROCESS_INFORMATION SGpi;
	DWORD dwBaseAddress;

	~ProcessInfo() {
		CloseHandle(SGpi.hThread);
		CloseHandle(SGpi.hProcess);
	}
};

inline ProcessInfo SGProccesInfo;