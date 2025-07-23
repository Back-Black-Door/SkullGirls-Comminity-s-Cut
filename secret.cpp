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
#include <stdexcept>
#include <sstream>
#include <vector>
#include <functional>

#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include "modslualib.h"
namespace secret {
    using json = nlohmann::json;
    namespace fs = std::filesystem;

    // Константы и настройки
    const int CURRENT_CC_VERSION = 1;
    const std::string TARGET_EXE_NAME = "ORIGINALSkullGirls.exe";
    const std::string OUR_EXE_NAME = "SkullGirls.exe";
    const std::string SAVE_FILE_NAME = "saves_CC.json";

    // Структура для хранения информации о процессе
    struct ProcessInfo {
        HANDLE hProcess = nullptr;
        HANDLE hThread = nullptr;
        DWORD dwProcessId = 0;
        DWORD dwBaseAddress = 0;

        ~ProcessInfo() {
            if (hThread) CloseHandle(hThread);
            if (hProcess) CloseHandle(hProcess);
        }
    };

    // Функции для работы с консолью
    namespace ConsoleUtils {
        void ClearScreen(COORD homeCoords = { 0, 0 }) {
            HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hStdOut == INVALID_HANDLE_VALUE) return;

            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;

            DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
            DWORD count;

            FillConsoleOutputCharacter(hStdOut, ' ', cellCount, homeCoords, &count);
            FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count);
            SetConsoleCursorPosition(hStdOut, homeCoords);
        }

        void ShowBanner() {
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
        }
    }

    // Функции для работы с процессами
    namespace ProcessUtils {
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

        bool CreateNewPage(HANDLE processHandle) {
            PVOID remoteBuffer = VirtualAllocEx(
                processHandle,
                nullptr,
                1,
                MEM_COMMIT,
                PAGE_READWRITE
            );

            if (!remoteBuffer) {
                std::cerr << "VirtualAllocEx failed: " << GetLastError() << "\n";
                return false;
            }

            std::cout << "New Page Base Address: " << std::hex << remoteBuffer << "\n";
            return true;
        }
    }

    // Функции для модификации памяти
    namespace MemoryPatcher {
        bool PatchMemory(HANDLE processHandle, DWORD baseAddress, DWORD offset,
            const void* patchData, size_t patchSize,
            const std::string& description) {
            DWORD oldProtect;
            if (!VirtualProtectEx(processHandle, (LPVOID)(baseAddress + offset),
                patchSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                std::cerr << "VirtualProtectEx failed: " << GetLastError() << "\n";
                return false;
            }

            if (!WriteProcessMemory(processHandle, (LPVOID)(baseAddress + offset),
                patchData, patchSize, nullptr)) {
                std::cerr << "WriteProcessMemory failed: " << GetLastError() << "\n";
                return false;
            }

            VirtualProtectEx(processHandle, (LPVOID)(baseAddress + offset),
                patchSize, oldProtect, &oldProtect);

            std::cout << "Applied patch: " << description << "\n";
            return true;
        }

        bool NewsBreaker(HANDLE processHandle, DWORD baseAddress, bool& shouldPatch) {
            if (!shouldPatch) return true;

            unsigned char newString[] = "SG_is_Still_Alive";
            int newsAddress = 0;

            if (!ReadProcessMemory(processHandle, (LPVOID)(baseAddress + 0x00436E54),
                &newsAddress, sizeof(newsAddress), nullptr)) {
                return false;
            }

            if (newsAddress == 0) {
                shouldPatch = true;
                return false;
            }

            if (!WriteProcessMemory(processHandle, (LPVOID)(newsAddress),
                newString, sizeof(newString), nullptr)) {
                return false;
            }

            shouldPatch = false;
            return true;
        }

        // Аналогично для других функций патчинга...
    }

    // Функции для работы с файлами и директориями
    namespace FileSystemUtils {
        bool SetupWorkingDirectory(const fs::path& exePath) {
            try {
                fs::path workDir = exePath.parent_path();

                // Проверка необходимых директорий
                std::vector<fs::path> requiredDirs = {
                    workDir / "data01",
                    workDir / "data02",
                    workDir / "mods"
                };

                for (const auto& dir : requiredDirs) {
                    if (!fs::exists(dir)) {
                        if (!fs::create_directory(dir)) {
                            std::cerr << "Failed to create directory: " << dir << "\n";
                            return false;
                        }
                        std::cout << "Created directory: " << dir << "\n";
                    }
                }

                // Создание файла сохранений, если его нет
                fs::path saveFile = workDir / SAVE_FILE_NAME;
                if (!fs::exists(saveFile)) {
                    std::ofstream file(saveFile);
                    if (!file) {
                        std::cerr << "Failed to create save file\n";
                        return false;
                    }
                    file << "{}";
                    std::cout << "Created save file: " << saveFile << "\n";
                }

                return true;
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Filesystem error: " << e.what() << "\n";
                return false;
            }
        }

        bool CreateSymlinks(const fs::path& sourceDir, const fs::path& targetDir) {
            try {
                for (const auto& entry : fs::directory_iterator(sourceDir)) {
                    if (entry.path().extension() == ".gfs") {
                        auto target = targetDir / entry.path().filename();
                        if (!fs::exists(target)) {
                            if (fs::is_symlink(target)) {
                                std::cout << "Symlink already exists: " << target << '\n';
                            }
                            else {
                                fs::create_symlink(entry.path(), target);
                                std::cout << "Created symlink: " << target << '\n';
                            }
                        }
                    }
                }
                return true;
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Error creating symlinks: " << e.what() << '\n';
                return false;
            }
        }
    }

    // Функции для работы с модами
    namespace ModUtils {
        struct ModInfo {
            std::string modName;
            std::string modAuthor;
            std::string modPath;
            int modVersion = 0;
        };

        void LoadMods(const fs::path& modsPath, json& savedata,
            std::vector<Mod>& mods,
            std::vector<size_t>& launchMods,
            std::vector<size_t>& loopMods,
            std::vector<size_t>& deinitMods) {

            for (const auto& entry : fs::directory_iterator(modsPath)) {
                if (entry.path().extension() != ".lua") continue;

                lua_State* L = luaL_newstate();
                if (!L) {
                    std::cerr << "Failed to create Lua state for " << entry.path() << "\n";
                    continue;
                }

                try {
                    luaL_openlibs(L);
                    luaL_newlib(L, nsCCLib::ССlib);
                    lua_setglobal(L, "CCLib");

                    if (luaL_dofile(L, entry.path().string().c_str()) != LUA_OK) {
                        throw std::runtime_error(lua_tostring(L, -1));
                    }

                    Mod currentMod(L);
                    std::cout << "[C] Loaded mod: " << entry.path() << "\n";

                    // Обработка сохраненных данных
                    auto& modData = savedata[currentMod.ModInfo.modName];
                    if (!modData.is_null()) {
                        if (modData["Version"] < currentMod.ModInfo.modVerion) {
                            currentMod.update();
                            modData["Version"] = currentMod.ModInfo.modVerion;
                        }
                    }
                    else {
                        currentMod.install();
                        modData["Version"] = currentMod.ModInfo.modVerion;
                        modData["Author"] = currentMod.ModInfo.modAuthor;
                        modData["Path"] = currentMod.ModInfo.modPath;
                    }

                    currentMod.init();
                    size_t modIndex = mods.size();
                    mods.push_back(std::move(currentMod));

                    // Проверка функций мода
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
                    std::cerr << "Error loading mod " << entry.path() << ": " << e.what() << "\n";
                    lua_close(L);
                }
            }
        }
    }

    int main(int argc, char* argv[]) {
        ConsoleUtils::ShowBanner();
        std::cout << "Author: ImpDi\n";
        std::cout << "Version: " << CURRENT_CC_VERSION << "\n";

        // Проверка имени исполняемого файла
        fs::path exePath(argv[0]);
        if (!_stricmp(exePath.filename().string().c_str(), OUR_EXE_NAME.c_str())) {
            fs::path originalExe = exePath.parent_path() / TARGET_EXE_NAME;

            if (fs::exists(originalExe)) {
                fs::rename(originalExe, exePath.parent_path() / TARGET_EXE_NAME);
            }

            fs::rename(exePath, exePath.parent_path() / OUR_EXE_NAME);
            WinExec(OUR_EXE_NAME.c_str(), SW_SHOW);
            return 0;
        }

        // Настройка рабочей директории
        if (!FileSystemUtils::SetupWorkingDirectory(exePath)) {
            std::cerr << "Failed to setup working directory\n";
            return 1;
        }

        // Загрузка сохранений
        json savedata;
        try {
            std::ifstream saveFile(exePath.parent_path() / SAVE_FILE_NAME);
            if (!saveFile) throw std::runtime_error("Failed to open save file");
            savedata = json::parse(saveFile);
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading save file: " << e.what() << "\n";
            return 1;
        }

        // Загрузка модов
        std::vector<Mod> mods;
        std::vector<size_t> launchMods, loopMods, deinitMods;

        ModUtils::LoadMods(exePath.parent_path() / "mods", savedata, mods,
            launchMods, loopMods, deinitMods);

        // Сохранение данных модов
        try {
            std::ofstream saveFile(exePath.parent_path() / SAVE_FILE_NAME);
            if (!saveFile) throw std::runtime_error("Failed to open save file for writing");
            saveFile << savedata.dump(4);
        }
        catch (const std::exception& e) {
            std::cerr << "Error saving mod data: " << e.what() << "\n";
        }

        // Создание симлинков
        if (!FileSystemUtils::CreateSymlinks(exePath.parent_path() / "data01",
            exePath.parent_path() / "data02")) {
            std::cerr << "Failed to create symlinks\n";
            return 1;
        }

        // Запуск игры
        ProcessInfo gameProcess;
        {
            STARTUPINFO si = { sizeof(si) };
            std::string commandLine = TARGET_EXE_NAME + " " + (argc > 1 ? argv[1] : "");

            if (!CreateProcess(
                nullptr,
                const_cast<LPSTR>(commandLine.c_str()),
                nullptr,
                nullptr,
                FALSE,
                CREATE_SUSPENDED,
                nullptr,
                nullptr,
                &si,
                reinterpret_cast<PROCESS_INFORMATION*>(&gameProcess))) {
                std::cerr << "Failed to create process: " << GetLastError() << "\n";
                return 1;
            }

            gameProcess.dwBaseAddress = ProcessUtils::GetModuleBaseAddress(
                gameProcess.dwProcessId, TARGET_EXE_NAME);

            std::cout << "Game PID: " << std::hex << gameProcess.dwProcessId << "\n";
            std::cout << "Base Address: " << std::hex << gameProcess.dwBaseAddress << "\n";
        }

        // Применение патчей
        bool patches[] = { true, true, true, true }; // Пример флагов для патчей

        auto ApplyPatch = [&](auto patchFunc, bool& flag, const char* desc) {
            if (flag && !patchFunc(gameProcess.hThread, gameProcess.hProcess,
                gameProcess.dwBaseAddress, flag)) {
                std::cerr << "Failed to apply patch: " << desc << "\n";
            }
            };

        while (std::any_of(std::begin(patches), std::end(patches), [](bool b) { return b; })) {
            //ApplyPatch(MemoryPatcher::NewsBreaker, patches[0], "News Breaker");
            //ApplyPatch(MemoryPatcher::GFSValidathionBreaker, patches[1], "GFS Validation");
            // ... другие патчи
        }

        // Запуск модов
        for (size_t i : launchMods) {
            mods[i].launch();
        }

        // Основной цикл
        DWORD exitCode = STILL_ACTIVE;
        while (exitCode == STILL_ACTIVE) {
            GetExitCodeProcess(gameProcess.hProcess, &exitCode);

            for (size_t i : loopMods) {
                mods[i].loop();
            }

            Sleep(100); // Небольшая пауза, чтобы не нагружать CPU
        }

        // Деинициализация модов
        for (size_t i : deinitMods) {
            mods[i].deinit();
        }

        // Очистка
        for (auto& mod : mods) {
            lua_close(mod.LuaState);
        }

        std::cout << "Game exited. Goodbye!\n";
        return 0;
    }
};