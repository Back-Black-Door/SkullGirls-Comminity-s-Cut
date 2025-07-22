#include <Windows.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>
#include <array>
#include <cstdio>
#include "json.hpp"
#include <filesystem>
#include <fstream>

#pragma comment(lib, "lua54.lib")



#include <lua.hpp>
#include "modslualib.h"

//#define _DEBUG Use if wanna see some debug info.

using json = nlohmann::json;

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
    //Install, if our program not named SkullGirls.exe
    /*if (_tcscmp(argv[0], "-logtoconsole")*/
    std::filesystem::path CCPath{ argv[0] };
    if (!((CCPath.filename() == "SkullGirls.exe") or (CCPath.filename() == "Skullgirls.exe"))) {
        std::cout << "We are not \"SkullGirls.exe\" must fix that" << '\n';
        if (std::filesystem::exists(CCPath.parent_path() / "SkullGirls.exe")) {
            std::cout << "SkullGirls.exe is exist, try to rename" << '\n';
            std::filesystem::rename(CCPath.parent_path() / "SkullGirls.exe", CCPath.parent_path() / "ORIGINALSkullGirls.exe");
         }
        std::filesystem::rename(CCPath, CCPath.parent_path() / "SkullGirls.exe");
        WinExec("SkullGirls.exe",0);
        return 0;
    }

    //ShowWindow(GetConsoleWindow(), SW_HIDE); //uncomment, if don't wanna see console

    std::cout << R"(
  ____    _              _   _           _          _                                 
 / ___|  | | __  _   _  | | | |   __ _  (_)  _ __  | |  ___                           
 \___ \  | |/ / | | | | | | | |  / _` | | | | '__| | | / __|                          
  ___) | |   <  | |_| | | | | | | (_| | | | | |    | | \__ \                          
 |____/  |_|\_\  \__,_| |_| |_|  \__, | |_| |_|    |_| |___/                          
                                 |___/                                                
   ____                                                   _   _             _         
  / ___|   ___    _ __ ___    _ __ ___    _   _   _ __   (_) | |_   _   _  ( )  ___   
 | |      / _ \  | '_ ` _ \  | '_ ` _ \  | | | | | '_ \  | | | __| | | | | |/  / __|  
 | |___  | (_) | | | | | | | | | | | | | | |_| | | | | | | | | |_  | |_| |     \__ \  
  \____|  \___/  |_| |_| |_| |_| |_| |_|  \__,_| |_| |_| |_|  \__|  \__, |     |___/  
                                                                    |___/             
   ____           _                                                                   
  / ___|  _   _  | |_                                                                 
 | |     | | | | | __|                                                                
 | |___  | |_| | | |_                                                                 
  \____|  \__,_|  \__|                                                                
                              
)" << '\n';

    int CCVersion{ 0 }; //CC Version
    COORD HomeCord{ 0,23 };
    std::cout << "Author: ImpDi" << "\n";
    std::cout << "Version: " << CCVersion << "\n";
#ifdef _DEBUG
    std::cout << "CC_LunchName: " << argv[0] << '\n';
    HomeCord = { 0,24 };
