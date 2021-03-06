# Storm-Engine
Game engine behind the [Sea Dogs](https://en.wikipedia.org/wiki/Sea_Dogs_(video_game)) game series.

## Building the project
You need to have a fairly recent version of [Conan](https://conan.io/downloads.html) package manager installed and added to the `%PATH%` environment variable.

We use Visual Studio CMake integration, so make sure you selected the following components in the Visual Studio 2019 installer:
- C++ CMake Tools for Windows

Open the repo root as a CMake project in Visual Studio.

## Contributing
If you'd like to join our team, don't hesitate to contact one of the project maintainers (you can find email addresses in our profiles).

### Current development priorities
Since our development team is small, we want to reduce the amount of code we have to maintain.
For this reason, it is encouraged to rely on the C++ standard library or third-party libraries if possible.

Some ideas of what should be done:
- Custom math library should be replaced by a third-party one, e.g. [glm](https://github.com/g-truc/glm).
- Custom rendering code should be replaced with a [bgfx](https://github.com/bkaradzic/bgfx) library.
- Custom input handling code should be replaced with a third-party library, e.g. [gainput](https://github.com/jkuhlmann/gainput).
- Custom data structures should be replaced by C++ standard types.

If you want to start with something smaller, updating the code to the modern C++ is very much appeciated.

Pull requests and any other kind of help are more than welcome as well.

## License
[GPL-3.0 License](https://choosealicense.com/licenses/gpl-3.0/)