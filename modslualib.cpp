#include "modslualib.h"
#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include <memory>
#include <fstream>
#include <iostream>
#include "dllmain.h"
#include "formats/gfs.h"
#include "formats/gbs.h"
#include "thread/ScopedLock.h"
#include "process/process.h"

static CriticalSection g_luaCs;  // Using our class instead of Windows's
std::vector<std::unique_ptr<Mod>> mods; //Mods vector


Mod::Mod(const std::string& modPath) : LuaState(nullptr), luaRefMod(LUA_NOREF) {
    std::cout << "[C] Creating Mod from: " << modPath << std::endl;

    LuaState = luaL_newstate();
    if (!LuaState) {
        throw std::runtime_error("Failed to create Lua state");
    }

    try {
        static const CCLib::luaL_Var mylib_vars[] = {
            {"work_dir", CCLib::VAR_STRING, {.str = main_paths::work_dir_path.c_str()}},
            {"data01_dir", CCLib::VAR_STRING, {.str = main_paths::data01_dir_path.c_str()}},
            {"data02_dir", CCLib::VAR_STRING, {.str = main_paths::data02_dir_path.c_str()}},
            {"savefile_path", CCLib::VAR_STRING, {.str = main_paths::save_file_path.c_str()}},
            {"salfile_path", CCLib::VAR_STRING, {.str = main_paths::sal_file_path.c_str()}},
            {nullptr, CCLib::VAR_NIL, {0}}
        };

        luaL_openlibs(LuaState);
        luaL_newlib(LuaState, CCLib::CClib_func);
        CCLib::push_vars(LuaState, mylib_vars);
        lua_setglobal(LuaState, "CCLib");

        if (luaL_loadfile(LuaState, modPath.c_str()) != LUA_OK ||
            lua_pcall(LuaState, 0, 0, 0) != LUA_OK) {
            std::string error = lua_tostring(LuaState, -1);
            throw std::runtime_error("Lua error: " + error);
        }

        ScopedLock lock(g_luaCs);  //Using RAII
        lua_getglobal(LuaState, "ModInfo");
        if (lua_istable(LuaState, -1)) {
            lua_getfield(LuaState, -1, "ModName");
            if (lua_isstring(LuaState, -1)) {
                ModInfo.modName = lua_tostring(LuaState, -1);
            }
            lua_pop(LuaState, 1);

            lua_getfield(LuaState, -1, "Version");
            if (lua_isnumber(LuaState, -1)) {
                ModInfo.modVersion = static_cast<int>(lua_tonumber(LuaState, -1));
            }
            lua_pop(LuaState, 1);

            lua_getfield(LuaState, -1, "Author");
            if (lua_isstring(LuaState, -1)) {
                ModInfo.modAuthor = lua_tostring(LuaState, -1);
            }
            lua_pop(LuaState, 1);

            lua_getfield(LuaState, -1, "Path");
            if (lua_isstring(LuaState, -1)) {
                ModInfo.modPath = lua_tostring(LuaState, -1);
            }
            lua_pop(LuaState, 1);
            lua_pop(LuaState, 1);
        }
        else {
            lua_pop(LuaState, 1);
        }

        lua_getglobal(LuaState, "Mod");
        if (lua_istable(LuaState, -1)) {
            luaRefMod = luaL_ref(LuaState, LUA_REGISTRYINDEX);
        }
        else {
            lua_pop(LuaState, 1);
            luaRefMod = LUA_NOREF;
        }
    }
    catch (...) {
        if (LuaState) {
            lua_close(LuaState);
            LuaState = nullptr;
        }
        throw;
    }
}

Mod::~Mod() {
    ScopedLock lock(g_luaCs);  //Using RAII
    if (LuaState) {
        if (luaRefMod != LUA_NOREF) {
            luaL_unref(LuaState, LUA_REGISTRYINDEX, luaRefMod);
        }
        lua_close(LuaState);
        LuaState = nullptr;
    }
}

bool Mod::callLuaFunction(const std::string& functionName) {
    ScopedLock lock(g_luaCs);  //Using RAII
    if (!LuaState || luaRefMod == LUA_NOREF) return false;

    int top = lua_gettop(LuaState);
    lua_rawgeti(LuaState, LUA_REGISTRYINDEX, luaRefMod);

    if (!lua_istable(LuaState, -1)) {
        lua_settop(LuaState, top);
        return false;
    }

    lua_getfield(LuaState, -1, functionName.c_str());
    if (!lua_isfunction(LuaState, -1)) {
        lua_settop(LuaState, top);
        return false;
    }

    lua_remove(LuaState, -2);

    if (lua_pcall(LuaState, 0, 0, 0) != LUA_OK) {
        const char* error = lua_tostring(LuaState, -1);
        std::cerr << "[LUA] Error in " << functionName << ": " << error << std::endl;
        lua_settop(LuaState, top);
        return false;
    }

    return true;
}

