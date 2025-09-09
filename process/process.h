#pragma once
#include <Windows.h>
inline struct ProcessInfo {
	PROCESS_INFORMATION SGpi = { 0 };
	DWORD dwBaseAddress = { 0 };
	void CloseHandles();
} SGProccesInfo;

bool GetCurrentProcessInfo();