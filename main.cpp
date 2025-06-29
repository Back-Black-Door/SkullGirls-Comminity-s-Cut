#include <Windows.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>
#include <array>
#include "gfs.h"
#include <cstdio>

void ClearScreen(COORD homeCoords)
{
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
        hStdOut,
        (TCHAR)' ',
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
        hStdOut,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    )) return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}

DWORD GetModuleBaseAddress(DWORD pID) {
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

void CreateNewPage(HANDLE& ThreadHande, HANDLE& ProcessHandle) {

    PVOID remoteBuffer;
    remoteBuffer = VirtualAllocEx(
        ProcessHandle,    //Handle Process
        NULL,           //lpAddress
        1,              //sizeof shellcode, 
        MEM_COMMIT,     //flAllocationType
        PAGE_READWRITE  //flProtect
    );
    std::cout << "New Page Base Adress: " << std::hex << remoteBuffer << "\n";

}

void NewsBreaker(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& NEWS) {
    if (NEWS == 0) return;
    unsigned char NewNewsString[] = "SG_is_Still_Alive";
    int NewsAdress{};


    ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x00436E54), &NewsAdress, sizeof(NewsAdress), 0);
    if (NewsAdress == 0) {
        NEWS = 1;
        return;
    }
    WriteProcessMemory(ProcessHandle, (LPVOID)(NewsAdress), (LPVOID)NewNewsString, sizeof NewNewsString, NULL);
    std::cout << "Breaking News!" << "\n";
    NEWS = 0;
}
void GFSValidathionBreaker(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& dotgfs) {
    if (dotgfs == 0) return;

    unsigned char JMP = { 0xEB };
    unsigned char JMPValue{ NULL };

    ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x0011BE67), &JMPValue, 1, 0);
    if (JMPValue == NULL) {
        dotgfs = 1;
        return;
    }
    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x0011BE67), &JMP, 1, NULL);
    std::cout << "Breaking .gfs Validathion!" << "\n";
    dotgfs = 0;
}
void SalValidathionBreaker(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& dotsal) {
    if (dotsal == 0) return;

    unsigned char JMP = { 0xEB };
    unsigned char JMPValue{ NULL };


    ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x5C0F4), &JMPValue, 1, 0);
    if (JMPValue == NULL) {
        dotsal = 1;
        return;
    }
    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x5C0F4), &JMP, 1, NULL);
    std::cout << "Breaking .sal Validathion!" << "\n";
    dotsal = 0;
}

void ChangeDataDirectoryFirstTime(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& data) {
    if (data == 0) return;
    unsigned char NewString[] = "/data02/";
    unsigned char OldString[sizeof NewString]{ NULL };


    ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3e922c), &OldString, sizeof(OldString), 0);
    if (OldString == 0) {
        data = 1;
        return;
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

    std::cout << "Change Data Directory First Time" << "\n";
    data = 0;
}
void ChangeDataDirectorySecondTime(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& data) {
    if (data == 0) return;
    unsigned char NewString[] = "data02";
    unsigned char OldString[sizeof NewString]{ NULL };


    ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3efe20), &OldString, sizeof(OldString), 0);
    if (OldString == 0) {
        data = 1;
        return;
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
    std::cout << "Change Data Directory Second Time Time" << "\n";
    std::cout << GetLastError() << "\n";
    data = 0;
}

void ChangeSal(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& data) {
    if (data == 0) return;
    unsigned char NewString[] = "FULL_SGCC.sal";
    unsigned char OldString[sizeof NewString]{ NULL };


    ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3dbc7c), &OldString, sizeof(OldString), 0);
    if (OldString == 0) {
        data = 1;
        return;
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

    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3dbc7c), &NewString, sizeof NewString, 0);

    VirtualProtectEx
    (
        ProcessHandle,
        LPVOID(BaseAdress + 0x3db000),		// адрес региона для установки флага
        0xE678,		// размер региона
        PAGE_READONLY,	// флаг
        &old	// адрес для сохранения старых флагов
    );

    std::cout << "Change .sal File Name" << "\n";
    data = 0;
}


