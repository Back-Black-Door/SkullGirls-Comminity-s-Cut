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
            std::string errorMsg = lua_tostring(LuaState, -1);
            lua_pop(LuaState, 1);  // Чистим стек
            throw std::runtime_error("[LUA] " + errorMsg);
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
            std::string errorMsg = lua_tostring(LuaState, -1);
            lua_pop(LuaState, 1);  // Чистим стек
            throw std::runtime_error("[LUA] " + errorMsg);
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
            std::string errorMsg = lua_tostring(LuaState, -1);
            lua_pop(LuaState, 1);  // Чистим стек
            throw std::runtime_error("[LUA] " + errorMsg);
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
            std::string errorMsg = lua_tostring(LuaState, -1);
            lua_pop(LuaState, 1);  // Чистим стек
            throw std::runtime_error("[LUA] " + errorMsg);
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
            std::string errorMsg = lua_tostring(LuaState, -1);
            lua_pop(LuaState, 1);  // Чистим стек
            throw std::runtime_error("[LUA] " + errorMsg);
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
            std::string errorMsg = lua_tostring(LuaState, -1);
            lua_pop(LuaState, 1);  // Чистим стек
            throw std::runtime_error("[LUA] " + errorMsg);
        }
    }
}

namespace nsCCLib {
      int test(lua_State* L) {
          lua_pushnumber(L, 1); // Возвращаем результат
          return 1; // Количество возвращаемых значений
      }
      int GetGameBaseAdress(lua_State* L) {
          int result = SGProccesInfo.dwBaseAddress;
          lua_pushnumber(L, result); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int ReadAddressNum(lua_State* L) {

          if (lua_gettop(L) != 1) {
              return luaL_error(L, "Expected 1 arguments: address.");
          }

          // Проверяем типы аргументов
          if (!lua_isnumber(L, 1)) {
              return luaL_error(L, "Argument must be number.");
          }

          const uintptr_t address = static_cast<uintptr_t>(lua_tonumber(L, 1));

          int result;

          BOOL success = ReadProcessMemory(
              SGProccesInfo.SGpi.hProcess,
              reinterpret_cast<LPCVOID>(address),
              &result,
              sizeof(result),
              0
          );
          lua_pushnumber(L, result); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int ReadAddressStr(lua_State* L) {

          if (lua_gettop(L) < 2) {
              return luaL_error(L, "Expected 2 arguments: address and size");
          }

          // Проверяем типы аргументов
          if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
              return luaL_error(L, "Both arguments must be numbers.");
          }

          const uintptr_t address = static_cast<uintptr_t>(lua_tonumber(L, 1));
          const size_t size = static_cast<size_t>(lua_tonumber(L, 2));

          std::string result(size, '\0');

          ReadProcessMemory(
              SGProccesInfo.SGpi.hProcess,
              reinterpret_cast<LPCVOID>(address),
              result.data(),
              size,
              0
          );
          lua_pushlstring(L, result.data(), size); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int WriteAddressStr(lua_State* L) {

          if (lua_gettop(L) < 2) {
              return luaL_error(L, "Expected 2 arguments: address and string");
          }

          // Проверяем типы аргументов
          if (!lua_isnumber(L, 1)) {
              return luaL_error(L, "First argument must be number.");
          }
          if (!lua_isstring(L, 2)) {
              return luaL_error(L, "Second argument must be string.");
          }

          const uintptr_t address = static_cast<uintptr_t>(lua_tonumber(L, 1));
          const std::string str = static_cast<std::string>(lua_tostring(L, 2));

          BOOL Success = WriteProcessMemory(
              SGProccesInfo.SGpi.hProcess,
              reinterpret_cast<LPVOID>(address),
              str.data(),
              str.size(),
              0);
          lua_pushboolean(L, Success); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int WriteAddressNum(lua_State* L) {

          if (lua_gettop(L) < 2) {
              return luaL_error(L, "Expected 2 arguments: address and number");
          }

          // Проверяем типы аргументов
          if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
              return luaL_error(L, "Both arguments must be numbers.");
          }

          const uintptr_t address = static_cast<uintptr_t>(lua_tonumber(L, 1));
          const int num = static_cast<int>(lua_tonumber(L, 2));

          BOOL Success = WriteProcessMemory(
              SGProccesInfo.SGpi.hProcess,
              reinterpret_cast<LPVOID>(address),
              &num,
              sizeof(num),
              0);
          lua_pushboolean(L, Success); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int GetWorkingDirectory(lua_State* L) {
          std::string result = workDir.string();
          lua_pushstring(L, result.c_str()); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
}

void push_vars(lua_State* L, const nsCCLib::luaL_Var* vars) {
    for (; vars->name != NULL; vars++) {
        switch (vars->type) {
        case VAR_NUMBER:
            lua_pushnumber(L, vars->value.num);
            break;
        case VAR_INTEGER:
            lua_pushinteger(L, vars->value.integer);
            break;
        case VAR_STRING:
            lua_pushstring(L, vars->value.str);  // Осторожно: str должен быть валидным!
            break;
        case VAR_BOOLEAN:
            lua_pushboolean(L, vars->value.boolean);
            break;
        case VAR_NIL:
            lua_pushnil(L);
            break;
        case VAR_LIGHTUSERDATA:
            lua_pushlightuserdata(L, vars->value.lightud);
            break;
        default:
            luaL_error(L, "Unknown variable type: %d", vars->type);
        }
        lua_setfield(L, -2, vars->name);  // Добавляем в таблицу
    }
}