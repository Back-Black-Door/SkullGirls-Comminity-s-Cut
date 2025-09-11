# CClib Documentation for ModLoader
## Introduction
CCLib provides a set of functions for working with game memory and the file system in Skullgirls. All functions are available through the CCLib table.

## Memory Operations
### getGameBaseAddress()
Returns the game's base memory address.

Return value: `number` (base address)

Example:

```lua
launch = function()
    local baseAddress = CCLib.getGameBaseAddress()
    print("[Lua] GameBaseAddress: " .. baseAddress)
end

-- Output: 10747904
```

### readAddressString(address, size)
Reads a string from the game's memory at the specified address.

Parameters:

`address` - memory address

`size` - number of characters to read

Return value: `string`

Example:

```lua
launch = function()
    local value = CCLib.readAddressString(0x00841984, 7)
    print("[Lua] Value at 0x00841984: " .. value)
end

-- Output: "example"
```

### readAddressNumber(address)
Reads a numeric value from the game's memory at the specified address.

Parameters:

`address` - memory address

Return value: `number`

Example:

```lua
launch = function()
    local value = CCLib.readAddressNumber(0x00841984)
    print("[Lua] Value at 0x00841984: " .. value)
end

-- Output: 0
```

### writeAddressString(address, message)
Writes a string to the game's memory at the specified address.

Parameters:

`address` - memory address

`message` - string to write

Return value: `boolean` (true on success)

Example:
```lua
launch = function()
    local str = "SkullGirls_CC_Is_Good!"
    local success = CCLib.writeAddressString(0x00841984, str)
    
    if success then
        local readValue = CCLib.readAddressString(0x00841984, #str)
        print("[Lua] Written value: " .. readValue)
    end
end

-- Output: "SkullGirls_CC_Is_Good"
```

### writeAddressNumber(address, num)
Writes a number to the game's memory at the specified address.

Parameters:

`address` - memory address

`num` - number to write

Return value: `boolean` (true on success)

Example:

```lua
launch = function()
    local success = CCLib.writeAddressNumber(0x00841984, 123)
    
    if success then
        local readValue = CCLib.readAddressNumber(0x00841984)
        print("[Lua] Written value: " .. readValue)
    end
end

-- Output: 123
```

## Filesystem Operations
### Constants
Available path constants:

```lua
CCLib.CC_version       -- Current CC version (number)
CCLib.work_dir         -- Full path to Skullgirls directory
CCLib.data01_dir       -- Full path to data01
CCLib.data02_dir       -- Full path to data02
CCLib.savefile_path    -- Full path to CC save file
CCLib.salfile_path     -- Full path to CC sal file (FULL_SGCC.sal)
```
### gfsAddFile(gfs_archive_name, relative_path, files_path)
Adds files to a GFS archive in data02 (creates a copy from data01 if it doesn't exist).

Parameters:

`gfs_archive_name` - name of the GFS archive

`relative_path` - relative path within the archive

`files_path` - path to files to add

Return value: `boolean` (true on success)

Example:
```lua
init = function()
    local modPath = CCLib.work_dir .. "\\mods\\test\\core"
    local success = CCLib.gfsAddFile("core", "", modPath)
    print("[Lua] Files added: " .. tostring(success))
end
```

### gfsAddFiles(gfs_archive_name, relative_path, file_path)
Adds a single file to a GFS archive in data02.

Parameters:

`gfs_archive_name` - name of the GFS archive

`relative_path` - relative path within the archive

`file_path` - path to the file to add

Return value: `boolean` (true on success)

Example:

```lua
init = function()
    local filePath = CCLib.work_dir .. "\\mods\\test\\core\\file.txt"
    local success = CCLib.gfsAddFiles("core", "file.txt", filePath)
    print("[Lua] File added: " .. tostring(success))
end
```

### gfsExtractFile(gfs_archive_name, relative_path, extract_path)
Extracts a file from a GFS archive.

Parameters:

`gfs_archive_name` - name of the GFS archive

`relative_path` - relative path to the file in the archive

`extract_path` - path to extract the file to

Return value: `boolean` (true on success)

Example:

```lua
init = function()
    local extractPath = CCLib.work_dir .. "\\mods\\test\\stages.ini"
    local success = CCLib.gfsExtractFile("core", "core/stages.ini", extractPath)
    print("[Lua] File extracted: " .. tostring(success))
end
```

### gbsMerge(original_gbs, merge_gbs, output_path)
Merges two GBS files, using the original file as a base.

Parameters:

`original_gbs` - path to the original GBS file

`merge_gbs` - path to the GBS file to merge with

`output_path` - path to save the result

Return value: `boolean` (true on success)

Example:

```lua
init = function()
    local original = CCLib.work_dir .. "\\mods\\test\\Charselect.gbs"
    local mergeWith = CCLib.work_dir .. "\\mods\\test\\Charselect_merge.gbs"
    local output = CCLib.work_dir .. "\\mods\\test\\ui-win\\ui\\Win\\Charselect.gbs"
    
    local success = CCLib.gbsMerge(original, mergeWith, output)
    print("[Lua] GBS merged: " .. tostring(success))
end
```
## Mods Interaction
I would ask you to make mods more independent and self-contained. 
If you really need another mod, use `required` - 
if such a mod is not in the folder with yours, 
we will get an error that will tell us about the need to download another mod.

### hasMod(ModName)

Checks if we have the required mod (Doesn't raise an error! Be careful)

Parameters:

 - string `ModName` - The name of the mod we are asking about

Return value: `boolean` (true if we have)

Example:

```lua

launch = function()
    local ExempleMod = CCLib.hasMod("ExempleMod")
    if ExempleMod then
        print("Yes, we have!")
    else
        print("No, we haven't!")
    end
end;
```

### getModVersion(ModName)

Returns the version number of the mod.
If there is no such mod, returns 0

Parameters:

 - string `ModName` - The name of the mod we are asking about

Return value: `number` (0 if we haven't)

Example:

```lua

launch = function()
        local ExempleMod = CCLib.getModVersion("ExempleMod")
        if ExempleMod==0 then
            print("We haven't ExempleMod!")
        elseif ExempleMod==1 then
            print("ExempleMod version is 1!")
        elseif ExempleMod==2 then
            print("ExempleMod version is 2!")
        end
end;
```

## Exemple of Mod, than add new stage

```lua
ModInfo = {
    ModName = "Anna_Killer",   --Mod Name. Be original - If ModName repeat another mod name, an error will occur
    Version = 1,           --Mod Version. Used in Mod.Update() func
    Author = "ImpDi",      --Not used anywhere, but hey - leave your mark :^>
    Path = "mods/Anna_Killer.lua" --Mod Path. Someday, might be useful. Maybe.
}

Mod = {
    install = function ()           --What CC will do, when installing the mod for the first time (Mod is NOT in savefile)
        print("[Lua] Anna_Killer install hello!")
        local i = CCLib.work_dir    --Get working directory
        local stagestext = i .. "\\mods\\Anna_Killer\\stage_anna_killer.dds"    --Locate stage .dds (Picture) file
        local stageslvl = i .. "\\mods\\Anna_Killer\\cs_anna_killer.lvl"    --Locate stage .lvl file
        local uiwin = i .. "\\mods\\Anna_Killer\\ui-win"    --Locate ui-win folder (We can add a whole folder!)
        local music_intro = i .. "\\mods\\Anna_Killer\\tech_intro.wav"  --Locate music file
        local music_loop = i .. "\\mods\\Anna_Killer\\tech_loop.wav"    --Locate music file
        local Stagesinifile = i .. "\\mods\\Anna_Killer\\stages.ini"    --Locate stage.ini file
        local Charselect_gbs = i .. "\\mods\\Anna_Killer\\Charselect.gbs"   --Locate Charselect.gbs file
        local Charselect_gbs_merge = i .. "\\mods\\Anna_Killer\\Charselect_merge.gbs"   --Locate Charselect.gbs file
        local Charselect_gbs_path_to_write = i .. "\\mods\\Anna_Killer\\ui-win\\ui\\Win\\Charselect.gbs"
        CCLib.add_new_permission("Filia_Pal7")  --Writing new permission (line) in FULL_SGCC.sal
        CCLib.gfs_extract_file("core", "core/stages.ini", Stagesinifile )   --Extract stages.ini from core.gfs
        local file = io.open(Stagesinifile, "a+")
        if not file then
            return false, "Can't open file"
        end
        file:write("Filia_Pal7 cs_anna_killer stage_anna_killer\n")
        file:close()    --Write new line to file
        CCLib.gfs_addfile("core","core/stages.ini", Stagesinifile)  --Add file stages.ini to core.gfs

        CCLib.gfs_addfile("music-win","music/tech_intro.wav", music_intro)  --Add music file to music-win.gfs
        CCLib.gfs_addfile("music-win","music/tech_loop.wav", music_loop)    --Add music file to music-win.gfs

        CCLib.gfs_addfile("stages","stages/cs_anna_killer.lvl", stageslvl)  --Add stage .lvl file to stages.gfs
        CCLib.gfs_addfile("stages-textures2d","stages/textures/2D/stage_anna_killer.dds", stagestext)   --Add stage .dds (Picture) file to stages-textures2d.gfs

        CCLib.gfs_extract_file("ui-win", "ui/Win/Charselect.gbs", Charselect_gbs)   --Extract charselect.gbs file from ui-win.gfs
        CCLib.gbs_merge(Charselect_gbs, Charselect_gbs_merge, Charselect_gbs_path_to_write)     --merge two .gbs files, and write result to Charselect_gbs_path_to_write
        CCLib.gfs_addfiles("ui-win","", uiwin)  --Add entire folder to ui-win.gfs!
        print("[Lua] Anna_Killer install goodbye!")
    end;
    update = function ()    --What CC will do, when updating the mod (Mod Version in lua, higher then in savefile)
        Mod.install()   --We don't think about updates, so... just reinstall.
    end;
    init = function ()              --What CC will do, when initiating the mod (Executed once, BEFORE game in launched)
        print("[Lua] init hello!")
        local i = CCLib.GetWorkingDirectory()
        local Stagesinifile = i .. "\\mods\\Anna_Killer\\stages.ini"
        local Charselect_gbs = i .. "\\mods\\Anna_Killer\\Charselect.gbs"
        os.remove(Stagesinifile)
        os.remove(Charselect_gbs) --We delete temp files, if they exisist after install
    end;
    -- launch = function ()            --What CC will do, when Skullgirls is already launched (Executed once, AFTER the game is launched)
    --     print("[Lua] launch hello!")
    -- end;
    -- loop = function ()              -- What CC will do, when Skullgirls is already launched, and executes function in loop (Executing a loop, AFTER the game is launched)
    --     print("[Lua] loop hello!")
    -- end;
    --deinit = function ()            -- What CC will do, when Skullgirls wants to exit, (Executed once)
    --    print("[Lua] deinit hello!")
    --end; --We don't need launch, loop and deinit actions, so just comment this functions
}
```

You can find this code and required files for MODS_Anna_Killer [here](https://github.com/Back-Black-Door/MODS-Anna_Killer)
## Conclusion
This documentation covers the main CCLib functions for working with game memory and the filesystem. Use these functions to create mods and interact with the Skullgirls.