void Debugger(DEBUG_EVENT& DebugEv, HANDLE& ThreadHande, HANDLE& ProcessHandle, bool& DebugOn) {
    if (DebugOn == 0) {
        return;
    }
    WaitForDebugEvent(&DebugEv, INFINITE);
    switch (DebugEv.dwDebugEventCode) {
    case OUTPUT_DEBUG_STRING_EVENT:

        BYTE* pBuffer = (BYTE*)malloc(DebugEv.u.DebugString.nDebugStringLength);
        SIZE_T bytesRead;

        ReadProcessMemory(ProcessHandle, DebugEv.u.DebugString.lpDebugStringData, pBuffer, DebugEv.u.DebugString.nDebugStringLength, &bytesRead);
        //ClearScreen(COORD{ 0,15 });
        std::cout << "Debug Output: " << pBuffer << "\n";
        free(pBuffer);
        break;
    }
    ContinueDebugEvent(DebugEv.dwProcessId, DebugEv.dwThreadId, DBG_CONTINUE);
}

void RussianLanguage(HANDLE& ThreadHande, HANDLE& ProcessHandle, DWORD BaseAdress, bool& data) {
    if (data == 0) return;


    unsigned char FirstJMPOld{ NULL };

    ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1AB977), &FirstJMPOld, sizeof(FirstJMPOld), 0);
    if (FirstJMPOld == 0) {
        data = 1;
        return;
    }

    unsigned char FirstJMP = { 0x8E };
    std::array<char, 2>SecondJMP = { 0xEB,0x2A };
    std::array<char, 76>MYCODE =
    {
    0xE9, 0x19, 0x04, 0x00, 0x00, 0x74, 0xD4, 0x8B, 0x55, 0xBC, 0x8B, 0x45, 0xD0, 0x83, 0xF8, 0x10,
    0x8D , 0x4D , 0xBC , 0x0F , 0x43 , 0xCA , 0x83 , 0xFF , 0x03 , 0x75 , 0x31 , 0x0F , 0x57 , 0xC0 , 0xC7 , 0x45,
    0xE4, 0x02 , 0x00 , 0x00 , 0x00 , 0xB8 , 0x72 , 0x75 , 0x00 , 0x00 , 0xC7 , 0x45 , 0xE8 , 0x0F , 0x00 , 0x00,
    0x00 , 0x0F , 0x11 , 0x45 , 0xD4 , 0x66 , 0x89 , 0x45 , 0xD4 , 0x8D , 0x4D , 0xD4 , 0xC6 , 0x45 , 0xFC , 0x03,
    0xE9 , 0xD9 , 0x03 , 0x00 , 0x00 , 0x90 , 0x90 , 0x90 , 0x90 , 0x90 , 0x90 , 0x90
    };
    std::array<char, 5>Space1 = { 0xE9,0x3A,0x03,0x00,0x00 };
    std::array<char, 5>Space2 = { 0xE9,0x5B,0x02,0x00,0x00 };
    std::array<char, 5>Space3 = { 0xE9,0xDB,0x00,0x00,0x00 };
    std::array<char, 4>HHTPS = { 0x32,0x32,0x32,0x33 };

    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1AB8F2), &FirstJMP, sizeof(FirstJMP), 0); //FirstJMP
    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1AB950), &SecondJMP, sizeof(SecondJMP), 0); //SecondJMP
    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1AB977), &MYCODE, sizeof(MYCODE), 0); //MYCODE
    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1ABA56), &Space1, sizeof(Space1), 0); //Space1
    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1ABB35), &Space2, sizeof(Space2), 0); //Space2
    WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x1ABCB5), &Space3, sizeof(Space3), 0); //Space3
    std::cout << "Russian Language! " << "\n";
    data = 0;
}

