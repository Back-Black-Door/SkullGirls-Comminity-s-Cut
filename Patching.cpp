#include "Patching.h"
#include "config.h"

namespace PachingUtils {
    const DWORD getppid()
    {
        HANDLE hSnapshot;
        PROCESSENTRY32 pe32;
        DWORD ppid = 0, pid = GetCurrentProcessId();

        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        __try {
            if (hSnapshot == INVALID_HANDLE_VALUE) __leave;

            ZeroMemory(&pe32, sizeof(pe32));
            pe32.dwSize = sizeof(pe32);
            if (!Process32First(hSnapshot, &pe32)) __leave;

            do {
                if (pe32.th32ProcessID == pid) {
                    ppid = pe32.th32ParentProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));

        }
        __finally {
            if (hSnapshot != INVALID_HANDLE_VALUE) CloseHandle(hSnapshot);
        }
        return ppid;
    }
    const DWORD GetModuleBaseAddress(DWORD pID) {
        DWORD dwModuleBaseAddress = 0;
        while (dwModuleBaseAddress == 0) {
            HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
            MODULEENTRY32 ModuleEntry32 = { 0 };
            ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

            if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
            {
                do {
                    if (_tcscmp(ModuleEntry32.szModule, "ORIGINALSkullGirls.exe") == 0) // if Found Module matches Module we look for -> done!
                    {
                        dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                        break;
                    }
                } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


            }
        }
        //CloseHandle(hSnapshot);
        return dwModuleBaseAddress;
    }
    const BOOL PidNameTest(DWORD processId, const std::string& moduleName) {
        DWORD dwModuleBaseAddress = 0;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId); // make snapshot of all modules within process
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return false;
        }
        MODULEENTRY32 ModuleEntry32 = { 0 };
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
        {
            do {
                if ((_tcscmp(ModuleEntry32.szModule, moduleName.c_str()) == 0)) // if Found Module matches Module we look for -> done!
                {
                    CloseHandle(hSnapshot);
                    return true;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


        }
        CloseHandle(hSnapshot);
        return false;
    }

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
        std::cout << "Breaking .sal Validathion!\n" ;
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
        char OldString[sizeof(SAL_FILE_NAME)]{ NULL };


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

        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3dbc7c), &SAL_FILE_NAME, sizeof SAL_FILE_NAME, 0);

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