# Документация CClib для ModLoader

## Введение
CClib предоставляет набор функций для работы с памятью игры и файловой системой для Skullgirls. Все функции доступны через таблицу CCLib.

## Работа с памятью
### GetGameBaseAddress()
Возвращает базовый адрес игры в памяти.

Возвращаемое значение: `number` (base address)

Пример:
```lua
launch = function()
    local baseAddress = CCLib.GetGameBaseAddress()
    print("[Lua] GameBaseAddress: " .. baseAddress)
end

-- Вывод: 10747904
```
### ReadAddressStr(address, size)
Читает строку из памяти игры по указанному адресу.

Параметры:

`address` - адрес в памяти

`size` - количество символов для чтения

Возвращаемое значение: `string`

Пример:
```lua
launch = function()
    local value = CCLib.ReadAddressStr(0x00841984, 7)
    print("[Lua] Value at 0x00841984: " .. value)
end

-- Вывод: "exemple"
```
### ReadAddressNum(address)
Читает числовое значение из памяти игры по указанному адресу.

Параметры:

`address` - адрес в памяти

Возвращаемое значение: `number`

Пример:
```lua
launch = function()
    local value = CCLib.ReadAddressNum(0x00841984)
    print("[Lua] Value at 0x00841984: " .. value)
end

-- Вывод: 0
```

### WriteAddressStr(address, message)
Записывает строку в память игры по указанному адресу.

Параметры:

`address` - адрес в памяти

`message` - строка для записи

Возвращаемое значение: `boolean` (true при успехе)

Пример:
```lua
launch = function()
    local str = "SkullGirls_CC_Is_Good!"
    local success = CCLib.WriteAddressStr(0x00841984, str)
    if success then
        local readValue = CCLib.ReadAddressStr(0x00841984, #str)
        print("[Lua] Written value: " .. readValue)
    end
end

-- Вывод: "SkullGirls_CC_Is_Good"
```
### WriteAddressNum(address, num)
Записывает число в память игры по указанному адресу.

Параметры:

`address` - адрес в памяти

`num` - число для записи

Возвращаемое значение: `boolean` (true при успехе)

Пример:
```lua
launch = function()
    local success = CCLib.WriteAddressNum(0x00841984, 123)
    
    if success then
        local readValue = CCLib.ReadAddressNum(0x00841984)
        print("[Lua] Written value: " .. readValue)
    end
end

-- Вывод: 123
```
## Работа с файловой системой
## Константы
Доступные константы для путей:

```lua
CCLib.CC_version       -- Текущая версия CC (число)
CCLib.work_dir         -- Полный путь к директории Skullgirls
CCLib.data01_dir       -- Полный путь к data01
CCLib.data02_dir       -- Полный путь к data02
CCLib.savefile_path    -- Полный путь к файлу сохранений CC
CCLib.salfile_path     -- Полный путь к sal-файлу CC (FULL_SGCC.sal)
```

### gfs_addfiles(gfs_archive_name, relative_path, files_path)
Добавляет файлы в GFS-архив в data02 (создаёт копию из data01, если не существует).

Параметры:

`gfs_archive_name` - имя GFS-архива

`relative_path` - относительный путь в архиве

`files_path` - путь к добавляемым файлам

Возвращаемое значение: `boolean` (true при успехе)

Пример:

```lua
init = function()
    local modPath = CCLib.work_dir .. "\\mods\\test\\core"
    local success = CCLib.gfs_addfiles("core", "", modPath)
    print("[Lua] Files added: " .. tostring(success))
end
```

### gfs_addfile(gfs_archive_name, relative_path, file_path)
Добавляет один файл в GFS-архив в data02.

Параметры:

`gfs_archive_name` - имя GFS-архива

`relative_path` - относительный путь в архиве

`file_path` - путь к добавляемому файлу

Возвращаемое значение: `boolean` (true при успехе)

Пример:

```lua
init = function()
    local filePath = CCLib.work_dir .. "\\mods\\test\\core\\file.txt"
    local success = CCLib.gfs_addfile("core", "file.txt", filePath)
    print("[Lua] File added: " .. tostring(success))
end
```

### gfs_extract_file(gfs_archive_name, relative_path, extract_path)
Извлекает файл из GFS-архива.

Параметры:

`gfs_archive_name` - имя GFS-архива

`relative_path` - относительный путь к файлу в архиве

`extract_path` - путь для извлечения файла

Возвращаемое значение: `boolean` (true при успехе)

Пример:

```lua
init = function()
    local extractPath = CCLib.work_dir .. "\\mods\\test\\stages.ini"
    local success = CCLib.gfs_extract_file("core", "core/stages.ini", extractPath)
    print("[Lua] File extracted: " .. tostring(success))
end
```

### gbs_merge(original_gbs, merge_gbs, output_path)
Объединяет два GBS-файла, используя оригинальный файл как основу.

Параметры:

`original_gbs` - путь к оригинальному GBS-файлу

`merge_gbs` - путь к GBS-файлу для слияния

`output_path` - путь для сохранения результата

Возвращаемое значение: `boolean` (true при успехе)

Пример:

```lua
init = function()
    local original = CCLib.work_dir .. "\\mods\\test\\Charselect.gbs"
    local mergeWith = CCLib.work_dir .. "\\mods\\test\\Charselect_merge.gbs"
    local output = CCLib.work_dir .. "\\mods\\test\\ui-win\\ui\\Win\\Charselect.gbs"
    
    local success = CCLib.gbs_merge(original, mergeWith, output)
    print("[Lua] GBS merged: " .. tostring(success))
end
```
## Пример мода, который добавляет новую арену

