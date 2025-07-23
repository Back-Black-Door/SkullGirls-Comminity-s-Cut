#include "Patching.h"

extern const char SAL_FILE_NAME[14];

namespace PachingUtils {
    const PVOID CreateNewPage(const HANDLE& ThreadHande, const HANDLE& ProcessHandle) {

        PVOID remoteBuffer;
        remoteBuffer = VirtualAllocEx(
            ProcessHandle,    //Handle Process
            NULL,           //lpAddress
            1,              //sizeof shellcode, 
            MEM_COMMIT,     //flAllocationType
            PAGE_READWRITE  //flProtect
        );
        std::cout << "New Page Base Adress: " << std::hex << remoteBuffer << std::endl;
        return remoteBuffer;
    }

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
        std::cout << "Breaking .sal Validathion!" << std::endl;
        return true;
    }

    const bool ChangeDataDirectoryFirstTime(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {

        const unsigned char NewString[] = "/data02/";
        unsigned char OldString[sizeof NewString]{ NULL };


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

        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3e9000),
            0x4B000,
            PAGE_READONLY,
            &old
        );

        std::cout << "Change Data Directory First Time\n" ;
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

    const bool ChangeSal(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        char OldString[sizeof(SAL_FILE_NAME)]{NULL};


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3dbc7c), &OldString, sizeof(OldString), 0);
        if (OldString == 0) {
            return false;
        }

        DWORD old;
        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3db000),		// адрес региона для установки флага
            0xE678,		// размер региона
            PAGE_EXECUTE_READWRITE,	// флаг
            &old	// адрес для сохранения старых флагов
        );

        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3dbc7c), &SAL_FILE_NAME, sizeof SAL_FILE_NAME, 0);

        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3db000),		// адрес региона для установки флага
            0xE678,		// размер региона
            PAGE_READONLY,	// флаг
            &old	// адрес для сохранения старых флагов
        );

        std::cout << "Change .sal File Name\n";
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

    //void RussianLanguage(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& data) {
    //    if (data == 0) return;


    //    unsigned char FirstJMPOld{ NULL };

    //    ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1AB977), &FirstJMPOld, sizeof(FirstJMPOld), 0);
    //    if (FirstJMPOld == 0) {
    //        data = 1;
    //        return;
    //    }

    //    unsigned char FirstJMP = { 0x8E };
    //    std::array<char, 2>SecondJMP = { 0xEB,0x2A };
    //    std::array<char, 76>MYCODE =
    //    {
    //    0xE9, 0x19, 0x04, 0x00, 0x00, 0x74, 0xD4, 0x8B, 0x55, 0xBC, 0x8B, 0x45, 0xD0, 0x83, 0xF8, 0x10,
    //    0x8D , 0x4D , 0xBC , 0x0F , 0x43 , 0xCA , 0x83 , 0xFF , 0x03 , 0x75 , 0x31 , 0x0F , 0x57 , 0xC0 , 0xC7 , 0x45,
    //    0xE4, 0x02 , 0x00 , 0x00 , 0x00 , 0xB8 , 0x72 , 0x75 , 0x00 , 0x00 , 0xC7 , 0x45 , 0xE8 , 0x0F , 0x00 , 0x00,
    //    0x00 , 0x0F , 0x11 , 0x45 , 0xD4 , 0x66 , 0x89 , 0x45 , 0xD4 , 0x8D , 0x4D , 0xD4 , 0xC6 , 0x45 , 0xFC , 0x03,
    //    0xE9 , 0xD9 , 0x03 , 0x00 , 0x00 , 0x90 , 0x90 , 0x90 , 0x90 , 0x90 , 0x90 , 0x90
    //    };
    //    std::array<char, 5>Space1 = { 0xE9,0x3A,0x03,0x00,0x00 };
    //    std::array<char, 5>Space2 = { 0xE9,0x5B,0x02,0x00,0x00 };
    //    std::array<char, 5>Space3 = { 0xE9,0xDB,0x00,0x00,0x00 };
    //    std::array<char, 4>HHTPS = { 0x32,0x32,0x32,0x33 };

    //    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1AB8F2), &FirstJMP, sizeof(FirstJMP), 0); //FirstJMP
    //    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1AB950), &SecondJMP, sizeof(SecondJMP), 0); //SecondJMP
    //    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1AB977), &MYCODE, sizeof(MYCODE), 0); //MYCODE
    //    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1ABA56), &Space1, sizeof(Space1), 0); //Space1
    //    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1ABB35), &Space2, sizeof(Space2), 0); //Space2
    //    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1ABCB5), &Space3, sizeof(Space3), 0); //Space3
    //    std::cout << "Russian Language! " << std::endl;
    //    data = 0;
    //}
}