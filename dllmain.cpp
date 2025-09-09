// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

#include "dllmain.h"
#include "dll_proxy/dll_proxy.h"
#include "dll_proxy/Hook.h"
#include "formats/gfs.h"
#include "Console.h"
#include "modslualib.h"
#include "process/process.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        return HandleProcessAttach(hModule);
    case DLL_PROCESS_DETACH:
        HandleProcessDetach(hModule);
        break;
    }
    return TRUE;
}

bool HandleProcessAttach(HMODULE hModule) {
    if (!DLL_PROXY_LOAD()) {
        OutputDebugString("We can't load DLL!");
    }
    if (!InitializeHook()) {
        OutputDebugString("We can't hook \"ExitProcess\"!");
    };
    if (!Console::InitializeConsole()) {
        OutputDebugString("We can't init Console!");
    };
    Console::DLL_WriteOutput(R"(
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
                              
)");
    Console::DLL_WriteOutput("\nAuthor: ImpDi\nVersion: DLL\n");
    if (!ReadMainArgs()) {
        OutputDebugString("We can't read args!");
    };
    if (config::LAUNCH_ORIGINAL_GAME) {
        Console::CleanupConsole();
        return TRUE;
    }
    if (!GetCurrentProcessInfo()) {
        OutputDebugString("We can't get current process info!");
    };
    json savedata;
    if (!InitializePaths(savedata)) {
        OutputDebugString("We can't init path!");
    };
    if (!InitializeMods(savedata)) {
        OutputDebugString("We can't init mods!");
    };
#ifdef _DEBUG
    system("pause");
#endif
    if (!CreateGFSLinks()) {
        OutputDebugString("We can't create symlink!");
    };
#ifdef _DEBUG
    system("pause");
#endif
#ifndef _DEBUG
    Console::CleanupConsole();
#endif // !_DEBUG
    return TRUE;
}

bool HandleProcessDetach(HMODULE hModule) {

    DLL_PROXY_UNLOAD();
    Console::DLL_DebugWriteOutput("DLL_PROXY_UNLOAD!\n", FOREGROUND_GREEN);

    SGProccesInfo.CloseHandles();
    Console::DLL_DebugWriteOutput("SGProccesInfo.CloseHandles()!\n", FOREGROUND_GREEN);

    mods.clear();
    Console::DLL_DebugWriteOutput("mods.clear()!\n", FOREGROUND_GREEN);
#ifdef _DEBUG
    Console::DLL_WriteOutput("Goodbye!\n", FOREGROUND_BLUE);
    system("pause");
#endif // DEBUG
    Console::CleanupConsole();
    return TRUE;
}


bool ReadMainArgs() {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv == NULL) {
        Console::DLL_DebugWriteOutput("CommandLineToArgvW failed!\n",FOREGROUND_RED);
        return FALSE; // или соответствующая обработка ошибки
    }

    // Преобразование из wide char (wchar_t) в multi-byte char
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, argv[0], -1, NULL, 0, NULL, NULL);
    if (bufferSize > 0) {
        std::vector<char> buffer(bufferSize);
        WideCharToMultiByte(CP_UTF8, 0, argv[0], -1, buffer.data(), bufferSize, NULL, NULL);
        main_paths::exe_path = std::string(buffer.data());
    }
    for (int i = 0; i < argc; i++) {
#ifdef _DEBUG
        wprintf(L"Argument %d: %ls\n", i, argv[i]);
#endif // DEBUG 
        if ((wcscmp(argv[i], L"-logtoconsole")) == 0) {
            config::DEBUG_ON = true;
        }
        if ((wcscmp(argv[i], L"-reinstall")) == 0) {
            config::REINSTALL_ALL = true;
        }
        if ((wcscmp(argv[i], L"-originalgame")) == 0) {
            config::LAUNCH_ORIGINAL_GAME = true;
        }
    }
    LocalFree(argv);
    return TRUE;
}

bool InitializePaths(json& savedata)
{
    try
    {
        main_paths::work_dir_path = fs::path(main_paths::exe_path).parent_path().string();
        main_paths::data01_dir_path = main_paths::work_dir_path + "\\data01";
        main_paths::data02_dir_path = main_paths::work_dir_path + "\\data02";
        main_paths::mods_dir_path = main_paths::work_dir_path + "\\mods";
        main_paths::save_file_path = main_paths::work_dir_path + "\\" + "\\" + config::SAVE_FILE_NAME;
        main_paths::sal_file_path = main_paths::work_dir_path + "\\Salmon\\FULL_SGCC.sal";
        if (config::REINSTALL_ALL) {
            Console::DLL_WriteOutput("We delete directories and files!\n", FOREGROUND_BLUE);
            fs::remove_all(main_paths::data02_dir_path);
            fs::remove(main_paths::save_file_path);
            fs::remove(main_paths::sal_file_path);
        }
        CreateRequiredDirectoriesAndFiles(savedata);
    }
    catch (const fs::filesystem_error& e)
    {
        std::string errorMessage = "Filesystem error: " + std::string(e.what()) + "\n";
        Console::DLL_DebugWriteOutput(errorMessage.c_str(), FOREGROUND_RED);
    }
    return true;
}

