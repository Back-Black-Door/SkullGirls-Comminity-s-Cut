#include <Windows.h>
#include <string>

#include "dll_proxy.h"

// ����� ��������� ��� ��������������� ���������� ���������
struct HandleDeleter {
    void operator()(HMODULE module) const {
        if (module) FreeLibrary(module);
    }
};

using ScopedModule = std::unique_ptr<std::remove_pointer<HMODULE>::type, HandleDeleter>;
ScopedModule hL;

FARPROC PROXY_FUNC_ADRESS[33] = { 0 };

bool DLL_PROXY_LOAD() {
    char system_Path[MAX_PATH] = { 0 };

    // �������� ���� � SysWOW64 ����������
    if (GetSystemWow64DirectoryA(system_Path, MAX_PATH) == 0) { //For 64-bit
        strcpy_s(system_Path, "C:\\Windows\\System32"); //For 32-bit
    }

    // ��������� ������ ���� � ����������
    std::string fullPath = system_Path;
    fullPath += "\\CoreMessaging.DLL";

    // ��������� ����������
    HMODULE tempHandle = LoadLibraryA(fullPath.c_str());
    if (!tempHandle) {
        return false;
    }

    hL.reset(tempHandle); // �������� �������� ������ ���������

    // ���������� �������� �������
    try {
        for (int i = 0; i < 33; i++) {
            PROXY_FUNC_ADRESS[i] = GetProcAddress(tempHandle, (LPCSTR)(i + 1)); // �������� ���������� � 1
            if (!PROXY_FUNC_ADRESS[i]) return FALSE;
        }
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

void DLL_PROXY_UNLOAD() {
    // ����� ��������� ������������� ������� FreeLibrary ��� ����������
    hL.reset();
}

// ������ ��� ��������� ������-�������
#define GENERATE_PROXY_FUNCTION(index) \
extern "C" __declspec(naked) void __stdcall ProxyFunction##index() \
{ \
    __asm { jmp [PROXY_FUNC_ADRESS + index * 4] } \
}

GENERATE_PROXY_FUNCTION(0)
GENERATE_PROXY_FUNCTION(1)
GENERATE_PROXY_FUNCTION(2)
GENERATE_PROXY_FUNCTION(3)
GENERATE_PROXY_FUNCTION(4)
GENERATE_PROXY_FUNCTION(5)
GENERATE_PROXY_FUNCTION(6)
GENERATE_PROXY_FUNCTION(7)
GENERATE_PROXY_FUNCTION(8)
GENERATE_PROXY_FUNCTION(9)
//We use ProxyFunction10 in Thread.cpp Check the .def file
GENERATE_PROXY_FUNCTION(10)
GENERATE_PROXY_FUNCTION(11)
GENERATE_PROXY_FUNCTION(12)
GENERATE_PROXY_FUNCTION(13)
GENERATE_PROXY_FUNCTION(14)
GENERATE_PROXY_FUNCTION(15)
GENERATE_PROXY_FUNCTION(16)
GENERATE_PROXY_FUNCTION(17)
GENERATE_PROXY_FUNCTION(18)
GENERATE_PROXY_FUNCTION(19)
GENERATE_PROXY_FUNCTION(20)
GENERATE_PROXY_FUNCTION(21)
GENERATE_PROXY_FUNCTION(22)
GENERATE_PROXY_FUNCTION(23)
GENERATE_PROXY_FUNCTION(24)
GENERATE_PROXY_FUNCTION(25)
GENERATE_PROXY_FUNCTION(26)
GENERATE_PROXY_FUNCTION(27)
GENERATE_PROXY_FUNCTION(28)
GENERATE_PROXY_FUNCTION(29)
GENERATE_PROXY_FUNCTION(30)
GENERATE_PROXY_FUNCTION(31)
GENERATE_PROXY_FUNCTION(32)