void Mod::install() { callLuaFunction("install"); }
void Mod::update() { callLuaFunction("update"); }
void Mod::init() { callLuaFunction("init"); }
void Mod::launch() { callLuaFunction("launch"); }
void Mod::loop() { callLuaFunction("loop"); }
void Mod::deinit() { callLuaFunction("deinit"); }


namespace CCLib {
      int GetGameBaseAdress(lua_State* L) {
          int result = SGProccesInfo.dwBaseAddress;
          lua_pushnumber(L, result); 
          return 1; 
      }
      int ReadAddressNum(lua_State* L) {

          if (lua_gettop(L) != 1) {
              return luaL_error(L, "Expected 1 arguments: address.");
          }

          // Checks argument type
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
          lua_pushnumber(L, result); 
          return 1; 
      }
      int ReadAddressStr(lua_State* L) {

          if (lua_gettop(L) < 2) {
              return luaL_error(L, "Expected 2 arguments: address and size");
          }

          // Checks argument type
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
          lua_pushlstring(L, result.data(), size); 
          return 1; 
      }
      int WriteAddressStr(lua_State* L) {

          if (lua_gettop(L) < 3) {
              return luaL_error(L, "Expected 3 arguments: address string size and string");
          }

          // Checks argument type
          if (!lua_isnumber(L, 1)) {
              return luaL_error(L, "First argument must be number.");
          }
          if (!lua_isstring(L, 2)) {
              return luaL_error(L, "Second argument must be number.");
          }
          if (!lua_isstring(L, 3)) {
              return luaL_error(L, "Third argument must be string.");
          }

          
          const uintptr_t address = static_cast<uintptr_t>(lua_tonumber(L, 1));
          size_t len = static_cast<size_t>(lua_tonumber(L, 2)); ;
          const char* data = lua_tolstring(L, 3, &len);
          std::vector<unsigned char> binary_data(data, data + len);

          BOOL Success = WriteProcessMemory(
              SGProccesInfo.SGpi.hProcess,
              reinterpret_cast<LPVOID>(address),
              binary_data.data(),
              binary_data.size(),
              0);
          lua_pushboolean(L, Success); 
          return 1; 
      }
      int WriteAddressNum(lua_State* L) {
          int nargs = lua_gettop(L);

          if (lua_gettop(L) < 2) {
              return luaL_error(L, "At least 2 arguments are expected: address and number.");
          }

          // Checks argument type
          if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
              return luaL_error(L, "Both arguments must be numbers.");
          }

          const uintptr_t address = static_cast<uintptr_t>(lua_tonumber(L, 1));
          int size = 32;
          if (nargs >= 3) {
              size = static_cast<int>(lua_tonumber(L, 3));
          }

          // Declare a pointer and size variable outside the switch
          void* numPtr = nullptr;
          size_t dataSize = 0;

          switch (size) {
          case 8: {
              uint8_t num = static_cast<uint8_t>(lua_tonumber(L, 2));
              numPtr = &num;
              dataSize = sizeof(num);
              break;
          }
          case 16: {
              uint16_t num = static_cast<uint16_t>(lua_tonumber(L, 2));
              numPtr = &num;
              dataSize = sizeof(num);
              break;
          }
          case 32: {
              uint32_t num = static_cast<uint32_t>(lua_tonumber(L, 2));
              numPtr = &num;
              dataSize = sizeof(num);
              break;
          }
          case 64: {
              uint64_t num = static_cast<uint64_t>(lua_tonumber(L, 2));
              numPtr = &num;
              dataSize = sizeof(num);
              break;
          }
          default: {
              return luaL_error(L, "Wrong size at \"WriteAddressNum\"");
              break;
          }
          }

