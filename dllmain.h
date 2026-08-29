#pragma once
#include <Windows.h>
#include <string>
#include <filesystem>
#include "json.hpp"
#include "modslualib.h"

using json = nlohmann::json;

inline json loc_json;

// data02 наполняется ссылками на файлы data01. Символическая ссылка требует прав,
// жёсткая — нет, поэтому создаём жёсткую. Отличить любую из них от настоящей копии
// можно по числу ссылок на файл.
bool IsLinkedToOriginal(const std::filesystem::path& target);

namespace config {
	inline const int CURRENT_CC_VERSION = 1;
	inline const std::string EXE_NAME = "SkullGirls.exe";
	inline const std::string SAVE_FILE_NAME = "saves_CC.json";
	inline const char SAL_FILE_NAME[] = "FULL_SGCC.sal";
	inline const std::string STEAM_APP_ID = "245170";
	inline std::string SteamLunchName = "steam://run/245170/";
	inline std::string SteamLunchArgs; // Параметры игры, без argv[0].

	inline bool DEBUG_ON{ 0 }; // LaunchOptions
	inline bool REINSTALL_ALL{ 0 };
	inline bool LAUNCH_ORIGINAL_GAME{ 0 };
}

namespace main_paths {
	inline std::string exe_path;
	inline std::string work_dir_path;
	inline std::string data01_dir_path;
	inline std::string data02_dir_path;
	inline std::string mods_dir_path;
	inline std::string save_file_path;
	inline std::string sal_file_path;
	inline std::string loc_files_path;
};
bool HandleProcessAttach(HMODULE hModule);
bool HandleProcessDetach(HMODULE hModule);
bool ReadMainArgs();
bool InitializePaths(json& savedata);
void CreateRequiredDirectoriesAndFiles(json& savedata);
bool InitializeMods(json& savedata);
void ProcessModFile(const fs::directory_entry& entry, json& savedata);
void ManageModLifecycle(Mod& mod, json& savedata);
void SaveModData(const json& savedata);
bool CreateGFSLinks();
void AddLoc();