#endif // DEBUG
    std::cout << '\n';
    bool DebugOn{ 0 }; // LaunchOptions
    bool ReinstallAll{ 0 };


    int count;
    std::string LunchName = "ORIGINALSkullGirls.exe ";
    if (argc != 1) {
        for (count = 1; count < argc; count++) {
            if (_tcscmp(argv[count], "-logtoconsole") == 0) {
                DebugOn = true;
                std::cout << "We are gonna debug SG" << "\n";
            }
            if (_tcscmp(argv[count], "-reinstall") == 0) {
                ReinstallAll = true;
                std::cout << "We are reinstall SG CC" << "\n";
                break;
            }
            LunchName += argv[count];
            LunchName += " ";
        }
    }

    std::cout << "Skullgirls_LunchName: " << LunchName << "\n";

    std::filesystem::path my_Work_DirectoryPath = argv[0];

    my_Work_DirectoryPath = my_Work_DirectoryPath.parent_path();

    std::filesystem::path data01FolderPath = my_Work_DirectoryPath / "data01";

    std::filesystem::path data02FolderPath = my_Work_DirectoryPath / "data02";

    std::filesystem::path modsFolderPath = my_Work_DirectoryPath / "mods";

    std::filesystem::path saveFilePath = my_Work_DirectoryPath / "saves_CC.json";

    if (!(std::filesystem::exists(data01FolderPath))) {
        std::cout << "We are NOT in SkullGirls directory?" << '\n';
        std::cout << "\"data01\" directory doesn't exist" << '\n';
        system("pause");
        return 1;
    }
    else
    {
        std::cout << "We are in SkullGirls directory\n";
        std::cout << "\"data01\" directory exist\n";
    }

    if (!(std::filesystem::exists(data02FolderPath))) {
        std::filesystem::create_directory(data02FolderPath);
        std::cout << "Create \"data02\" directory!" << '\n';
    }
    else {
        std::cout << "\"data02\" directory exist" << '\n';
    }

    if (!(std::filesystem::exists(modsFolderPath))) {
        std::filesystem::create_directory(modsFolderPath);
        std::cout << "Create \"mods\" directory!" << '\n';
    }
    else {
        std::cout << "\"mods\" directory exist" << '\n';
    }
    if (!(std::filesystem::exists(saveFilePath))) {
        std::ofstream saveFile(saveFilePath);
        if (saveFile.is_open()) {
            saveFile << "{}";
            std::cout << "Create \"saves_CC.json\" file!" << '\n';
            saveFile.close();
        }
        else {
            std::cout << "Can't create \"saves_CC.json\" file!" << '\n';
            system("pause");
            return 1;
        }
    }
    else {
        std::cout << "\"saves_CC.json\" exist" << '\n';
    }

    json savedatajson{};
    //Read saveFile
    std::ifstream saveFileRead(saveFilePath);
    if (saveFileRead.is_open()) {
        savedatajson = json::parse(saveFileRead);
        saveFileRead.close();
        std::cout << "SaveJson parsered." << '\n';
    }
    else {
        std::cout << "Can't open \"saves_CC.json\" file!" << '\n';
        system("pause");
        return 1;
    }

#ifdef _DEBUG    
    system("pause");
#endif // DEBUG
    ClearScreen(HomeCord);

    std::vector<Mod> vectorMod;
    std::vector<size_t> vectorModlaunch;
    std::vector<size_t> vectorModloop;
    std::vector<size_t> vectorModDeinit;



    for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator(modsFolderPath)) {
        
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);

        luaL_newlib(L, nsCCLib::ССlib);
        lua_setglobal(L, "CCLib");

        std::filesystem::path dir_entry_path = dir_entry;
        if (dir_entry_path.extension() == ".lua") {
            if (luaL_dofile(L, dir_entry_path.string().c_str()) == LUA_OK) {
                Mod CurrentMod(L);
                std::cout << "[C] Executed " << dir_entry_path<< "\n";
                if (!(savedatajson[CurrentMod.ModInfo.modName].is_null())) {
                    std::cout << "[C] " << CurrentMod.ModInfo.modName << " Already has in savedata" << "\n";
                    if (savedatajson[CurrentMod.ModInfo.modName]["Version"] < CurrentMod.ModInfo.modVerion) {
                        std::cout << "[C] " << CurrentMod.ModInfo.modName << " NOT in actual version, so - Update" << "\n";
                        CurrentMod.update();
                        savedatajson[CurrentMod.ModInfo.modName]["Version"] = CurrentMod.ModInfo.modVerion;
                        savedatajson[CurrentMod.ModInfo.modName]["Author"] = CurrentMod.ModInfo.modAuthor;
                        savedatajson[CurrentMod.ModInfo.modName]["Path"] = CurrentMod.ModInfo.modPath;
                    }
                    else {
                        std::cout << "[C] " << CurrentMod.ModInfo.modName << " Has actual version" << "\n";
                    }
                }
                else {
                    std::cout << "[C] We have a new mod. Try to save mod data in json" << "\n";
                    CurrentMod.install();
                    savedatajson[CurrentMod.ModInfo.modName]["Version"] = CurrentMod.ModInfo.modVerion;
                    savedatajson[CurrentMod.ModInfo.modName]["Author"] = CurrentMod.ModInfo.modAuthor;
                    savedatajson[CurrentMod.ModInfo.modName]["Path"] = CurrentMod.ModInfo.modPath;
                }
                CurrentMod.init();
                vectorMod.push_back(CurrentMod);
                
                    lua_getglobal(L, "Mod"); // get foo on the stack
                    lua_getfield(L, -1, "launch");

                    if (lua_isfunction(L, -1) != 1) {
                        std::cout << "[C] " << "We haven't Launch() func" << "\n";
                    }
                    else {
                        vectorModlaunch.push_back(vectorMod.size()-1);
                        
                    }
                    lua_getglobal(L, "Mod");
                    lua_getfield(L, -1, "loop");

                    if (lua_isfunction(L, -1) != 1) {
                        std::cout << "[C] " << "We haven't Loop()  func" << "\n";
                    }
                    else {
                        vectorModloop.push_back(vectorMod.size() - 1);
                    }
                    lua_getglobal(L, "Mod");
                    lua_getfield(L, -1, "deinit");

                    if (lua_isfunction(L, -1) != 1) {
                        std::cout << "[C] " << "We haven't Deinit()  func" << "\n";
                    }
                    else {
                        vectorModDeinit.push_back(vectorMod.size() - 1);
                    }
            }
            else {
                std::cout << "[C] Error reading script\n";
                luaL_error(L, "Error: %s\n", lua_tostring(L, -1));
            }
        } 
    }
    std::cout << "[C] Done Mod Init & Install & Update" << '\n';

    std::ofstream saveFileWrite(saveFilePath);
    saveFileWrite << savedatajson;
    saveFileWrite.close();

    std::cout << "[C] Rewrite \"saves_CC.json\"" << '\n';
    std::cout << "[C] Next messages probably in [C]" << '\n';


