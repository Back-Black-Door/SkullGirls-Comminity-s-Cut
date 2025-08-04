#include "main.h"
#include "config.h"
#include "modslualib.h"
#include "Patching.h"

using namespace main_paths;

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

bool Start_Skullgirls(std::string LaunchName) {
    ZeroMemory(&SGProccesInfo.SGsi, sizeof(SGProccesInfo.SGsi));
    SGProccesInfo.SGsi.cb = sizeof(SGProccesInfo.SGsi);
    ZeroMemory(&SGProccesInfo.SGpi, sizeof(SGProccesInfo.SGpi));

    if (!CreateProcess(
        NULL,    // No module name (use command line)
        const_cast<char*>(LaunchName.c_str()),                // Command line
        NULL,                // Process handle not inheritable
        NULL,                // Thread handle not inheritable
        FALSE,               // Set handle inheritance to FALSE
        NULL,                // No creation flags
        NULL,                // Use parent's environment block
        NULL,                // Use parent's starting directory 
        &SGProccesInfo.SGsi,               // Pointer to STARTUPINFO structure
        &SGProccesInfo.SGpi)               // Pointer to PROCESS_INFORMATION structure
        )
    {
        std::cerr << "Failed to create process: " << GetLastError() << std::endl;
        return 1;
    }
    std::cout << "We lunched the game!" << std::endl;
    return 0;
}

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
    
    std::cout << "Author: ImpDi" << std::endl;
    std::cout << "Version: " << CURRENT_CC_VERSION << std::endl;
    COORD console_home_coord{ 0, 23 };
#ifdef _DEBUG
    std::cout << "LunchName: " << argv[0] << std::endl;
    console_home_coord = { 0,24 };
#endif // DEBUG
    std::cout << std::endl;
   
    //Install, if our program not named SkullGirls.exe
    exe_path = argv[0];
    if (!((exe_path.filename().string() == OUR_EXE_NAME) or
        (exe_path.filename().string() == "Skullgirls.exe"))) {
        original_exe_path = exe_path.parent_path() / OUR_EXE_NAME;

        if (fs::exists(original_exe_path)) {
            fs::rename(original_exe_path, original_exe_path.parent_path() / TARGET_EXE_NAME);
        }

        fs::rename(original_exe_path, original_exe_path.parent_path() / OUR_EXE_NAME);
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
              if (_tcscmp(argv[count], "-originalgame") == 0) {
                LAUNCH_ORIGINAL_GAME = true;
                std::cout << "We are lauch original SG" << std::endl;
            }
            LunchName += argv[count];
            LunchName += " ";
            SteamLunchName += argv[count];
            SteamLunchName += "%20";
        }
    }