void CreateRequiredDirectoriesAndFiles(json& savedata)
{
        // Создание директорий
        for (const auto& dir : { main_paths::data02_dir_path, main_paths::mods_dir_path }) {
            if (!fs::exists(dir) && !fs::create_directory(dir)) {
                throw std::runtime_error("Failed to create directory: " + dir);
            }
        }

        // Создание файла сохранений
        if (!fs::exists(main_paths::save_file_path)) {
            std::ofstream file(main_paths::save_file_path);
            if (!file) throw std::runtime_error("Failed to create save file");
            file << "{}";
            file.close();
        }

        // Загрузка данных сохранений
        std::ifstream save_file_stream(main_paths::save_file_path);
        savedata = json::parse(save_file_stream);
        save_file_stream.close();

        // Копирование SAL файла
        if (!fs::exists(main_paths::sal_file_path)) {
            fs::copy(main_paths::work_dir_path + "\\Salmon\\FULL_MODE.sal",
                main_paths::sal_file_path);
        }
    }

bool InitializeMods(json& savedata)
{
    // Считаем количество .lua файлов
    try
        {
        size_t count = 0;
        for (const auto& entry : fs::directory_iterator(main_paths::mods_dir_path)) {
            if (entry.path().extension() == ".lua") count++;
        }

        // Резервируем память заранее
        mods.reserve(mods.size() + count);

        for (const auto& entry : fs::directory_iterator(main_paths::mods_dir_path)) {
            if (entry.path().extension() == ".lua") {

                ProcessModFile(entry, savedata);
            }
        }
        for (auto &mod : mods) {
            ManageModLifecycle(*mod, savedata);
        }
        SaveModData(savedata);
    }
    catch (const std::exception&)
    {
        return false;
    }
    return true;
}

// Обработка отдельного файла мода
void ProcessModFile(const fs::directory_entry& entry, json& savedata)
{
    try {
        // Создаем мод с собственным Lua состоянием
        mods.emplace_back(std::make_unique<Mod>(entry.path().string()));
    }
    catch (const std::exception& e) {
        std::string errorMessage = "Mod creation error: " + std::string(e.what()) + "\n";
        Console::DLL_WriteOutput(errorMessage.c_str(), FOREGROUND_RED);
    }
}

// Управление жизненным циклом мода
void ManageModLifecycle(Mod& mod, json& savedata)
{
    if (savedata[mod.ModInfo.modName].is_null()) {
        mod.install();
        savedata[mod.ModInfo.modName] = {
            {"Version", mod.ModInfo.modVersion},
            {"Author", mod.ModInfo.modAuthor},
            {"Path", mod.ModInfo.modPath}
        };
    }
    else if (savedata[mod.ModInfo.modName]["Version"] < mod.ModInfo.modVersion) {
        mod.update();
        savedata[mod.ModInfo.modName]["Version"] = mod.ModInfo.modVersion;
    }
    for (auto& i : GFS_CHANGES) {
        i.commit_changes();
    }
    GFS_CHANGES.clear();
    mod.init();
}

bool CreateGFSLinks()
{
    Console::DLL_WriteOutput("Checking .gfs files before starting the game\n", FOREGROUND_BLUE);

    try {
        for (const auto& entry : fs::directory_iterator(main_paths::data01_dir_path)) {
            if (entry.path().extension() == ".gfs") {
                fs::path data02_path = main_paths::data02_dir_path;
                auto target = data02_path / entry.path().filename();

                if (!fs::exists(target)) {
                    if (fs::is_symlink(target)) {
                        std::string Message = "Filesystem error creating GFS links: " + target.string();
                        Console::DLL_WriteOutput(Message.c_str());
                    }
                    else {
                        fs::create_symlink(entry.path(), target);
                        std::string Message = "Created symlink: " + target.string();
                        Console::DLL_WriteOutput(Message.c_str());
                    }
                }
            }
        }
        Console::DLL_WriteOutput("\nGFS links check completed!");
    }
    catch (const fs::filesystem_error& e) {
        std::string errorMessage = "Filesystem error creating GFS links: " + std::string(e.what()) + "\n";
        Console::DLL_WriteOutput(errorMessage.c_str(), FOREGROUND_RED);
        throw;
        return false;
    }
    return true;
}

void SaveModData(const json& savedata)
{
    try {
        std::ofstream saveFileWrite(main_paths::save_file_path);
        saveFileWrite << savedata.dump(4); // Форматированный вывод для читаемости
        saveFileWrite.close();
        Console::DLL_WriteOutput("[C] Rewrite \"saves_CC.json\"\n");
    }
    catch (const std::exception& e) {
        std::string errorMessage = "[C] Error saving mod data: " + std::string(e.what()) + "\n";
        Console::DLL_WriteOutput(errorMessage.c_str(), FOREGROUND_RED);
    }
}