          BOOL Success = WriteProcessMemory(
              SGProccesInfo.SGpi.hProcess,
              reinterpret_cast<LPVOID>(address),
              numPtr,
              dataSize,
              0);
          lua_pushboolean(L, Success); 
          return 1; 
      }
      int GetWorkingDirectory(lua_State* L) {
          std::string result = main_paths::work_dir_path;
          lua_pushstring(L, result.c_str()); 
          return 1; 
      }
      int GFS_addfile(lua_State* L) {

          if (lua_gettop(L) < 3) {
              return luaL_error(L, "Expected 3 arguments: gfs archive name,relative path and path to files");
          }

          // Checks argument type
          if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
              return luaL_error(L, "All arguments must be string.");
          }

          const std::string gfsname = static_cast<std::string>(lua_tostring(L, 1));
          const std::string relative_path = static_cast<std::string>(lua_tostring(L, 2));
          const fs::path filepath = static_cast<std::string>(lua_tostring(L, 3));
          fs::path data01_path = main_paths::data01_dir_path;
          fs::path data02_path = main_paths::data02_dir_path;
          fs::path targetdata02 = data02_path / gfsname;
          
          targetdata02.replace_extension(".gfs");
            if (fs::is_symlink(targetdata02)) {
                fs::remove(targetdata02);
                fs::copy_file((data01_path / gfsname).replace_extension(".gfs"), targetdata02);
            } 
            else {
                if (!fs::exists(targetdata02)) {
                    fs::copy_file((data01_path / gfsname).replace_extension(".gfs"), targetdata02);
                }
            }
              auto meta_it = std::find_if(
                  GFS_CHANGES.begin(),
                  GFS_CHANGES.end(),
                  [&](const GFSEdit& edit)
                  { return edit.gfs_path == targetdata02; });

                if (meta_it != GFS_CHANGES.end()) {
                    meta_it->add_file(filepath, relative_path, 1);
                }
                else {
                    GFSEdit Current(targetdata02);
                    Current.add_file(filepath, relative_path, 1);
                    GFS_CHANGES.emplace_back(Current);
                }
          lua_pushboolean (L, 1); 
          return 1; 
      }
      int GFS_addfiles(lua_State* L) {

          if (lua_gettop(L) < 3) {
              return luaL_error(L, "Expected 3 arguments: gfs archive name,relative path and path to files");
          }

          // Checks argument type
          if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
              return luaL_error(L, "All arguments must be string.");
          }

          const std::string gfsname = static_cast<std::string>(lua_tostring(L, 1));
          const std::string relative_path = static_cast<std::string>(lua_tostring(L, 2));
          const fs::path filepath = static_cast<std::string>(lua_tostring(L, 3));
          fs::path data01_path = main_paths::data01_dir_path;
          fs::path data02_path = main_paths::data02_dir_path;
          fs::path targetdata02 = data02_path / gfsname;

          targetdata02.replace_extension(".gfs");
          if (fs::is_symlink(targetdata02)) {
              fs::remove(targetdata02);
              fs::copy_file((data01_path / gfsname).replace_extension(".gfs"), targetdata02);
          }
          else {
              if (!fs::exists(targetdata02)) {
                  fs::copy_file((data01_path / gfsname).replace_extension(".gfs"), targetdata02);
              }
          }
          auto meta_it = std::find_if(
              GFS_CHANGES.begin(),
              GFS_CHANGES.end(),
              [&](const GFSEdit& edit)
              { return edit.gfs_path == targetdata02; });

          if (meta_it != GFS_CHANGES.end()) {
              meta_it->add_files(filepath, relative_path, 1);
          }
          else {
              GFSEdit Current(targetdata02);
              Current.add_files(filepath, relative_path, 1);
              GFS_CHANGES.emplace_back(Current);
          }
          lua_pushboolean(L, 1); 
          return 1; 
      }
      int Add_New_Permission(lua_State* L) {
          if (lua_gettop(L) < 1) {
              return luaL_error(L, "Expected 1 arguments: New permission");
          }

          // Checks argument type
          if (!lua_isstring(L, 1)) {
              return luaL_error(L, "Arguments must be string.");
          }

          const std::string NewPermission = static_cast<std::string>(lua_tostring(L, 1));

          std::ifstream inFile(main_paths::sal_file_path);
          if (!inFile) {
              std::cerr << "Ошибка открытия файла!" << std::endl;
              return 1;
          }

          // Reading all lines in vector
          std::vector<std::string> lines;
          std::string line;
          while (std::getline(inFile, line)) {
              lines.push_back(line);
          }
          inFile.close();

          // Insert new line
          lines.insert(lines.end() - 1, NewPermission);

          // Re-write file
          std::ofstream outFile(main_paths::sal_file_path);
          for (const auto& l : lines) {
              outFile << l << "\n";
          }
          outFile.close();

          return 0;
      }
      int GFS_extract_file(lua_State* L) {
          if (lua_gettop(L) < 3) {
              return luaL_error(L, "Expected 3 arguments: gfs archive name, relative path and path to file extract");
          }

          // Checks argument type
          if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
              return luaL_error(L, "All arguments must be string.");
          }
          const std::string gfsname = static_cast<std::string>(lua_tostring(L, 1));
          const std::string relative_path = static_cast<std::string>(lua_tostring(L, 2));
          const fs::path filepath = static_cast<std::string>(lua_tostring(L, 3));
          fs::path data01_path = main_paths::data01_dir_path;
          fs::path data02_path = main_paths::data02_dir_path;
          fs::path targetdata02 = data02_path / gfsname;

          targetdata02.replace_extension(".gfs");
          if (fs::is_symlink(targetdata02)) {
              fs::remove(targetdata02);
              fs::copy_file((data01_path / gfsname).replace_extension(".gfs"), targetdata02);
          }
          else {
              if (!fs::exists(targetdata02)) {
                  fs::copy_file((data01_path / gfsname).replace_extension(".gfs"), targetdata02);
              }
          }
          auto meta_it = std::find_if(
              GFS_CHANGES.begin(),
              GFS_CHANGES.end(),
              [&](const GFSEdit& edit)
              { return edit.gfs_path == targetdata02; });

          if (meta_it != GFS_CHANGES.end()) {
              meta_it->extract_file(relative_path, filepath);
          }
          else {
              GFSEdit Current(targetdata02);
              Current.extract_file(relative_path, filepath);
              GFS_CHANGES.emplace_back(Current);
          }
          lua_pushboolean(L, 1); 
          return 1; 
      }
      int GBS_merge(lua_State* L) {
          if (lua_gettop(L) < 3) {
              return luaL_error(L, "Expected 3 arguments: gfs archive name, relative path and path to file extract");
          }

          // Checks argument type
          if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
              return luaL_error(L, "All arguments must be string.");
          }

          const std::string original = static_cast<std::string>(lua_tostring(L, 1));
          const std::string mergewith = static_cast<std::string>(lua_tostring(L, 2));
          const fs::path pathwowrite = static_cast<std::string>(lua_tostring(L, 3));

          gbs::gbs_t Current(original);
          gbs::gbs_t Second(mergewith);
          gbs::gbs_t merged_gbs = gbs::merge(Current, Second , gbs::add_new_fonts | gbs::calculate_texture_id | gbs::divide_coords);
          merged_gbs.write(pathwowrite);
      }
      int allocate_executable_memory(lua_State* L) {
          if (!lua_isnumber(L, 1)) {
              return luaL_error(L, "Argument must be number.");
          }
          const size_t size = static_cast<size_t>(lua_tonumber(L, 1));
          LPVOID memory = VirtualAlloc(
              nullptr,
              size,
              MEM_COMMIT | MEM_RESERVE,
              PAGE_EXECUTE_READWRITE
          );
          lua_pushnumber(L, DWORD(memory)); 
          return 1; 
      }
      int has_mod(lua_State* L) {
          if (!lua_isstring(L, 1)) {
              return luaL_error(L, "Argument must be string.");
          }
          const std::string ModName = static_cast<std::string>(lua_tostring(L, 1));
          bool hasmod = false;
          for (const auto& mod : mods) {
              if (mod->ModInfo.modName == ModName) {
                  hasmod = true;
                  break;
              }
          }
          lua_pushboolean(L, hasmod);
          return 1; 
      }

      int getModVersion(lua_State* L) {
          if (!lua_isstring(L, 1)) {
              return luaL_error(L, "Argument must be string.");
          }
          const std::string ModName = static_cast<std::string>(lua_tostring(L, 1));
          int version = 0;
          for (const auto& mod : mods) {
              if (mod->ModInfo.modName == ModName) {
                  version = mod->ModInfo.modVersion;
                  break;
              }
          }
          lua_pushnumber(L, version);
          return 1;
      }


      void push_vars(lua_State* L, const CCLib::luaL_Var* vars) {
    for (; vars->name != NULL; vars++) {
        switch (vars->type) {
        case VAR_NUMBER:
            lua_pushnumber(L, vars->value.num);
            break;
        case VAR_INTEGER:
            lua_pushinteger(L, vars->value.integer);
            break;
        case VAR_STRING:
            lua_pushstring(L, vars->value.str);  // Carefull: str need be valid!
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
        lua_setfield(L, -2, vars->name);  // Adding to table
    }
}
}