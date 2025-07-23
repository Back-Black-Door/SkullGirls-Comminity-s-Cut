#include <Windows.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>
#include <array>
#include <cstdio>
#include <fstream>
#include <process.h>
#include <thread>
#include <functional>
#include <filesystem>
namespace fs = std::filesystem;

#pragma comment(lib, "lua54.lib")
#include <lua.hpp>

#include "json.hpp"
using json = nlohmann::json;

#include "config.h"
#include "modslualib.h"
#include "Patching.h"





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

namespace ProcessUtils {
    DWORD getppid()
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
    DWORD GetModuleBaseAddress(DWORD processId, const std::string& moduleName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to create module snapshot");
        }

        MODULEENTRY32 moduleEntry = { sizeof(moduleEntry) };
        DWORD baseAddress = 0;

        if (Module32First(hSnapshot, &moduleEntry)) {
            do {
                if (_stricmp(moduleEntry.szModule, moduleName.c_str()) == 0) {
                    baseAddress = (DWORD)moduleEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &moduleEntry));
        }

        CloseHandle(hSnapshot);
        return baseAddress;
    }
    BOOL PidNameTest(DWORD processId, const std::string& moduleName) {
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
};

int main(int argc, char* argv[]) {

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
                              
)" << std::endl;
    
    COORD HomeCord{ 0,23 };
    std::cout << "Author: ImpDi" << std::endl;
    std::cout << "Version: " << CURRENT_CC_VERSION << std::endl;
#ifdef _DEBUG
    std::cout << "CC_LunchName: " << argv[0] << std::endl;
    HomeCord = { 0,24 };
#endif // DEBUG
    std::cout << std::endl;
   
    //Install, if our program not named SkullGirls.exe
    fs::path exePath(argv[0]);
    if (!((exePath.filename().string() == OUR_EXE_NAME) or
        (exePath.filename().string() == "Skullgirls.exe"))) {
        fs::path originalExe = exePath.parent_path() / OUR_EXE_NAME;

        if (fs::exists(originalExe)) {
            fs::rename(originalExe, exePath.parent_path() / TARGET_EXE_NAME);
        }

        fs::rename(exePath, exePath.parent_path() / OUR_EXE_NAME);
        WinExec(OUR_EXE_NAME.c_str(), SW_SHOW);
        return 0;
    }

    std::string LunchName = TARGET_EXE_NAME + " ";
    std::string SteamLunchName = "start steam://run/" + std::to_string(SKULLGIRLS_STEAM_ID) + "//";
    if (argc != 1) {
        for (int count = 1; count < argc; count++) {
            if (_tcscmp(argv[count], "-logtoconsole") == 0) {
                DEBUG_ON = true;
                std::cout << "We are gonna debug SG" << std::endl;
            }
            //if (_tcscmp(argv[count], "-reinstall") == 0) {
            //    REINSTALL_ALL = true;
            //    std::cout << "We are reinstall SG_CC" << std::endl;
            //    break;
            //}
            LunchName += argv[count];
            LunchName += " ";
            SteamLunchName += argv[count];
            SteamLunchName += "%20";
        }
    }
