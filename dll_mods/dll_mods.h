#include <vector>
#include <Windows.h>
#include <filesystem>

#include "imgui/imgui.h"

class dll_mod {
public:
    struct ModHeader {
        std::string modName;
        std::string modVersion;
        std::string modAuthor;
        std::string modDesc;
        std::filesystem::path modPath;
    };

private:
    HMODULE hModule;

public:
    ~dll_mod();
    void DLL_UI(ImGuiContext* context);
    dll_mod(const std::string& modPath);
    ModHeader ModInfo;
    bool GetDllInfo(const char* dllPath, ModHeader& ModInfo);
};

inline std::vector<std::unique_ptr<dll_mod>> dll_mods;