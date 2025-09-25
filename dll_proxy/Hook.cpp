#include <Windows.h>
#include <iostream>
#include <d3d9.h>

#include "../thread/Thread.h"
#include "Hook.h"
#include "../Overlay/d3d9Wrapper.h"
#include "../dllmain.h"

void SendDebugString(const char* str);
void SendDebugStringW(const wchar_t* str);

// Указатель на оригинальную ExitProcess
void (WINAPI* OriginalExitProcess)(UINT uExitCode) = nullptr;
typedef void (WINAPI* OutputDebugStringA_t)(LPCSTR lpOutputString);

OutputDebugStringA_t OriginalOutputDebugStringA = NULL;

void WINAPI HookedExitProcess(UINT uExitCode) {
    std::cout << "ExitProcess hooked!" << "\n";
    ThreadPatchAndMods.Stop();
    OriginalExitProcess(uExitCode);
}



VOID WINAPI HookedOutputDebugStringA(LPCSTR lpOutputString) {
    // Перехватываем строку и отправляем в наше приложение
    if (lpOutputString) {
        std::cout << "[Skullgirls Debugger]" << lpOutputString << "\n";
    }

    // Вызываем оригинальную функцию
    if (OriginalOutputDebugStringA) {
        OriginalOutputDebugStringA(lpOutputString);
    }
}

BOOL InitializeHook()
{
    bool KernelHooked = false;
    bool d3d9Hooked = false;
    HMODULE hModule = GetModuleHandle(nullptr); // Получаем базовый адрес текущего модуля
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule +
        pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    // Ищем kernel32.dll в IAT
    for (; pImportDesc->Name; pImportDesc++) {
        const char* dllName = (const char*)((BYTE*)hModule + pImportDesc->Name);
        if (_stricmp(dllName, "kernel32.dll") == 0) {
            bool KernelHookedExitProcess = false;
            bool KernelHookedOutputDebugStringA = false;
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->FirstThunk);
            for (; pThunk->u1.Function; pThunk++) {
                PROC* ppFunc = (PROC*)&pThunk->u1.Function;
                if (*ppFunc == GetProcAddress(GetModuleHandleA("kernel32.dll"), "ExitProcess")) {
                    DWORD oldProtect;
                    VirtualProtect(ppFunc, sizeof(PROC), PAGE_READWRITE, &oldProtect);
                    OriginalExitProcess = (void (WINAPI*)(UINT)) * ppFunc; // Сохраняем оригинал
                    *ppFunc = (PROC)HookedExitProcess; // Заменяем на свою функцию
                    VirtualProtect(ppFunc, sizeof(PROC), oldProtect, &oldProtect);
                    KernelHookedExitProcess = 1;
                    break;
                }
                if (*ppFunc == GetProcAddress(GetModuleHandleA("kernel32.dll"), "OutputDebugStringA")) {
                    if (config::DEBUG_ON) {
                        DWORD oldProtect;
                        VirtualProtect(ppFunc, sizeof(PROC), PAGE_READWRITE, &oldProtect);
                        OriginalOutputDebugStringA = (void (WINAPI*)(LPCSTR)) * ppFunc; // Сохраняем оригинал
                        *ppFunc = (PROC)HookedOutputDebugStringA; // Заменяем на свою функцию
                        VirtualProtect(ppFunc, sizeof(PROC), oldProtect, &oldProtect);
                        KernelHookedOutputDebugStringA = 1;
                        break;
                    }
                    else {
                        KernelHookedOutputDebugStringA = 1;
                        break;
                    }
                }
                if (KernelHookedExitProcess and KernelHookedOutputDebugStringA) {
                    KernelHooked = 1;
                    break;
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
                    d3d9Hooked = 1;
                    break;
                }
                if (d3d9Hooked) break;
            }
        }
        if (KernelHooked and d3d9Hooked) return true;
    }
    return true;
}