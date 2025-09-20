#include <functional>

#include "Thread.h"
#include "../Console.h"
#include "../process/Patching.h"
#include "../process/process.h"
#include "../modslualib.h"

bool ApplyGamePatches();
bool ExecuteModLaunch();
bool ExecuteModDeinit();
bool ExecuteModLoop();

DWORD IsolatedThread::Run() {
    Console::DLL_DebugWriteOutput("IsolatedThread started\n");
    
    while (!GetCurrentProcessInfo());
    Console::DLL_WriteOutput("Read process info!");

    while (!ApplyGamePatches());
    Console::DLL_WriteOutput("Apply games patch!");

    while (!ExecuteModLaunch());
    Console::DLL_WriteOutput("Execute Mod Launch completed successfully\n");

    while (m_running.load()) {
        // ѕровер€ем событие остановки каждые 10 мс
        if (WaitForSingleObject(m_hStopEvent, 10) == WAIT_OBJECT_0) {
            break;
        }
        //ExecuteModLoop();


        Sleep(1000);
    }
    while (!ExecuteModDeinit());
    Console::DLL_DebugWriteOutput("IsolatedThread stopped\n");
    return 0;
}

bool ApplyGamePatches()
{
    std::function<bool()> patches[] = {
        [=]() { return PachingUtils::ChangeDataDirectoryFirstTime(SGProccesInfo.SGpi.hThread, SGProccesInfo.SGpi.hProcess, SGProccesInfo.dwBaseAddress); },
        [=]() { return PachingUtils::SalValidathionBreaker(SGProccesInfo.SGpi.hThread, SGProccesInfo.SGpi.hProcess, SGProccesInfo.dwBaseAddress); },
        [=]() { return PachingUtils::GFSValidathionBreaker(SGProccesInfo.SGpi.hThread, SGProccesInfo.SGpi.hProcess, SGProccesInfo.dwBaseAddress); },
        [=]() { return PachingUtils::ChangeSal(SGProccesInfo.SGpi.hThread, SGProccesInfo.SGpi.hProcess, SGProccesInfo.dwBaseAddress); }
    };

    bool allSuccess = true;

    for (auto& patch : patches) {
        if (!patch()) {
            allSuccess = false;
        }
    }

    return allSuccess;
}

bool ExecuteModLaunch()
{

    Console::DLL_DebugWriteOutput(("ExecuteModLaunch " + std::to_string(mods.size()) + " mods\n").c_str());

    for (size_t i = 0; i < mods.size(); ++i) {
        //Console::DLL_DebugWriteOutput(("Processing mod " + std::to_string(i) + ": " +
        //    mods[i]->ModInfo.modName + "\n").c_str());

        try {
            mods[i]->launch();
        }
        catch (const std::exception& e) {
            Console::DLL_WriteOutput(("Error in launch: " + std::string(e.what()) + "\n").c_str(), FOREGROUND_RED);
        }
    }
    return true;
}

bool ExecuteModDeinit()
{

    for (size_t i = 0; i < mods.size(); ++i) {
        Console::DLL_DebugWriteOutput(("Processing mod " + std::to_string(i) + ": " +
            mods[i]->ModInfo.modName + "\n").c_str());

        try {
            mods[i]->deinit();
        }
        catch (const std::exception& e) {
            Console::DLL_WriteOutput(("Error in deinit: " + std::string(e.what()) + "\n").c_str(), FOREGROUND_RED);
        }
    }

    return true;
}

bool ExecuteModLoop()
{

    for (size_t i = 0; i < mods.size(); ++i) {
        Console::DLL_DebugWriteOutput(("Processing mod " + std::to_string(i) + ": " +
            mods[i]->ModInfo.modName + "\n").c_str());

        try {
            mods[i]->loop();
        }
        catch (const std::exception& e) {
            Console::DLL_WriteOutput(("Error in deinit: " + std::string(e.what()) + "\n").c_str(), FOREGROUND_RED);
        }
    }

    return true;
}