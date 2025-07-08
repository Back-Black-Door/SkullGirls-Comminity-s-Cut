#pragma once
#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include <string>
#include <filesystem>

namespace Cfunc {
	void loadCFunc(lua_State* L);
	int multiply_2_numbers(lua_State* L);
}
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
	void Test();
	void init();
	void install();
	void loop();
	void deinit();
	void update();
	void launch();
};