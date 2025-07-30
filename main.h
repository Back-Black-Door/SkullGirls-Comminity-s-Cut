#pragma once
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

#define DATA01_DIR_PATH requiredDirs[0]
#define DATA02_DIR_PATH requiredDirs[1]
#define MODS_DIR_PATH requiredDirs[2]

COORD HomeCord{ 0,23 };
fs::path workDir;