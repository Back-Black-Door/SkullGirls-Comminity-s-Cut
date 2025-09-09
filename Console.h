#pragma once
#include <string>
#include <Windows.h>

namespace Console {
    inline COORD console_home_coord{ 0, 20 };

    bool InitializeConsole();
    
    void RestoreConsoleInput();

    void DLL_WriteOutput(const char* format, int colorargs = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, ...);

    void DLL_DebugWriteOutput(const char* format, int colorargs = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, ...);

    void ClearScreen(COORD homeCoords = console_home_coord);

    void CleanupConsole();
}