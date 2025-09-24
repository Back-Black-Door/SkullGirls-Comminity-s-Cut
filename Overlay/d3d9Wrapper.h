#pragma once
#include <Windows.h>
#include <d3d9.h>

extern WNDPROC OriginalWndProc;

IDirect3D9* WINAPI HookedDirect3DCreate9(UINT SDKVersion);
inline decltype(Direct3DCreate9)* originalDirect3DCreate9 = nullptr;

bool D3D9_LOAD();