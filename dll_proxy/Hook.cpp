#include <Windows.h>
#include <iostream>
#include <d3d9.h>

#include "../thread/Thread.h"
#include "Hook.h"
#include "../Overlay/d3d9Wrapper.h"

// Указатель на оригинальную ExitProcess
void (WINAPI* OriginalExitProcess)(UINT uExitCode) = nullptr;

void WINAPI HookedExitProcess(UINT uExitCode) {
    std::cout << "ExitProcess hooked!" << "\n";
    g_workerThread.Stop();
    OriginalExitProcess(uExitCode);
}

BOOL InitializeHook()
{
    HMODULE hModule = GetModuleHandle(nullptr); // Получаем базовый адрес текущего модуля
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule +
        pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    // Ищем kernel32.dll в IAT
    for (; pImportDesc->Name; pImportDesc++) {
        const char* dllName = (const char*)((BYTE*)hModule + pImportDesc->Name);
        if (_stricmp(dllName, "kernel32.dll") == 0) {

            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->FirstThunk);
            for (; pThunk->u1.Function; pThunk++) {
                PROC* ppFunc = (PROC*)&pThunk->u1.Function;
                if (*ppFunc == GetProcAddress(GetModuleHandleA("kernel32.dll"), "ExitProcess")) {
                    DWORD oldProtect;
                    VirtualProtect(ppFunc, sizeof(PROC), PAGE_READWRITE, &oldProtect);
                    OriginalExitProcess = (void (WINAPI*)(UINT)) * ppFunc; // Сохраняем оригинал
                    *ppFunc = (PROC)HookedExitProcess; // Заменяем на свою функцию
                    VirtualProtect(ppFunc, sizeof(PROC), oldProtect, &oldProtect);
                    return true;
                }
            }
        }
        if (_stricmp(dllName, "d3d9.dll") == 0) {

            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->FirstThunk);
            for (; pThunk->u1.Function; pThunk++) {
                PROC* ppFunc = (PROC*)&pThunk->u1.Function;
                if (*ppFunc == GetProcAddress(GetModuleHandleA("d3d9.dll"), "Direct3DCreate9")) {
                    DWORD oldProtect;
                    VirtualProtect(ppFunc, sizeof(PROC), PAGE_READWRITE, &oldProtect);
                    originalDirect3DCreate9 = (IDirect3D9 * (WINAPI*)(UINT)) * ppFunc; // Сохраняем оригинал
                    *ppFunc = (PROC)HookedDirect3DCreate9; // Заменяем на свою функцию
                    VirtualProtect(ppFunc, sizeof(PROC), oldProtect, &oldProtect);
                    return true;
                }
            }
        }
    }
    return false;
}