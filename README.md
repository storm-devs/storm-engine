# Storm-Engine
Game engine behind [Sea Dogs](https://en.wikipedia.org/wiki/Sea_Dogs_(video_game)), [Pirates of the Caribbean](https://en.wikipedia.org/wiki/Pirates_of_the_Caribbean_(video_game)) and [Age of Pirates](https://en.wikipedia.org/wiki/Age_of_Pirates_2:_City_of_Abandoned_Ships) games.

[![Join the chat at https://discord.gg/jmwETPGFRe](https://img.shields.io/discord/353218868896071692?color=%237289DA&label=storm-devs&logo=discord&logoColor=white)](https://discord.gg/jmwETPGFRe) 
[![GitHub Windows Build Status](https://github.com/storm-devs/storm-engine/actions/workflows/ci_windows.yml/badge.svg)](https://github.com/storm-devs/storm-engine/actions/workflows/ci_windows.yml) [![GitHub Linux Build Status](https://github.com/storm-devs/storm-engine/actions/workflows/ci_linux.yml/badge.svg)](https://github.com/storm-devs/storm-engine/actions/workflows/ci_linux.yml)

 * [GitHub Discussions](https://github.com/storm-devs/storm-engine/discussions)
 * [Discord Chat](https://discord.gg/jmwETPGFRe)

## Supported games
- [Sea Dogs: To Each His Own](https://github.com/storm-devs/sd-teho-public)
- [Sea Dogs: City of Abandoned Ships](https://store.steampowered.com/app/937940/Sea_Dogs_City_of_Abandoned_Ships/) (work in progress)
- [Pirates of the Caribbean: New Horizons](https://www.piratesahoy.net/wiki/new-horizons/) (work in progress)

<p align="center">
<img src="https://steamuserimages-a.akamaihd.net/ugc/879748394074455443/FD04CEA2434D8DACAD4886AF6A5ADAA54CDE42AA/">
</p>

## Building the project
You need to install [Conan](https://conan.io/downloads.html) and add it to the `%PATH%` environment variable. Also, make sure you have the following Visual Studio components installed:
- C++ CMake Tools for Windows
- C++ Clang Compiler for Windows
- C++ MFC for latest v142 build tools (x86 & x64)
- Windows 10 SDK

Open the repo root as a CMake project in Visual Studio 2019 and select `engine.exe` as a startup item.

For running `engine.exe` you need to have [DirectX 9 runtime libraries](https://www.microsoft.com/en-us/download/details.aspx?id=8109) installed.
You will also need assets from one of the supported games. 

## Roadmap
Since our development team is small, we want to reduce the amount of code we have to maintain.
For this reason, we decided to rely on the C++ standard library or third-party libraries if possible.

Some things that we are going to do:
- Replace a custom math library with a third-party one, e.g. [glm](https://github.com/g-truc/glm).
- Replace custom rendering code with a [bgfx](https://github.com/bkaradzic/bgfx) library.
- Replace custom input handling code with a third-party library (see the [discussion](https://github.com/storm-devs/storm-engine/discussions/19)).
- Replace custom data structures with C++ standard types.
- Replace a custom scripting language with Lua.
- Replace ini config files with a standard format (see the [discussion](https://github.com/storm-devs/storm-engine/discussions/26)).
- Replace custom binary asset formats with standard ones.
- Update code using the latest C++ standard

## Contributing
If you'd like to get involved, please check [CONTRIBUTING.md](https://github.com/storm-devs/storm-engine/blob/develop/CONTRIBUTING.md).

## License
[GPL-3.0 License](https://choosealicense.com/licenses/gpl-3.0/)
