#pragma once
#include <string>

//#define _DEBUG Use if wanna see some debug info.

extern const int CURRENT_CC_VERSION = 1;
extern const std::string TARGET_EXE_NAME = "ORIGINALSkullGirls.exe";
extern const std::string OUR_EXE_NAME = "SkullGirls.exe";
extern const std::string SAVE_FILE_NAME = "saves_CC.json";
extern const char SAL_FILE_NAME[] = "FULL_SGCC.sal";
extern const std::string STEAM_NAME = "steam.exe";
extern const int SKULLGIRLS_STEAM_ID = 245170;

extern bool DEBUG_ON{ 0 }; // LaunchOptions
extern bool REINSTALL_ALL{ 0 };

//For requiredDirs
#define DATA01_DIR_PATH requiredDirs[0]
#define DATA02_DIR_PATH requiredDirs[1]
#define MODS_DIR_PATH requiredDirs[2]