```lua
ModInfo = {
    ModName = "Anna_Killer",   --Имя мода. Будьте оригинальны - если имя повториться, мы, вероятно, получим ошибку.
    Version = 1,           --Версия мода. Используется в Mod.update()
    Author = "ImpDi",      --Не используется нигде, но... Хей! Оставьте свой след :^>
    Path = "mods/Anna_Killer.lua" --Путь мода. Однажды, может быть необходимо. Однажды.
}

Mod = {
    install = function ()           --Что CC будет делать, когда мы только устанавилваем мод. (Мода НЕТ в сохранениях)
        print("[Lua] Anna_Killer install hello!")
        local i = CCLib.work_dir    --Получаем рабочую папку.
        local stagestext = i .. "\\mods\\Anna_Killer\\stage_anna_killer.dds"    --Указываем stage .dds (Картинка, фон) файл
        local stageslvl = i .. "\\mods\\Anna_Killer\\cs_anna_killer.lvl"    --Указываем stage .lvl файл
        local uiwin = i .. "\\mods\\Anna_Killer\\ui-win"    --Указываем папку ui-win  (Мы можем добавить целую папку позже!)
        local music_intro = i .. "\\mods\\Anna_Killer\\tech_intro.wav"  --Указываем файл музыки
        local music_loop = i .. "\\mods\\Anna_Killer\\tech_loop.wav"    --Указываем файл музыки
        local Stagesinifile = i .. "\\mods\\Anna_Killer\\stages.ini"    --Указываем stage.ini файл
        local Charselect_gbs = i .. "\\mods\\Anna_Killer\\Charselect.gbs"   --Указываем Charselect.gbs файл
        local Charselect_gbs_merge = i .. "\\mods\\Anna_Killer\\Charselect_merge.gbs"   --Указываем Charselect.gbs файл (Мы будем использовать его для обьединения!)
        local Charselect_gbs_path_to_write = i .. "\\mods\\Anna_Killer\\ui-win\\ui\\Win\\Charselect.gbs" --Указываем, куда будем записывать файл после обьединения
        CCLib.add_new_permission("Filia_Pal7")  --Пишем новое "Разрешение" в FULL_SGCC.sal
        CCLib.gfs_extract_file("core", "core/stages.ini", Stagesinifile )   --Разархивируем stages.ini из core.gfs
        local file = io.open(Stagesinifile, "a+")
        if not file then
            return false, "Can't open file"
        end
        file:write("Filia_Pal7 cs_anna_killer stage_anna_killer\n")
        file:close()    --Добавляем новую строчку в файл
        CCLib.gfs_addfile("core","core/stages.ini", Stagesinifile)  --Добавляем stages.ini в core.gfs

        CCLib.gfs_addfile("music-win","music/tech_intro.wav", music_intro)  --Добавляем файл музыки в music-win.gfs
        CCLib.gfs_addfile("music-win","music/tech_loop.wav", music_loop)    --Добавляем файл музыки в music-win.gfs

        CCLib.gfs_addfile("stages","stages/cs_anna_killer.lvl", stageslvl)  --Добавляем файл stage.lvl в stages.gfs
        CCLib.gfs_addfile("stages-textures2d","stages/textures/2D/stage_anna_killer.dds", stagestext)   --Добавляем файл stage_anna_killer.dds (Картинка, фон)  в stages-textures2d.gfs

        CCLib.gfs_extract_file("ui-win", "ui/Win/Charselect.gbs", Charselect_gbs)   --Разархивируем charselect.gbs из ui-win.gfs
        CCLib.gbs_merge(Charselect_gbs, Charselect_gbs_merge, Charselect_gbs_path_to_write)     --Обьединяем два .gbs файла, и пишем результат в Charselect_gbs_path_to_write
        CCLib.gfs_addfiles("ui-win","", uiwin)  --Добавляем целую папку в ui-win.gfs!
        print("[Lua] Anna_Killer install goodbye!")
    end;
    update = function ()    --Что CC будет делать, если мы обновляем мод (Версия мода в луа, выше чем в сейв-даты)
        Mod.install()   --Мы не задумываемся об обновлении, так что... просто переустанавливаем. (Возможны ошибки)
    end;
    init = function ()              --Что CC будет делать во время инифиализации (Выполняется один раз, ДО ТОГО как игра запущена)
        print("[Lua] init hello!")
        local i = CCLib.GetWorkingDirectory()
        local Stagesinifile = i .. "\\mods\\Anna_Killer\\stages.ini"
        local Charselect_gbs = i .. "\\mods\\Anna_Killer\\Charselect.gbs"
        os.remove(Stagesinifile)
        os.remove(Charselect_gbs) --Удаляем временные файлы, если они ещё есть.
    end;
    -- launch = function ()            --Что CC будет делать, когда игра уже запущена (Выполняется ОДИН РАЗ, после запуска игры)
    --     print("[Lua] launch hello!")
    -- end;
    -- loop = function ()              --Что CC будет делать, когда игра уже запущена, и нам надо выполнять это по кругу (Выполняется ПОСТОЯННО, ПОСЛЕ запуска игры)
    --     print("[Lua] loop hello!")
    -- end;
    --deinit = function ()            --Что CC будет делать, когда Skullgirls хочет завершить работу (Выполняется ОДИН РАЗ)
    --    print("[Lua] deinit hello!")
    --end; --Нам не нужны launch, loop и deinit функции, так что просто закоментируем их
}
```

Вы можете найди этот же код и необходимые для работы файлы [здесь](https://github.com/Back-Black-Door/MODS-Anna_Killer)

## Заключение
Эта документация охватывает основные функции CClib для работы с памятью игры и файловой системой. Используйте эти функции для создания модификаций и взаимодействия с Skullgirls.