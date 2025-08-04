#pragma once
#include "include.h"

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

