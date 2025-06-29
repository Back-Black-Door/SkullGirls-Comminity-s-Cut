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
        LPVOID(BaseAdress + 0x3e9000),		// 