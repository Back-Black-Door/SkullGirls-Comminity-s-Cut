#pragma once
#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include <string>
#include <filesystem>
#include "Patching.h"



class Mod {
	public:
	Mod(lua_State* L);
	struct ModHeader {
		std::string modName;
		int modVerion{ 0 };
		std::string modAuthor;
		std::filesystem::path modPath;
	};
	lua_State* LuaState;
	ModHeader ModInfo;
	const void Test();
	const void init();
	const void install();
	const void loop();
	const void deinit();
	const void update();
	const void launch();
};

typedef enum {
	VAR_NUMBER,
	VAR_INTEGER,
	VAR_STRING,
	VAR_BOOLEAN,
	VAR_NIL,
	VAR_LIGHTUSERDATA
} VarType;

namespace nsCCLib {
	typedef struct {
		const char* name;  // Имя переменной
		VarType type;      // Тип (из enum)
		union {
			lua_Number num;     // LUA_TNUMBER
			lua_Integer integer;// LUA_TINTEGER (Lua 5.3+)
			const char* str;     // LUA_TSTRING
			int boolean;         // LUA_TBOOLEAN
			void* lightud;       // LUA_TLIGHTUSERDATA
		} value;
	} luaL_Var;

	static const luaL_Var mylib_vars[] = {
	{NULL, VAR_NIL, {0}}
	};

	int test(lua_State* L);
	int GameBaseAdress(lua_State* L);
	int ReadAddressStr(lua_State* L);
	int ReadAddressNum(lua_State* L);
	int WriteAddressStr(lua_State* L);
	static const luaL_Reg ССlib[] = {
	  {"test", test}, // Функция `add` в Lua будет вызывать `my_add` из C++
	  {"GameBaseAdress", GameBaseAdress},
	  {"ReadAddressStr", ReadAddressStr},
	  {"ReadAddressNum", ReadAddressNum},
	  {"WriteAddressStr", WriteAddressStr},
	  {nullptr, nullptr} // Маркер конца
	};

}

void push_vars(lua_State* L, const nsCCLib::luaL_Var* vars);