int main(int argc, char* argv[]) {

    //ShowWindow(GetConsoleWindow(), SW_HIDE); //uncomment, if don't wanna see console

    bool DebugOn{ 0 };

    int count;

    std::string LunchName = "ORIGINALSkullGirls.exe ";
    if (argc != 1) {
        for (count = 1; count < argc; count++) {
            if (_tcscmp(argv[count], "-logtoconsole") == 0) {
                DebugOn = true;
                std::cout << "We are gonna debug SG" << "\n";
            }
            LunchName += argv[count];
            LunchName += " ";
        }
    }

    std::cout << "Skullgirls_LunchName: " << LunchName << "\n";


    //TODO: Install  

    //GFS Stuff Start

    std::filesystem::path my_Work_Directory = argv[0];

    my_Work_Directory = my_Work_Directory.parent_path();

    std::filesystem::path modsFolder = my_Work_Directory / "mods";

    std::filesystem::path data01Folder = my_Work_Directory / "data01";

    std::filesystem::path data02Folder = my_Work_Directory / "data02";

    std::cout << "My Mods Directory:" << modsFolder << '\n';
    if (!(std::filesystem::exists(data01Folder))) {
        std::cout << "We are in SkullGirls directory?" << '\n';
    }
    if (!(std::filesystem::exists(data02Folder))) {
        std::filesystem::create_directory(data02Folder);
        std::cout << "Create data02 directory!" << '\n';
    }

    for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator(data01Folder)) {

        std::filesystem::path currentfile = dir_entry;

        if (currentfile.extension() == ".gfs") { //Work only with gfs files
            std::cout << "Working with: " << data01Folder / currentfile.filename() << '\n';
            if (!(std::filesystem::exists(modsFolder / currentfile.filename().replace_extension("")))) { //We haven't mods
                if (!(std::filesystem::exists(data02Folder / currentfile.filename()))) {
                    std::cout << "There are no mods for this package. Making symlink..." << '\n';
                    std::filesystem::create_symlink(currentfile, data02Folder / currentfile.filename());
                }
                else {
                    std::cout << "There are no mods for this package. Symlink already exist" << '\n';
                }
            }
            else {  //We have mods in ModeFolder
                if (std::filesystem::exists(data02Folder / currentfile.filename())) {
                    std::cout << "We already have something in data02 Folder. Remove..." << '\n';
                    std::filesystem::remove(data02Folder / currentfile.filename());
                }
                GFS Data01Package(data01Folder / currentfile.filename());
                GFS ModePackage(modsFolder / currentfile.filename().replace_extension(""));

                ModePackage.header.print_Header();

                Data01Package.addGFStoGFS(ModePackage);
                Data01Package.write_GFS(data02Folder / currentfile.filename().replace_extension(".gfs"));
            }
        }

    }
    //GFS Stuff Ends

    STARTUPINFO SGsi;
    PROCESS_INFORMATION SGpi;

    ZeroMemory(&SGsi, sizeof(SGsi));
    SGsi.cb = sizeof(SGsi);
    ZeroMemory(&SGpi, sizeof(SGpi));

    if (!CreateProcess(
        NULL,    // No module name (use command line)
        const_cast<char*>(LunchName.c_str()),                // Command line
        NULL,                // Process handle not inheritable
        NULL,                // Thread handle not inheritable
        FALSE,               // Set handle inheritance to FALSE
        NULL,                // No creation flags
        NULL,                // Use parent's environment block
        NULL,                // Use parent's starting directory 
        &SGsi,               // Pointer to STARTUPINFO structure
        &SGpi)               // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 0;
    }
    std::cout << "We are lunch a game!" << "\n";

    DWORD BaseAdress = GetModuleBaseAddress(SGpi.dwProcessId);
    std::cout << "Skullgirls_PID: " << std::hex << SGpi.dwProcessId << "\n";
    std::cout << "Skullgirls_Base_Adress: " << std::hex << BaseAdress << "\n";



    bool NEWS{ 1 };
    bool dotgfs{ 1 };
    bool dotsal{ 1 };
    bool datafirst{ 1 };
    bool datassecond{ 1 };
    bool salreplace{ 1 };
    bool russianlang{ 1 };

    while (NEWS or dotgfs or dotsal or datafirst or salreplace) {
        NewsBreaker(SGpi.hThread, SGpi.hProcess, BaseAdress, NEWS);
        GFSValidathionBreaker(SGpi.hThread, SGpi.hProcess, BaseAdress, dotgfs);
        SalValidathionBreaker(SGpi.hThread, SGpi.hProcess, BaseAdress, dotsal);
        ChangeDataDirectoryFirstTime(SGpi.hThread, SGpi.hProcess, BaseAdress, datafirst);
        ChangeSal(SGpi.hThread, SGpi.hProcess, BaseAdress, salreplace);
        RussianLanguage(SGpi.hThread, SGpi.hProcess, BaseAdress, russianlang);
    }


    std::cout << "Done modification SG" << "\n";




    //init something, thaw we did'n need to reinit them later
    DEBUG_EVENT DebugEv;
    if (DebugOn == 1) {
        DebugActiveProcess(SGpi.dwProcessId);
        DebugSetProcessKillOnExit(false);
    }

    DWORD code{ 259 };

    while (code == 259) {
        GetExitCodeProcess(SGpi.hProcess, &code);

        //Debugger(DebugEv, SGpi.hThread, SGpi.hProcess, DebugOn);
        //For infinity function 
        Sleep(500);
    }

    std::cout << "Skullgirls Wanna Exit" << "\n";

    CloseHandle(SGpi.hProcess);
    CloseHandle(SGpi.hThread);

    std::cout << "Goodbye!" << "\n";
    //Sleep(10000);

    return 0;

}