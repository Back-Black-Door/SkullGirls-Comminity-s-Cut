# README

[Русская версия](docs/README_RUS.md)

### How To Install:

1. Copy this DLL, into your game's directory, like this: 
C:\Program Files (x86)\Steam\steamapps\common\Skullgirls\d3d9.dll

2. Enjoy!

You'll probably also want to download mods or create your own.
It's just a **Mod-Loader!**
### UI
Press the `INSERT` button to open the Mod Loader interface.

Here you can view loaded mods and view the console

### Launch options

`-originalgame` - Launches the original game (without mods)

`-reinstall` - Reinstalls all mods

`-logtoconsole` -This isn't a ModLoader launch options, but it forces the game to output its logs to the console. It might be useful for debugging or other purposes.

### For users:
Mods are on the [organization main page](https://github.com/Back-Black-Door)

### For mod developers:
Documentation is in the [CClib.md](docs/CClib.md)

For more information, join our [Discord](https://discord.gg/4ufGJQjkpc)

Maybe you want to ask _ImpDi_ for something? :^)

### Building tips:
The default output path for the SGCC dll is set in `DefaultSettings.props`. To use a custom output path (e.g., your Skullgirls game folder), create a `LocalOverrides.props` file:
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <OutDir>$(SystemDrive)\SteamLibrary\steamapps\common\Skullgirls</OutDir>
  </PropertyGroup>
</Project>
```
