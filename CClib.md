## Patching

### GetGameBaseAdress() 
The returned number is Game Base Adress

```
launch = function ()           
    local i = CCLib.GetGameBaseAdress()
    print("[Lua] GameBaseAdress: ")
    print(i)
    end;

Output: 10747904
```
### ReadAdressStr (number adress, number size)
Return string from game memory from __adress__.

Exemple:
```
launch = function ()           
    local i = CCLib.ReadAdressStr(0x00841984, 7)
    print("[Lua] Value at "Skullgirls.exe" + 0x00841984: ")
    print(i)
    end;

Output: "exemple"
```
### ReadAdressNum (number adress)
Return number(int) from game memory from __adress__.

Exemple:
```
launch = function ()           
    local i = CCLib.ReadAdressNum(0x00841984)
    print("[Lua] Value at 0x00841984:")
    print(i)
    end;

Output: 0
```

### WriteAdressStr (number adress, string message)
Write string to game memory from __adress__. Return true(bool) if success.

Exemple:
```
launch = function ()  
    local str = "SkullGirls_CC_Is_Good!"
    CCLib.WriteAdressStr(0x00841984, str )
    local i = CCLib.ReadAdressStr(0x00841984, #str )
    print("[Lua] Value at 0x00841984: ")
    print(i)
    end;

Output: "SkullGirls_CC_Is_Good"
```

### WriteAdressNum (number adress, number num)
Write Number to game memory from __adress__. Return true(bool) if success.

Exemple:
```
launch = function ()  
    CCLib.WriteAdressNum(0x00841984, 123 )
    local i = CCLib.ReadAdressNum(0x00841984)
    print("[Lua] Value at 0x00841984: ")
    print(i)
    end;

Output: "123"
```

## Filesystem

### Constant:

```
CCLib.CC_version --Curent CC Version(Number)
CCLib.work_dir   --full path (String) to SkullGirls Directory (...\steamapps\common\Skullgirls) 
CCLib.data01_dir --full path (String) to SkullGirls data01 (...\steamapps\common\Skullgirls\data01) 
CCLib.data02_dir --full path (String) to SkullGirls data02 (...\steamapps\common\Skullgirls\data02) 
CCLib.savefile_path --full path (String) to CC savefile (...\steamapps\common\Skullgirls\saves_CC.json) 
CCLib.salfile_path --full path (String) to CC sal file (...\steamapps\common\Skullgirls\Salmon\FULL_SGCC.sal) 
```

### gfs_addfiles("gfsarhivename",relative path, path to files)
Add Files to gfs archive in data02 (Create copy from data01, if doesn't exist, and than add)

Return True, if success

Exemple:
```
init = function ()  
    local i = CCLib.GetWorkingDirectory()
    i = i .. "\\mods\\test\\core"
    print("[Lua] My Files Directory: ")
    print(i)
    CCLib.gfs_addfiles("core","", i)
    end;
```

### gfs_addfile("gfsarhivename", relative path, path to file)
Add Files to gfs archive in data02 (Create copy from data01, if doesn't exist, and than add)

Return True, if success

Exemple:
```
init = function ()  
    local i = CCLib.GetWorkingDirectory()
    i = i .. "\\mods\\test\\core"
    print("[Lua] My Files Directory: ")
    print(i)
    CCLib.gfs_addfile("core","", i)
    end;
```

### gfs_extract_file("gfsarhivename", relative path, path to file)

Extrart File from gfs archive to "path to file" (Create copy from data01, if doesn't exist, and than add)

Return True, if success

Exemple:
```
init = function ()  
    local i = CCLib.GetWorkingDirectory()
    i = i .. "\\mods\\test\\stages.ini"
    CCLib.gfs_extract_file("core", "core/stages.ini", i)
    end;
```

### gbs_merge(original gbs, gbs with which merge, path to write)

Merge two gbs, using original *.gbs like a base. Adding fonts and textures.

Return True, if success

Exemple:
```
init = function ()  
    local i = CCLib.GetWorkingDirectory()
    local Charselect_gbs = i .. "\\mods\\test\\Charselect.gbs"
    local Charselect_gbs_merge_with = i .. "\\mods\\test\\Charselect_merge.gbs"
    local Charselect_gbs_path_to_write = i .. "\\mods\\test\\ui-win\\ui\\Win\\Charselect.gbs"
    CCLib.gbs_merge(Charselect_gbs, Charselect_gbs_merge_with, Charselect_gbs_path_to_write)
    end;
```