#ifdef NDEBUG
    try
    {
        if (!ProcessUtils::PidNameTest(ProcessUtils::getppid(), STEAM_NAME)) {
            system(SteamLunchName.c_str());
            return 0;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        //return 1;
    }
#endif
    
    json savedata{NULL};
    fs::path workDir = exePath.parent_path();
    fs::path requiredDirs[] = {
        workDir / "data01",
        workDir / "data02",
        workDir / "mods",
    };
    
    try {
        // Проверка необходимых директорий
        for (const auto& dir : requiredDirs) {
            if (!fs::exists(dir)) {
                if (!fs::create_directory(dir)) {
                    throw std::runtime_error("Failed to create directory");
                }
                std::cout << "Created directory: " << dir << std::endl;
            }
        }
        
        // Создание файла сохранений, если его нет
        fs::path SAVE_FILE_PATH = workDir / SAVE_FILE_NAME;
        if (!fs::exists(SAVE_FILE_PATH)) {
            std::ofstream file(SAVE_FILE_PATH);
            if (!file) {
                throw std::runtime_error("Failed to create save file");
                return false;
            }
            file << "{}";
            std::cout << "Created save file: " << SAVE_FILE_PATH << std::endl;
            file.close();
        }
        
        std::ifstream saveFile(SAVE_FILE_PATH);
        if (!saveFile) throw std::runtime_error("Failed to open save file");
        savedata = json::parse(saveFile);

        fs::path SAL_FILE_PATH = workDir / "Salmon" / "FULL_SGCC.sal";
         // Создание файла .sal, если его нет
        if (!fs::exists(SAL_FILE_PATH)) {

            fs::copy(SAL_FILE_PATH.parent_path() / "FULL_MODE.sal", SAL_FILE_PATH);
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }


#ifdef _DEBUG    
    system("pause");
#endif // DEBUG
    ClearScreen(HomeCord);

    std::vector<Mod> mods;
    std::vector<size_t> launchMods, loopMods, deinitMods;

    for (const auto& entry : fs::directory_iterator(MODS_DIR_PATH)) {
        if (entry.path().extension() != ".lua") continue;
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);

        if (!L) {
            std::cerr << "Failed to create Lua state for " << entry.path() << std::endl;
            continue;
        }

        try {
            luaL_newlib(L, nsCCLib::ССlib);
            lua_setglobal(L, "CCLib");

            if (luaL_dofile(L, entry.path().string().c_str()) != LUA_OK) {
                throw std::runtime_error(lua_tostring(L, -1));
            }
            Mod CurrentMod(L);
            std::cout << "[C] Loaded mod: " << entry.path() << std::endl;

            auto modData = savedata[CurrentMod.ModInfo.modName];

            if (!modData.is_null()) {
                if (modData["Version"] < CurrentMod.ModInfo.modVerion) {
                    CurrentMod.update();
                    modData["Version"] = CurrentMod.ModInfo.modVerion;
                }
            }
            else {
                CurrentMod.install();
                modData["Version"] = CurrentMod.ModInfo.modVerion;
                modData["Author"] = CurrentMod.ModInfo.modAuthor;
                modData["Path"] = CurrentMod.ModInfo.modPath;
            }

            CurrentMod.init();
            size_t modIndex = mods.size();
            mods.push_back(CurrentMod);
            auto CheckFunction = [&](const char* funcName, std::vector<size_t>& vec) {
                lua_getglobal(L, "Mod");
                lua_getfield(L, -1, funcName);
                if (lua_isfunction(L, -1)) {
                    vec.push_back(modIndex);
                }
                lua_pop(L, 2);
                };

            CheckFunction("launch", launchMods);
            CheckFunction("loop", loopMods);
            CheckFunction("deinit", deinitMods);
        }
        catch (const std::exception& e) {
            std::cerr << "[C] Error loading mod " << entry.path() << ": " << e.what() << std::endl;
            lua_close(L);
        }
    }
    std::cout << "[C] Done Mod Init & Install & Update" << std::endl;

    std::ofstream saveFileWrite(workDir / SAVE_FILE_NAME);
    saveFileWrite << savedata;
    saveFileWrite.close();

    std::cout << "[C] Rewrite \"saves_CC.json\"" << std::endl;
    std::cout << "[C] Next messages probably in [C]" << std::endl;


#ifdef _DEBUG    
    system("pause");
#endif // DEBUG
    ClearScreen(HomeCord);


    try {
        for (const auto& entry : fs::directory_iterator(DATA01_DIR_PATH)) {
            if (entry.path().extension() == ".gfs") {
                auto target = DATA02_DIR_PATH / entry.path().filename();
                if (!fs::exists(target)) {
                    if (fs::is_symlink(target)) {
                        std::cout << "Symlink already exists: " << target << std::endl;
                    }
                    else {
                        fs::create_symlink(entry.path(), target);
                        std::cout << "Created symlink: " << target << std::endl;
                    }
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error creating symlinks: " << e.what() << std::endl;
        return false;
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
        std::cerr << "Failed to create process: " << GetLastError() << std::endl;
        return 0;
    }
    std::cout << "We are lunch a game!" << std::endl;

    DWORD dwBaseAddress = GetModuleBaseAddress(SGpi.dwProcessId);
    std::cout << "Skullgirls_PID: " << std::hex << SGpi.dwProcessId << std::endl;
    std::cout << "Skullgirls_Base_Adress: " << std::hex << dwBaseAddress << std::endl;



    std::function<const bool()> functions[] = {
        [=]() { return PachingUtils::GFSValidathionBreaker(SGpi.hThread, SGpi.hProcess, dwBaseAddress); },
        [=]() { return PachingUtils::SalValidathionBreaker(SGpi.hThread, SGpi.hProcess, dwBaseAddress); },
        [=]() { return PachingUtils::ChangeDataDirectoryFirstTime(SGpi.hThread, SGpi.hProcess, dwBaseAddress); },
        [=]() { return PachingUtils::ChangeSal(SGpi.hThread, SGpi.hProcess, dwBaseAddress); } };

    std::vector<std::thread> threads;

    for (auto& func : functions) {
        threads.emplace_back([&func]() {
            while (!func());
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (size_t i : launchMods) {
        mods[i].launch();
    }

    std::cout << "Done modification SG" << std::endl;

    #ifdef NDEBUG
    if (loopMods.size() == 0 || deinitMods.size() == 0)
    {
        std::cout << "Goodbye!" << std::endl;
        return 0;
    }
    for (auto& mod : mods) {
        lua_close(mod.LuaState);
    }
    #endif


    //init something, thaw we did'n need to reinit them later
    DEBUG_EVENT DebugEv{};
    if (DEBUG_ON == 1) {
        DebugActiveProcess(SGpi.dwProcessId);
        DebugSetProcessKillOnExit(false);
    }


    DWORD exitCode = STILL_ACTIVE;
    while (exitCode == STILL_ACTIVE) {
        GetExitCodeProcess(SGpi.hProcess, &exitCode);

        for (size_t i : loopMods) {
            mods[i].loop();
        }

        Sleep(100); // Небольшая пауза, чтобы не нагружать CPU
    }

    std::cout << "Skullgirls Wanna Exit" << std::endl;
    for (size_t i : deinitMods) {
        mods[i].deinit();
    }

    CloseHandle(SGpi.hProcess);
    CloseHandle(SGpi.hThread);

    for (auto& mod : mods) {
        lua_close(mod.LuaState);
    }

    std::cout << "Goodbye!" << std::endl;
#ifdef _DEBUG    
    Sleep(5000);
#endif // DEBUG
    return 0;
}