#ifdef NDEBUG
    try
    {
        if (!PachingUtils::PidNameTest(PachingUtils::getppid(), STEAM_NAME)) {
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
    if (LAUNCH_ORIGINAL_GAME) {
        Start_Skullgirls(LunchName);
        return 0;
    }
    json savedata{NULL};
    work_dir_path = exe_path.parent_path();
    data01_dir_path = work_dir_path / "data01";
    data02_dir_path = work_dir_path / "data02";
    mods_dir_path = work_dir_path / "mods";
   
    fs::path requiredDirs[] = {
        data01_dir_path,
        data02_dir_path,
        mods_dir_path,
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
        save_file_path = work_dir_path / SAVE_FILE_NAME;
        if (!fs::exists(save_file_path)) {
            std::ofstream file(save_file_path);
            if (!file) {
                throw std::runtime_error("Failed to create save file");
                return false;
            }
            file << "{}";
            std::cout << "Created save file: " << save_file_path << std::endl;
            file.close();
        }
        
        std::ifstream save_file_stream(save_file_path);
        if (!save_file_stream) throw std::runtime_error("Failed to open save file");
        savedata = json::parse(save_file_stream);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }


#ifdef _DEBUG    
    system("pause");
#endif // DEBUG
    ClearScreen(console_home_coord);

    for (const auto& entry : fs::directory_iterator(mods_dir_path)) {
        if (entry.path().extension() != ".lua") continue;
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);

        if (!L) {
            std::cerr << "Failed to create Lua state for " << entry.path() << std::endl;
            continue;
        }

        try {
            luaL_newlib(L, nsCCLib::ССlib);
            push_vars(L, nsCCLib::mylib_vars);
            lua_setglobal(L, "CCLib");

            if (luaL_loadfile(L, entry.path().string().c_str()) != LUA_OK || lua_pcall(L, 0, 0, 0) != LUA_OK) {
                std::string errorMsg = lua_tostring(L, -1);
                lua_pop(L, 1);  // Чистим стек
                throw std::runtime_error("[LUA] " + errorMsg);
            }
            Mod CurrentMod(L);
            std::cout << "[C] Opened mod: " << entry.path() << std::endl;

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
            std::cerr << "[C] Error loading mod: \n" << e.what() << std::endl;
            lua_close(L);
        }
    }
    std::cout << "[C] Done Mod Init & Install & Update" << std::endl;

    std::ofstream saveFileWrite(save_file_path);
    saveFileWrite << savedata;
    saveFileWrite.close();

    std::cout << "[C] Rewrite \"saves_CC.json\"" << std::endl;
    std::cout << "[C] Next messages probably in [C]" << std::endl;


#ifdef _DEBUG    
    system("pause");
#endif // DEBUG
    ClearScreen(console_home_coord);

    try {
        for (const auto& entry : fs::directory_iterator(data01_dir_path)) {
            if (entry.path().extension() == ".gfs") {
                auto target = data02_dir_path / entry.path().filename();
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

    Start_Skullgirls(LunchName);

    SGProccesInfo.dwBaseAddress = PachingUtils::GetModuleBaseAddress(SGProccesInfo.SGpi.dwProcessId);
    std::cout << "Skullgirls_PID: " << std::hex << SGProccesInfo.SGpi.dwProcessId << std::endl;
    std::cout << "Skullgirls_Base_Adress: " << std::hex << SGProccesInfo.dwBaseAddress << std::endl;



    std::function<const bool()> functions[] = {
        [=]() { return PachingUtils::GFSValidathionBreaker(SGProccesInfo.SGpi.hThread, SGProccesInfo.SGpi.hProcess, SGProccesInfo.dwBaseAddress); },
        [=]() { return PachingUtils::SalValidathionBreaker(SGProccesInfo.SGpi.hThread, SGProccesInfo.SGpi.hProcess, SGProccesInfo.dwBaseAddress); },
        [=]() { return PachingUtils::ChangeDataDirectoryFirstTime(SGProccesInfo.SGpi.hThread, SGProccesInfo.SGpi.hProcess, SGProccesInfo.dwBaseAddress); } };

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
        try {
            mods[i].launch();
        }
        catch (const std::exception& e) {
            std::cerr << "[C] Error loading mod: \n" << e.what() << std::endl;
            lua_close(mods[i].LuaState);
        }
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
        DebugActiveProcess(SGProccesInfo.SGpi.dwProcessId);
        DebugSetProcessKillOnExit(false);

        for (Mod i : mods) {
            std::cout << i.ModInfo.modName << std::endl;
            std::cout << i.ModInfo.modVerion << std::endl;
            std::cout << i.ModInfo.modAuthor << std::endl;
            std::cout << i.ModInfo.modPath << std::endl;
        }
    }


    DWORD exitCode = STILL_ACTIVE;
    while (exitCode == STILL_ACTIVE) {
        GetExitCodeProcess(SGProccesInfo.SGpi.hProcess, &exitCode);

        for (size_t i : loopMods) {
            try {
                mods[i].loop();
            }
            catch (const std::exception& e) {
                std::cerr << "[C] Error loading mod: \n" << e.what() << std::endl;
                lua_close(mods[i].LuaState);
            }
        }

        Sleep(100); // Небольшая пауза, чтобы не нагружать CPU
    }

    std::cout << "Skullgirls Wanna Exit" << std::endl;
    for (size_t i : deinitMods) {
        try {
            mods[i].deinit();
        }
        catch (const std::exception& e) {
            std::cerr << "[C] Error loading mod: \n" << e.what() << std::endl;
            lua_close(mods[i].LuaState);
        }
    }

    for (auto& mod : mods) {
        lua_close(mod.LuaState);
    }

    std::cout << "Goodbye!" << std::endl;
#ifdef _DEBUG    
    Sleep(5000);
#endif // DEBUG
    return 0;
}