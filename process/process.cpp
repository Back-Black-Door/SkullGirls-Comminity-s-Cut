#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>
#include "process.h"

DWORD GetModuleBaseAddress();

bool GetCurrentProcessInfo()
{
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hCurrentThread = GetCurrentThread(); // Получаем реальный handle потока

    HANDLE hRealHandleProcess, hRealHandleThread;

    // Дублируем handle процесса
    if (!DuplicateHandle(GetCurrentProcess(), hProcess,
        GetCurrentProcess(), &hRealHandleProcess,
        0, FALSE, DUPLICATE_SAME_ACCESS)) {
        return FALSE;
    }

    // Дублируем handle потока 
    if (!DuplicateHandle(GetCurrentProcess(), hCurrentThread,
        GetCurrentProcess(), &hRealHandleThread,
        0, FALSE, DUPLICATE_SAME_ACCESS)) {
        CloseHandle(hRealHandleProcess);
        return FALSE;
    }

    SGProccesInfo.SGpi.hProcess = hRealHandleProcess;
    SGProccesInfo.SGpi.hThread = hRealHandleThread; // ← Теперь это настоящий handle потока
    SGProccesInfo.SGpi.dwProcessId = GetCurrentProcessId();
    SGProccesInfo.SGpi.dwThreadId = GetCurrentThreadId();
    SGProccesInfo.dwBaseAddress = GetModuleBaseAddress();
    return TRUE;
}

DWORD GetModuleBaseAddress() {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot;
    while (dwModuleBaseAddress == 0) {
        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, SGProccesInfo.SGpi.dwProcessId); // make snapshot of all modules within process
        MODULEENTRY32 ModuleEntry32 = { 0 };
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
        {
            do {
                if (_tcscmp(ModuleEntry32.szModule, "Skullgirls.exe") == 0 || _tcscmp(ModuleEntry32.szModule, "SkullGirls.exe") == 0) // if Found Module matches Module we look for -> done!
                {
                    dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32
        }
    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

void ProcessInfo::CloseHandles() {
    CloseHandle(SGProccesInfo.SGpi.hProcess);
    CloseHandle(SGProccesInfo.SGpi.hThread);
}