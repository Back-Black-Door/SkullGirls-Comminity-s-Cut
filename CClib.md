# Patching
## Functions
### GameBaseAdress() 
The returned number is Game Base Adress

```
launch = function ()           
    local i = CCLib.GameBaseAdress()
    print("[Lua] GameBaseAdress: ")
    print(i)
    end;

Output: 
```
### ReadAdressStr (number adress, number size)
Return string from game memory from __adress__.

Exemple:
```
launch = function ()           
    local i = CCLib.ReadAdressStr(0x00841984, 4 )
    print("[Lua] Value at "Skullgirls.exe" + 0x00841984: ")
    print(i)
    end;

Output: 
```
### ReadAdressNum (number adress)
Return number(int) from game memory from __adress__.

Exemple:
```
launch = function ()           
    local i = CCLib.ReadAdressNum(0x00841984)
    print("[Lua] Value at "Skullgirls.exe" + 0x00841984:")
    print(i)
    end;

Output: 
```

### WriteAdressStr (number adress, string message)
Write string to game memory from __adress__.

Exemple:
```
launch = function ()  
    local str = "SkullGirls_CC_Is_Good!"
    CCLib.WriteAdressStr(0x00841984, str )
    local i = CCLib.ReadAdressStr(0x00841984, #str )
    print("[Lua] Value at "Skullgirls.exe" + 0x00841984: ")
    print(i)
    end;

Output: 
```

## Constants
