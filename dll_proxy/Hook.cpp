#include <Windows.h>
#include <iostream>
#include <d3d9.h>
#include <cstring>

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

// Слот ищем по имени из таблицы имён импорта, а не по лежащему в нём адресу:
// адрес мог уже подменить другой мод, и тогда сравнение не совпадёт.
static PROC* FindImportSlot(BYTE* base, PIMAGE_IMPORT_DESCRIPTOR desc, const char* name)
{
    if (!desc->OriginalFirstThunk) return nullptr;
    PIMAGE_THUNK_DATA nameThunk = (PIMAGE_THUNK_DATA)(base + desc->OriginalFirstThunk);
    PIMAGE_THUNK_DATA addrThunk = (PIMAGE_THUNK_DATA)(base + desc->FirstThunk);
    for (; nameThunk->u1.AddressOfData; nameThunk++, addrThunk++) {
        if (nameThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
        PIMAGE_IMPORT_BY_NAME imported = (PIMAGE_IMPORT_BY_NAME)(base + nameThunk->u1.AddressOfData);
        if (strcmp((const char*)imported->Name, name) == 0) return (PROC*)&addrThunk->u1.Function;
    }
    return nullptr;
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
            PROC* ppExit = FindImportSlot((BYTE*)hModule, pImportDesc, "ExitProcess");
            if (ppExit) {
                DWORD oldProtect;
                VirtualProtect(ppExit, sizeof(PROC), PAGE_READWRITE, &oldProtect);
                OriginalExitProcess = (void (WINAPI*)(UINT)) * ppExit; // Сохраняем оригинал
                *ppExit = (PROC)HookedExitProcess; // Заменяем на свою функцию
                VirtualProtect(ppExit, sizeof(PROC), oldProtect, &oldProtect);
                KernelHooked = 1;
            }

            // Перенаправляет отладочный вывод в консоль мода, включается -logtoconsole.
            PROC* ppDebug = config::DEBUG_ON
                ? FindImportSlot((BYTE*)hModule, pImportDesc, "OutputDebugStringA")
                : nullptr;
            if (ppDebug) {
                DWORD oldProtect;
                VirtualProtect(ppDebug, sizeof(PROC), PAGE_READWRITE, &oldProtect);
                OriginalOutputDebugStringA = (void (WINAPI*)(LPCSTR)) * ppDebug;
                *ppDebug = (PROC)HookedOutputDebugStringA;
                VirtualProtect(ppDebug, sizeof(PROC), oldProtect, &oldProtect);
            }
        }
        if (_stricmp(dllName, "d3d9.dll") == 0) {

            PROC* ppFunc = FindImportSlot((BYTE*)hModule, pImportDesc, "Direct3DCreate9");
            if (ppFunc) {
                DWORD oldProtect;
                VirtualProtect(ppFunc, sizeof(PROC), PAGE_READWRITE, &oldProtect);
                originalDirect3DCreate9 = (IDirect3D9 * (WINAPI*)(UINT)) * ppFunc; // Сохраняем оригинал
                *ppFunc = (PROC)HookedDirect3DCreate9; // Заменяем на свою функцию
                VirtualProtect(ppFunc, sizeof(PROC), oldProtect, &oldProtect);
                d3d9Hooked = 1;
            }
        }
        if (KernelHooked and d3d9Hooked) return true;
    }
    return true;
}