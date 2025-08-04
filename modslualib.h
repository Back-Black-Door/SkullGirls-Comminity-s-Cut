#pragma once
#include "include.h"
#include "main.h"

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

inline std::vector<Mod> mods;
inline std::vector<size_t> launchMods, loopMods, deinitMods;

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
			lua_Number num;      // LUA_TNUMBER
			lua_Integer integer; // LUA_TINTEGER (Lua 5.3+)
			const char* str;     // LUA_TSTRING
			int boolean;         // LUA_TBOOLEAN
			void* lightud;       // LUA_TLIGHTUSERDATA
		} value;
	} luaL_Var;

	extern const luaL_Var mylib_vars[];

	int test(lua_State* L);
	int GetGameBaseAdress(lua_State* L);
	int ReadAddressStr(lua_State* L);
	int ReadAddressNum(lua_State* L);
	int WriteAddressStr(lua_State* L);
	int WriteAddressNum(lua_State* L);
	int GetWorkingDirectory(lua_State* L);
	int GFS_addfiles(lua_State* L);
	extern const luaL_Reg ССlib[];

	inline const luaL_Var mylib_vars[] = {
		//{"test", VAR_STRING, {.str = "Test"}},
		{NULL, VAR_NIL, {0}}
	};

	inline const luaL_Reg ССlib[] = {
	 // {"test", test}, // Функция `test` в Lua будет вызывать `test` из C++
	  {"GetGameBaseAdress", GetGameBaseAdress},
	  {"ReadAddressStr", ReadAddressStr},
	  {"ReadAddressNum", ReadAddressNum},
	  {"WriteAddressStr", WriteAddressStr},
	  {"WriteAddressNum", WriteAddressNum},
	  {"GetWorkingDirectory", GetWorkingDirectory},
	  {"gfs_addfiles", GFS_addfiles},
	  {nullptr, nullptr} // Маркер конца
	};

}

void push_vars(lua_State* L, const nsCCLib::luaL_Var* vars);