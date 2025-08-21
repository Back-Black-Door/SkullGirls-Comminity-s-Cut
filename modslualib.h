#pragma once
#include "include.h"
#include "main.h"
#include "config.h"

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

namespace nsCCLib {

	typedef enum {
		VAR_NUMBER,
		VAR_INTEGER,
		VAR_STRING,
		VAR_BOOLEAN,
		VAR_NIL,
		VAR_LIGHTUSERDATA
	} VarType;

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

	int test(lua_State* L);
	int GetGameBaseAdress(lua_State* L);
	int ReadAddressStr(lua_State* L);
	int ReadAddressNum(lua_State* L);
	int WriteAddressStr(lua_State* L);
	int WriteAddressNum(lua_State* L);
	int GetWorkingDirectory(lua_State* L);
	int GFS_addfile(lua_State* L);
	int GFS_addfiles(lua_State* L);
	int GFS_extract_file(lua_State* L);
	int Add_New_Permission(lua_State* L);
	int GBS_merge(lua_State* L);
	extern const luaL_Reg ССlib[];

	inline const luaL_Reg ССlib[] = {
	 // {"test", test}, // Функция `test` в Lua будет вызывать `test` из C++
	  {"GetGameBaseAdress", GetGameBaseAdress},
	  {"ReadAddressStr", ReadAddressStr},
	  {"ReadAddressNum", ReadAddressNum},
	  {"WriteAddressStr", WriteAddressStr},
	  {"WriteAddressNum", WriteAddressNum},
	  {"GetWorkingDirectory", GetWorkingDirectory},
	  {"gfs_addfile", GFS_addfile},
	  {"gfs_addfiles", GFS_addfiles},
	  {"add_new_permission", Add_New_Permission},
	  {"gfs_extract_file", GFS_extract_file},
	  {"gbs_merge", GBS_merge},
	  {nullptr, nullptr} // Маркер конца
	};


void push_vars(lua_State* L, const nsCCLib::luaL_Var* vars);
}