#include "modslualib.h"
#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include <iostream>

Mod::Mod(lua_State* L) {
    std::cout << "[C] Start Reading ModInfo" << std::endl;
    lua_getglobal(L, "ModInfo"); // get foo on the stack
    LuaState = L;
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "ModName"); // get the field"foo_number", table is on top of the stack
        const char* ModNameC = lua_tostring(L, -1); // foo is on top of the stack, use -1
        std::cout << "[C] Mode Name: " << ModNameC << std::endl;
        ModInfo.modName = ModNameC;

        lua_getfield(L, -2, "Version");; // get foo on the stack
        int VersionC = lua_tonumber(L, -1); // foo is on top of the stack, use -1
        std::cout << "[C] Mode Version: " << VersionC << std::endl;
        ModInfo.modVerion = VersionC;

        lua_getfield(L, -3, "Author"); // get foo on the stack
        const char* AuthorC = lua_tostring(L, -1); // foo is on top of the stack, use -1
        std::cout << "[C] Mode Author: " << AuthorC << std::endl;
        ModInfo.modAuthor = AuthorC;

        lua_getfield(L, -4, "Path"); // get foo on the stack
        const char* PathC = lua_tostring(L, -1); // foo is on top of the stack, use -1
        std::cout << "[C] Mode Path: " << PathC << std::endl;
        ModInfo.modPath = PathC;
    }
    else {
        std::cout << "[C] There's apparently no table on the stack top\n";
        luaL_error(L, "Error: %s\n", lua_tostring(L, -1));
    }
}

const void Mod::Test() {
    std::cout << "TestTest" << std::endl;
}

const void Mod::init() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "init");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't init Func" << std::endl;
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
const void Mod::install() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "install");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't install Func" << std::endl;
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
const void Mod::loop() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "loop");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't loop Func" << std::endl;
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
const void Mod::deinit() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "deinit");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't deinit Func" << std::endl;
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
const void Mod::update() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "update");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't update Func" << std::endl;
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
const void Mod::launch() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "launch");

        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't launch Func" << std::endl;
            return;
        };

        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}

namespace nsCCLib {
      int my_add(lua_State* L) {
          int a = lua_tonumber(L, 1); // Первый аргумент
          int b = lua_tonumber(L, 2); // Второй аргумент
          int result = a + b;
          lua_pushnumber(L, result); // Возвращаем результат
          return 1; // Количество возвращаемых значений
      }
}
