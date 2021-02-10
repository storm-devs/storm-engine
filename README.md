# Storm-Engine
Game engine behind the [Sea Dogs](https://en.wikipedia.org/wiki/Sea_Dogs_(video_game)) game series.

## Building the project
0. You need to have fairly recent versions of [python](https://www.python.org/downloads/) and [cmake](https://cmake.org/download/) installed and added to the `%PATH%` environment variable.


1. Run `gen-sln.bat` to setup the development environment and generate a Visual Studio 2019 solution.

It will use python to install the Conan package manager, which will download and build required third-party dependencies.

Cmake will then generate `storm-engine.sln` in the `build` folder.

## Contributing
If you'd like to join our team, don't hesitate to contact one of the project maintainers (you can find email addresses in our profiles).

Pull requests and any other kind of help are more than welcome as well.

## License
[GPL-3.0 License](https://choosealicense.com/licenses/gpl-3.0/)