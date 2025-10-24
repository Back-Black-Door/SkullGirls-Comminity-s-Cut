#include "dll_mods.h"
#include <string>
#include <map>
#include <Windows.h>
#pragma comment(lib, "Version.lib")
#include <iostream>

typedef void (*DrawUIFunc)(ImGuiContext* context);

void dll_mod::DLL_UI(ImGuiContext* context) {
		DrawUIFunc drawUI = (DrawUIFunc)GetProcAddress(hModule, "UI");
		if (!drawUI) {
			//std::cerr << "Failed to get functions from DLL!" << std::endl;
			return;
		}
		drawUI(context);
	}

dll_mod::dll_mod(const std::string& modPath) {
    ModInfo.modPath = modPath;
	hModule = LoadLibrary(modPath.c_str());
    GetDllInfo(modPath.c_str(), ModInfo);
}

dll_mod::~dll_mod() {
	FreeLibrary(hModule);
}

bool dll_mod::GetDllInfo(const char* dllPath, ModHeader& ModInfo) {
    DWORD handle;
    DWORD size = GetFileVersionInfoSize(dllPath, &handle);

    if (size > 0) {
        void* buffer = malloc(size);
        if (GetFileVersionInfo(dllPath, handle, size, buffer)) {
            char* value;
            UINT len;

            // ѕростой способ - используем первый доступный €зык
            if (VerQueryValue(buffer, "\\StringFileInfo\\040904b0\\FileDescription", (LPVOID*)&value, &len) && len > 0) {
                ModInfo.modDesc = value;
            }

            if (VerQueryValue(buffer, "\\StringFileInfo\\040904b0\\FileVersion", (LPVOID*)&value, &len) && len > 0) {
                ModInfo.modVersion = value;
            }

            if (VerQueryValue(buffer, "\\StringFileInfo\\040904b0\\ProductName", (LPVOID*)&value, &len) && len > 0) {
                ModInfo.modName = value;
            }

            if (VerQueryValue(buffer, "\\StringFileInfo\\040904b0\\Author", (LPVOID*)&value, &len) && len > 0) {
                ModInfo.modAuthor = value;
            }
        }
        free(buffer);
        return true;
    }
}
