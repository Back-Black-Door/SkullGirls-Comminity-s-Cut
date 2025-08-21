#include "modslualib.h"
#pragma comment(lib, "lua54.lib")
#include <lua.hpp>
#include <memory>
#include "main.h"
#include "formats/gfs.h"
#include "formats/gbs.h"

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
          std::string result = main_paths::work_dir_path.string();
          lua_pushstring(L, result.c_str()); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int GFS_addfile(lua_State* L) {

          if (lua_gettop(L) < 3) {
              return luaL_error(L, "Expected 3 arguments: gfs archive name,relative path and path to files");
          }

          // Проверяем типы аргументов
          if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
              return luaL_error(L, "All arguments must be string.");
          }

          const std::string gfsname = static_cast<std::string>(lua_tostring(L, 1));
          const std::string relative_path = static_cast<std::string>(lua_tostring(L, 2));
          const fs::path filepath = static_cast<std::string>(lua_tostring(L, 3));
          fs::path targetdata02 = main_paths::data02_dir_path / gfsname;
          
          targetdata02.replace_extension(".gfs");
            if (fs::is_symlink(targetdata02)) {
                fs::remove(targetdata02);
                fs::copy_file((main_paths::data01_dir_path / gfsname).replace_extension(".gfs"), targetdata02);
            } 
            else {
                if (!fs::exists(targetdata02)) {
                    fs::copy_file((main_paths::data01_dir_path / gfsname).replace_extension(".gfs"), targetdata02);
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
          lua_pushboolean (L, 1); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int GFS_addfiles(lua_State* L) {

          if (lua_gettop(L) < 3) {
              return luaL_error(L, "Expected 3 arguments: gfs archive name,relative path and path to files");
          }

          // Проверяем типы аргументов
          if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
              return luaL_error(L, "All arguments must be string.");
          }

          const std::string gfsname = static_cast<std::string>(lua_tostring(L, 1));
          const std::string relative_path = static_cast<std::string>(lua_tostring(L, 2));
          const fs::path filepath = static_cast<std::string>(lua_tostring(L, 3));
          fs::path targetdata02 = main_paths::data02_dir_path / gfsname;

          targetdata02.replace_extension(".gfs");
          if (fs::is_symlink(targetdata02)) {
              fs::remove(targetdata02);
              fs::copy_file((main_paths::data01_dir_path / gfsname).replace_extension(".gfs"), targetdata02);
          }
          else {
              if (!fs::exists(targetdata02)) {
                  fs::copy_file((main_paths::data01_dir_path / gfsname).replace_extension(".gfs"), targetdata02);
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
          lua_pushboolean(L, 1); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int Add_New_Permission(lua_State* L) {
          if (lua_gettop(L) < 1) {
              return luaL_error(L, "Expected 1 arguments: New permission");
          }

          // Проверяем типы аргументов
          if (!lua_isstring(L, 1)) {
              return luaL_error(L, "Arguments must be string.");
          }

          const std::string NewPermission = static_cast<std::string>(lua_tostring(L, 1));

          std::ifstream inFile(main_paths::sal_file_path);
          if (!inFile) {
              std::cerr << "Ошибка открытия файла!" << std::endl;
              return 1;
          }

          // Читаем все строки в вектор
          std::vector<std::string> lines;
          std::string line;
          while (std::getline(inFile, line)) {
              lines.push_back(line);
          }
          inFile.close();

          // Вставляем новую строку, например, после 2-й строки
          lines.insert(lines.end() - 1, NewPermission);

          // Перезаписываем файл
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

          // Проверяем типы аргументов
          if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3)) {
              return luaL_error(L, "All arguments must be string.");
          }
          const std::string gfsname = static_cast<std::string>(lua_tostring(L, 1));
          const std::string relative_path = static_cast<std::string>(lua_tostring(L, 2));
          const fs::path filepath = static_cast<std::string>(lua_tostring(L, 3));
          fs::path targetdata02 = main_paths::data02_dir_path / gfsname;

          targetdata02.replace_extension(".gfs");
          if (fs::is_symlink(targetdata02)) {
              fs::remove(targetdata02);
              fs::copy_file((main_paths::data01_dir_path / gfsname).replace_extension(".gfs"), targetdata02);
          }
          else {
              if (!fs::exists(targetdata02)) {
                  fs::copy_file((main_paths::data01_dir_path / gfsname).replace_extension(".gfs"), targetdata02);
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
          lua_pushboolean(L, 1); // Возвращаем результат
          return 1; // Количество возвращаемых значенийк
      }
      int GBS_merge(lua_State* L) {
          if (lua_gettop(L) < 3) {
              return luaL_error(L, "Expected 3 arguments: gfs archive name, relative path and path to file extract");
          }

          // Проверяем типы аргументов
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
}