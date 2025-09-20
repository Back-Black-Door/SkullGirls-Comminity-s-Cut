#pragma once
#include <Windows.h>
#include <memory>
#include <d3d9.h>

bool DLL_PROXY_LOAD();

extern WNDPROC OriginalWndProc;