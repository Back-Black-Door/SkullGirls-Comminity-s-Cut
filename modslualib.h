#pragma once
#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include <string>
#include <filesystem>

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

namespace nsCCLib {
	int my_add(lua_State* L);
	static const luaL_Reg ССlib[] = {
	  {"add", my_add}, // Функция `add` в Lua будет вызывать `my_add` из C++
	  {nullptr, nullptr} // Маркер конца
	};
}