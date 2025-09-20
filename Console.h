#pragma once
#include <string>
#include <Windows.h>

namespace Console {
    inline COORD console_home_coord{ 0, 20 };
    inline bool Show_Console;

    bool InitializeConsole();
    
    void DLL_WriteOutput(const char* format, int colorargs = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, ...);

    void DLL_DebugWriteOutput(const char* format, int colorargs = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, ...);

    void ClearScreen(COORD homeCoords = console_home_coord);

    void CleanupConsole();

    void HideConsole();

    void ShowConsole();
}