# Patching
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

# Filesystem
## Filesystem

### GetWorkingDirectory()
Return string with full path to SkullGirls Directory (...\steamapps\common\Skullgirls)

Exemple:
```
launch = function ()  
    local i = CCLib.GetWorkingDirectory()
    print("[Lua] My Working Directory: ")
    print(i)
    end;

Output: "123"
```

### gfs_addfiles("gfsarhivename", path to folder with files)
Add Files to gfs archive in data02 (Create copy from data01, if doesn't exist, and than add)

Return True, if success

Exemple:
```
init = function ()  
    local i = CCLib.GetWorkingDirectory()
    i = i .. "\\mods\\test\\core"
    print("[Lua] My Files Directory: ")
    print(i)
    CCLib.gfs_addfiles("core", i)
    end;

Output: "123"
```
