#pragma once
#include <Windows.h>
#include <iostream>
namespace PachingUtils {
	const void CreateNewPage(HANDLE& ThreadHande, HANDLE& ProcessHandle);

	const void NewsBreaker(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& NEWS);
	const void GFSValidathionBreaker(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& dotgfs);
	const void SalValidathionBreaker(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& dotsal);
	const void ChangeDataDirectoryFirstTime(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& data);
	const void ChangeSal(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& data);
	const void Debugger(DEBUG_EVENT& DebugEv, HANDLE& ThreadHande, HANDLE& ProcessHandle, bool& DebugOn);
}
