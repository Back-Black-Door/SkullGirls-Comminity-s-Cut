ModInfo = {
    ModName = "TestLua",   --Mod Name. Be original - If ModName repeat another mod name, we will got error
    Version = 1,           --Mod Version. Used in Mod.Update() func
    Author = "ImpDi",      --Not used anywhere, but hey - leave your mark :^>
    Path = "mods/test.lua" --Mod Path. 
}

Mod = {
    install = function ()           --What we will do, when first time install mod (Mod NOT in savefile)
        print("[Lua] install hello!")
    end;
    update = function ()            --What we will do, when update mod (Mod Version in lua, higher then in savefile)
        print("[Lua] update hello!")
    end;
    init = function ()              --What we will do, when init mod (One time executed, BEFORE game in launched)
        print("[Lua] init hello!")
    end;
    launch = function ()            --What we will do, when Skullgirls already launched (One time executed, AFTER game in launched)
        print("[Lua] launch hello!")
    end;
    loop = function ()              -- What we will do, when Skullgirls already launched, and do this in loop (Loop executing, AFTER game in launched)
        print("[Lua] loop hello!")
    end;
    deinit = function ()            -- What we will do, when Skullgirls want to exit, (One time executed)
        print("[Lua] deinit hello!")
    end;
}