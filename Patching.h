#pragma once
#include <Windows.h>
#include <iostream>

namespace PachingUtils {
	const PVOID CreateNewPage(HANDLE& ThreadHande, HANDLE& ProcessHandle);

	const bool NewsBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const bool GFSValidathionBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const bool SalValidathionBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const bool ChangeDataDirectoryFirstTime(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const bool ChangeSal(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress);
	const void Debugger(DEBUG_EVENT& DebugEv, const HANDLE& ThreadHande, const HANDLE& ProcessHandle);
}
