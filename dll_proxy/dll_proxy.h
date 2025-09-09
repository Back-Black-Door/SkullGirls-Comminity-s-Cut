#pragma once
#include <Windows.h>
#include <memory>

extern FARPROC PROXY_FUNC_ADRESS[33]; //Need extern this for using in another .cpp file (See Thread.cpp)

bool DLL_PROXY_LOAD();
void DLL_PROXY_UNLOAD();