#ifdef _DEBUG    
    system("pause");
#endif // DEBUG

    ClearScreen(HomeCord);


    for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator(data01FolderPath)) {
        std::string i = dir_entry.path().filename().string();
        if (dir_entry.path().extension() == ".gfs") {
            if (!(std::filesystem::exists(data02FolderPath / i))) {
                std::cout << "We haven't " << data02FolderPath / i << '\n';
                if (std::filesystem::is_symlink(data02FolderPath / i)) {
                    std::cout << "Symlink already exist" << '\n';
                }
                else
                {
                    std::filesystem::create_symlink(data01FolderPath / i, data02FolderPath / i);
                    std::cout << "Symlink created" << '\n';
                }
            }
        }
    }


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



    bool dotgfs{ 1 };
    bool dotsal{ 1 };
    bool datafirst{ 1 };
    //bool datassecond{ 1 };
    bool salreplace{ 1 };

    while (dotgfs or dotsal or datafirst or salreplace) {
        //NewsBreaker(SGpi.hThread, SGpi.hProcess, BaseAdress, NEWS);
        GFSValidathionBreaker(SGpi.hThread, SGpi.hProcess, BaseAdress, dotgfs);
        SalValidathionBreaker(SGpi.hThread, SGpi.hProcess, BaseAdress, dotsal);
        ChangeDataDirectoryFirstTime(SGpi.hThread, SGpi.hProcess, BaseAdress, datafirst);
        ChangeSal(SGpi.hThread, SGpi.hProcess, BaseAdress, salreplace);
        //RussianLanguage(SGpi.hThread, SGpi.hProcess, BaseAdress, russianlang);
    }  

    for (size_t i : vectorModlaunch) {
        vectorMod[i].launch();
    }

    std::cout << "Done modification SG" << "\n";

    #ifdef NDEBUG
    if (vectorModloop.size() == 0)
    {
        std::cout << "Goodbye!" << "\n";
        return 0;
    }
    #endif


    //init something, thaw we did'n need to reinit them later
    DEBUG_EVENT DebugEv{};
    if (DebugOn == 1) {
        DebugActiveProcess(SGpi.dwProcessId);
        DebugSetProcessKillOnExit(false);
    }

    DWORD code{ 259 };

    while (code == 259) {
        GetExitCodeProcess(SGpi.hProcess, &code);
        for (size_t i : vectorModloop) {
            vectorMod[i].loop();
        }
        //Debugger(DebugEv, SGpi.hThread, SGpi.hProcess, DebugOn);
        //For infinity function 
        //Sleep(500);
    }

    std::cout << "Skullgirls Wanna Exit" << "\n";
    for (size_t i : vectorModDeinit) {
        vectorMod[i].deinit();
    }
    CloseHandle(SGpi.hProcess);
    CloseHandle(SGpi.hThread);
    for (Mod i : vectorMod) {
        lua_close(i.LuaState);
    }

    std::cout << "Goodbye!" << "\n";
#ifdef _DEBUG    
    Sleep(5000);
#endif // DEBUG

    return 0;

}