#include <Windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "Overlay/Overlay.h"
#include "Console.h"
#include "Thread/ScopedLock.h"

static HANDLE hNullInput = INVALID_HANDLE_VALUE;
static bool console_allocated = false;
static CriticalSection g_consoleCs;  // Using our class instead of Windows's

namespace Console {
    bool InitializeConsole()
    {
        Show_Console = true;
        if (console_allocated) return true;

        if (!AllocConsole())
            return false;

        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode;
        if (GetConsoleMode(hInput, &mode)) {
            mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE | ENABLE_MOUSE_INPUT);
            mode |= ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT;
            SetConsoleMode(hInput, mode);
        }
        // Redirect streams
        FILE* fDummy;
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);

        HWND hwnd = GetConsoleWindow();
        if (hwnd) {
            // Delete mouse interaction in console
            LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
            style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
            SetWindowLongPtr(hwnd, GWL_STYLE, style);
        }

        console_allocated = true;
        return true;
    }

    static void WriteOutputInternal(const char* format, int colorargs, va_list args)
    {
        if (!console_allocated) return;

        ScopedLock lock(g_consoleCs);  // Using RAII

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE || hConsole == NULL)
            return;

        char buffer[2048];
        int len = vsnprintf(buffer, sizeof(buffer) - 2, format, args);
        if (len <= 0)
            return;

        buffer[len] = '\r';
        buffer[len + 1] = '\n';
        buffer[len + 2] = '\0';

        SetConsoleTextAttribute(hConsole, colorargs);

        DWORD bytesWritten;
        WriteConsoleA(hConsole, buffer, len + 2, &bytesWritten, NULL);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    void DLL_WriteOutput(const char* format, int colorargs, ...)
    {
        va_list args;
        va_start(args, colorargs);
        WriteOutputInternal(format, colorargs, args);
        va_end(args);
    }

    void DLL_DebugWriteOutput(const char* format, int colorargs, ...)
    {
        OutputDebugString(format);
#ifdef _DEBUG
        va_list args;
        va_start(args, colorargs);
        WriteOutputInternal(format, colorargs, args);
        va_end(args);
#endif
    }
    
    void ClearScreen(COORD homeCoords)
        {
            if (!console_allocated) return;

            ScopedLock lock(g_consoleCs);  // Using RAII

            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hConsole == INVALID_HANDLE_VALUE || hConsole == NULL)
                return;

            CONSOLE_SCREEN_BUFFER_INFO csbi;
            DWORD count;
            DWORD cells;
            COORD home = homeCoords;

            if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
                return;

            cells = csbi.dwSize.X * csbi.dwSize.Y;

            if (!FillConsoleOutputCharacterA(hConsole, ' ', cells, home, &count))
                return;

            if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cells, home, &count))
                return;

            SetConsoleCursorPosition(hConsole, home);
        }
  
    void CleanupConsole()
    {
        if (console_allocated)
        {
            {
                ScopedLock lock(g_consoleCs);

                HWND hConsole = GetConsoleWindow();
                if (hConsole != NULL)
                {
                    ShowWindow(hConsole, SW_HIDE);
                }
                FreeConsole();
                console_allocated = false;
            }
            //Critical Section automatically clears after g_consoleCs
        }
    }

    void HideConsole() {
        if (console_allocated)
        {
            ScopedLock lock(g_consoleCs);

            HWND hConsole = GetConsoleWindow();
            if (hConsole != NULL)
            {
                ShowWindow(hConsole, SW_HIDE);
            }                       
        }
    }

    void ShowConsole() {
        if (console_allocated)
        {
            ScopedLock lock(g_consoleCs);

            HWND hConsole = GetConsoleWindow();
            if (hConsole != NULL)
            {
                ShowWindow(hConsole, SW_SHOW);
            }
        }
    }

}