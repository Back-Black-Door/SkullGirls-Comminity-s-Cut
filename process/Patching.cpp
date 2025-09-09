#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>
#include <iostream>
#include "../dllmain.h"
#include "Patching.h"

namespace PachingUtils {
    const bool NewsBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        const unsigned char NewNewsString[] = "SG_is_Still_Alive";
        int NewsAdress{};


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x00436E54), &NewsAdress, sizeof(NewsAdress), 0);
        if (NewsAdress == 0) {
            return false;
        }
        WriteProcessMemory(ProcessHandle, (LPVOID)(NewsAdress), (LPVOID)NewNewsString, sizeof NewNewsString, NULL);
        std::cout << "Breaking News!\n" << std::endl;
        return true;
    }
    const bool GFSValidathionBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {

        const unsigned char JMP = { 0xEB };
        unsigned char JMPValue{ NULL };

        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x0011BE67), &JMPValue, 1, 0);
        if (JMPValue == NULL) {
            return false;
        }
        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x0011BE67), &JMP, 1, NULL);
        std::cout << "Breaking .gfs Validathion!\n";
        return true;
    }
    const bool SalValidathionBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {

        const unsigned char JMP = { 0xEB };
        unsigned char JMPValue{ NULL };


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x5C0F4), &JMPValue, 1, 0);
        if (JMPValue == NULL) {
            return false;
        }
        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x5C0F4), &JMP, 1, NULL);
        std::cout << "Breaking .sal Validathion!\n";
        return true;
    }

    const bool ChangeDataDirectoryFirstTime(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        const unsigned char NewString[] = "/data02/";
        unsigned char OldString[sizeof NewString]{ NULL };

        if (ProcessHandle == NULL || ProcessHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Invalid process handle\n";
            return false;
        }

        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3e922c), &OldString, sizeof(OldString), 0);
        if (OldString == 0) {
            return false;
        }

        DWORD old;
        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3e9000),
            0x4B000,
            PAGE_EXECUTE_READWRITE,
            &old
        );

        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3e922c), &NewString, sizeof NewString, 0);

        std::cout << "Change Data Directory First Time\n";
        return true;
    }
    const bool ChangeDataDirectorySecondTime(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        const unsigned char NewString[] = "data02";
        unsigned char OldString[sizeof NewString]{ NULL };


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3efe20), &OldString, sizeof(OldString), 0);
        if (OldString == 0) {
            return false;
        }

        DWORD old;
        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3ef000),
            0x45000,
            PAGE_EXECUTE_READWRITE,
            &old
        );

        //WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3efe20), &NewString, sizeof NewString, 0);

        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3ef000),
            0x4B000,
            PAGE_READONLY,
            &old
        );
        std::cout << "Change Data Directory Second Time Time\n";
        std::cout << GetLastError() << std::endl;
        return true;
    }

    const void Debugger(DEBUG_EVENT& DebugEv, const HANDLE& ThreadHande, const HANDLE& ProcessHandle) {
        WaitForDebugEvent(&DebugEv, INFINITE);
        switch (DebugEv.dwDebugEventCode) {
        case OUTPUT_DEBUG_STRING_EVENT:

            BYTE* pBuffer = (BYTE*)malloc(DebugEv.u.DebugString.nDebugStringLength);
            SIZE_T bytesRead;

            ReadProcessMemory(ProcessHandle, DebugEv.u.DebugString.lpDebugStringData, pBuffer, DebugEv.u.DebugString.nDebugStringLength, &bytesRead);
            //ClearScreen(COORD{ 0,15 });
            std::cout << "Debug Output: " << pBuffer << std::endl;
            free(pBuffer);
            break;
        }
        ContinueDebugEvent(DebugEv.dwProcessId, DebugEv.dwThreadId, DBG_CONTINUE);
    }

    const bool ChangeSal(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        char OldString[sizeof(config::SAL_FILE_NAME)]{ NULL };


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3dbc7c), &OldString, sizeof(OldString), 0);
        if (OldString == 0) {
            return false;
        }

        DWORD old;
        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3db000),
            0xE678,
            PAGE_EXECUTE_READWRITE,
            &old
        );

        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3dbc7c), &config::SAL_FILE_NAME, sizeof config::SAL_FILE_NAME, 0);

        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3db000),
            0xE678,
            PAGE_READONLY,
            &old
        );

        std::cout << "Change .sal File Name\n";
        return true;
    }
}