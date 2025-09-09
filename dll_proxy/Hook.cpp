#pragma comment(lib, "libMinHook.x86.lib")

#include <Windows.h>
#include "MinHook.h"
#include <iostream>
#include "../thread/Thread.h"
#include "Hook.h"


typedef VOID(WINAPI* ExitProcess_t)(UINT);
ExitProcess_t fpExitProcess = NULL;

VOID WINAPI DetourExitProcess(UINT uExitCode)
{
    OutputDebugString("ExitProcess hooked!");
    g_workerThread.Stop();
    fpExitProcess(uExitCode);
}

BOOL InitializeHook()
{
    if (MH_Initialize() != MH_OK)
        return FALSE;

    if (MH_CreateHookApi(L"kernel32", "ExitProcess",
        &DetourExitProcess,
        reinterpret_cast<LPVOID*>(&fpExitProcess)) != MH_OK)
        return FALSE;

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
        return FALSE;
    OutputDebugString("Hook \"Exit Process\" init!");
    return TRUE;
}