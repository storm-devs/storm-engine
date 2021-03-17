# Storm-Engine
Game engine behind the [Sea Dogs](https://en.wikipedia.org/wiki/Sea_Dogs_(video_game)) game series.

[![Join the chat at https://discord.gg/jmwETPGFRe](https://img.shields.io/discord/353218868896071692?color=%237289DA&label=storm-devs&logo=discord&logoColor=white)](https://discord.gg/jmwETPGFRe) 
[![GitHub Actions Build Status](https://github.com/storm-devs/storm-engine/actions/workflows/cibuild.yml/badge.svg)](https://github.com/storm-devs/storm-engine/actions/workflows/cibuild.yml)

 * [GitHub Discussions](https://github.com/storm-devs/storm-engine/discussions)
 * [Discord Chat](https://discord.gg/jmwETPGFRe)

<p align="center">
<img src="https://steamuserimages-a.akamaihd.net/ugc/879748394074455443/FD04CEA2434D8DACAD4886AF6A5ADAA54CDE42AA/">
</p>

## Building the project
You need to install [Conan](https://conan.io/downloads.html) and add it to the `%PATH%` environment variable. Also, make sure you have the following Visual Studio components installed:
- C++ CMake Tools for Windows
- C++ Clang Compiler for Windows
- C++ MFC for latest v142 build tools (x86 & x64)

Open the repo root as a CMake project in Visual Studio 2019 and select `engine.exe` as a startup item.

## Roadmap
Since our development team is small, we want to reduce the amount of code we have to maintain.
For this reason, we decided to rely on the C++ standard library or third-party libraries if possible.

Some things that we are going to do:
- Replace a custom math library by a third-party one, e.g. [glm](https://github.com/g-truc/glm).
- Replace custom rendering code with a [bgfx](https://github.com/bkaradzic/bgfx) library.
- Replace custom input handling code with a third-party library, e.g. [gainput](https://github.com/jkuhlmann/gainput).
- Replace custom data structures by C++ standard types.

## Contributing
If you'd like to get involved, don't hesitate to contact us. Pull requests and any other kind of help are more than welcome as well.

If you want to start with small contributions, updating the code to the modern C++ and fixing compiler warnings are very much appeciated.

## License
[GPL-3.0 License](https://choosealicense.com/licenses/gpl-3.0/)
