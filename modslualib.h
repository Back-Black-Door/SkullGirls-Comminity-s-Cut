// modslualib.h
#pragma once

#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

namespace fs = std::filesystem;

class Mod {
public:
    struct ModHeader {
        std::string modName;
        int modVersion{ 0 };
        std::string modAuthor;
        std::filesystem::path modPath;
    };

private:
    lua_State* LuaState;
    int luaRefMod;

public:
    Mod(const Mod&) = delete;
    Mod& operator=(const Mod&) = delete;
    Mod(Mod&&) = delete;
    Mod& operator=(Mod&&) = delete;

    explicit Mod(const std::string& modPath);
    ~Mod();

    ModHeader ModInfo;

    bool callLuaFunction(const std::string& functionName);
    void init();
    void install();
    void loop();
    void deinit();
    void update();
    void launch();
};

extern std::vector<std::unique_ptr<Mod>> mods;

namespace CCLib {
    typedef enum {
        VAR_NUMBER,
        VAR_INTEGER,
        VAR_STRING,
        VAR_BOOLEAN,
        VAR_NIL,
        VAR_LIGHTUSERDATA
    } VarType;

    typedef struct {
        const char* name;
        VarType type;
        union {
            lua_Number num;
            lua_Integer integer;
            const char* str;
            int boolean;
            void* lightud;
        } value;
    } luaL_Var;

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
    int allocate_executable_memory(lua_State* L);
    int has_mod(lua_State* L);

    inline const luaL_Reg CClib_func[] = {
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
     {"allocate_executable_memory", allocate_executable_memory},
     {"has_mod", has_mod},
     {nullptr, nullptr} // Маркер конца
    };
    void push_vars(lua_State* L, const luaL_Var* vars);
}