from conans import ConanFile
from os import getenv
from distutils.dir_util import copy_tree

class StormEngine(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    # build options provided by CMakeLists.txt that are used in conanfile.py
    options = {
        "output_directory": "ANY",
        "crash_reports": [True, False],
        "steam": [True, False]
    }

    # dependencies used in deploy binaries
    # conan-center
    requires = ["zlib/1.2.11", "spdlog/1.9.2", "sentry-native/0.4.12", "7zip/19.00",
    # bincrafters
    "sdl2/2.0.16@bincrafters/stable",
    # storm.jfrog.io
    "directx/9.0@storm/prebuilt", "fmod/2.1.11@storm/prebuilt"]
    # aux dependencies (e.g. for tests)
    build_requires = "catch2/2.13.7"

    # optional dependencies
    def requirements(self):
        if self.options.steam:
            self.requires("steamworks/1.5.1@storm/prebuilt")

    generators = "cmake_multi"

    default_options = {
        "sdl2:sdl2main": False,
        "sentry-native:backend": "crashpad",
        "sentry-native:transport": "winhttp"
    }

    def __install_bin(self, name):
        self.copy(name, dst=self.__dest, src="bin")

    def __intall_lib(self, name):
        self.copy(name, dst=self.__dest, src="lib")

    def __install_folder(self, src, dst):
        copy_tree(self.recipe_folder + src, self.__dest + dst)

    def imports(self):
        self.__dest = str(self.options.output_directory) + "/" + getenv("CONAN_IMPORT_PATH", "bin")

        self.__install_folder("/src/techniques", "/resource/techniques")
        self.__install_folder("/src/libs/shared_headers/include/shared", "/resource/shared")

        if self.settings.build_type == "Debug":
            self.__intall_lib("fmodL.dll")
        else:
            self.__intall_lib("fmod.dll")

        if self.options.crash_reports:
            self.__install_bin("crashpad_handler.exe")
            self.__install_bin("7za.exe")

        if self.options.steam:
            self.__intall_lib("steam_api64.dll")
