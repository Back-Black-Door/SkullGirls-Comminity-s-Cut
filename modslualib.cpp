#include "modslualib.h"
#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include <iostream>
#include "gfs.h"

Mod::Mod(lua_State* L) {
    std::cout << "[C] Start Reading ModInfo" << '\n';
    lua_getglobal(L, "ModInfo"); // get foo on the stack
    LuaState = L;
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "ModName"); // get the field"foo_number", table is on top of the stack
        const char* ModNameC = lua_tostring(L, -1); // foo is on top of the stack, use -1
        std::cout << "[C] Mode Name: " << ModNameC << '\n';
        ModInfo.modName = ModNameC;

        lua_getfield(L, -2, "Version");; // get foo on the stack
        int VersionC = lua_tonumber(L, -1); // foo is on top of the stack, use -1
        std::cout << "[C] Mode Version: " << VersionC << '\n';
        ModInfo.modVerion = VersionC;

        lua_getfield(L, -3, "Author"); // get foo on the stack
        const char* AuthorC = lua_tostring(L, -1); // foo is on top of the stack, use -1
        std::cout << "[C] Mode Author: " << AuthorC << '\n';
        ModInfo.modAuthor = AuthorC;

        lua_getfield(L, -4, "Path"); // get foo on the stack
        const char* PathC = lua_tostring(L, -1); // foo is on top of the stack, use -1
        std::cout << "[C] Mode Path: " << PathC << '\n';
        ModInfo.modPath = PathC;
    }
    else {
        std::cout << "[C] There's apparently no table on the stack top\n";
        luaL_error(L, "Error: %s\n", lua_tostring(L, -1));
    }
}

void Mod::Test() {
    std::cout << "TestTest" << '\n';
}

void Mod::init() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "init");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't init Func" << "\n";
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
void Mod::install() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "install");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't install Func" << "\n";
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
void Mod::loop() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "loop");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't loop Func" << "\n";
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
void Mod::deinit() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "deinit");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't deinit Func" << "\n";
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
void Mod::update() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "update");
        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't update Func" << "\n";
            return;
        };
        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}
void Mod::launch() {
    lua_getglobal(LuaState, "Mod"); // get foo on the stack
    if (lua_istable(LuaState, -1)) {
        lua_getfield(LuaState, -1, "launch");

        if (lua_isfunction(LuaState, -1) != 1) {
            std::cout << "[C] " << "Haven't launch Func" << "\n";
            return;
        };

        if (lua_pcall(LuaState, 0, 0, 0) != 0) {
            luaL_error(LuaState, "Error: %s\n", lua_tostring(LuaState, -1));
        }
    }
}

namespace Cfunc {


      void loadCFunc(lua_State* L) {
            lua_pushcfunction(L, multiply_2_numbers);
            lua_setglobal(L, "multiply_2_numbers");
      }


      static int multiply_2_numbers(lua_State* L) {
            // function args are on the lua stack, last arg is on top
            lua_Number arg_2 = lua_tonumber(L, -1);
            lua_Number arg_1 = lua_tonumber(L, -2);

            std::cout << "[C] executing multiply_2_numbers with two arguments: "
                << arg_1 << " and " << arg_2 << '\n';
            // calculate the result
            lua_Number result = arg_2 * arg_1;
            // push the result back to the stack
            lua_pushnumber(L, result);
            // return 1 because we have 1 return value (i know magic number ...)
            return 1;
      }
}
