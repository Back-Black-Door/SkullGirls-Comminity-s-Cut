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

#include "formats/json.hpp"
using json = nlohmann::json;