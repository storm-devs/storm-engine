from conans import ConanFile, tools
from os import getenv
from random import getrandbits
from distutils.dir_util import copy_tree

class StormEngine(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    # build options provided by CMakeLists.txt that are used in conanfile.py
    options = {
        "output_directory": "ANY",
        "watermark_file": "ANY",
        "crash_reports": [True, False],
        "steam": [True, False]
    }

    # dependencies used in deploy binaries
    # conan-center
    requires = ["zlib/1.2.11", "spdlog/1.9.2", "fast_float/3.4.0", "sdl/2.0.18", "mimalloc/2.0.3", "sentry-native/0.5.0",
    # storm.jfrog.io
    "directx/9.0@storm/prebuilt", "fmod/2.02.05@storm/prebuilt"]
    # aux dependencies (e.g. for tests)
    build_requires = "catch2/2.13.7"

    # optional dependencies
    def requirements(self):
        if self.settings.os == "Windows":
            # conan-center
            self.requires("7zip/19.00")
        else:
            # conan-center
            self.requires("libsafec/3.6.0")
            self.requires("wayland/1.20.0#ae7daf789db25b25ad2e22969fd9ae9d")#fix for error: Could not find WAYLAND_SCANNER using the following names: wayland-scanner
            self.requires("libiconv/1.17")#fix for error: 'gettext/0.21' requires 'libiconv/1.17' while 'pulseaudio/14.2' requires 'libiconv/1.16'
            self.requires("openssl/1.1.1n")#fix for error: 'sentry-crashpad/0.4.13' requires 'openssl/1.1.1n' while 'pulseaudio/14.2' requires 'openssl/1.1.1q'
        if self.options.steam:
            self.requires("steamworks/1.5.1@storm/prebuilt")

    generators = "cmake_multi"

    default_options = {
        "sentry-native:backend": "crashpad",
        "mimalloc:shared": True,
        "mimalloc:override": True
    }

    def imports(self):
        self.__dest = str(self.options.output_directory) + "/" + getenv("CONAN_IMPORT_PATH", "bin")
        self.__install_folder("/src/techniques", "/resource/techniques")
        self.__install_folder("/src/libs/shared_headers/include/shared", "/resource/shared")

        if self.settings.os == "Windows":
            if self.settings.build_type == "Debug":
                self.__install_lib("fmodL.dll")
            else:
                self.__install_lib("fmod.dll")

            self.__install_bin("crashpad_handler.exe")
            if self.options.crash_reports:
                self.__install_bin("7za.exe")
            if self.options.steam:
                self.__install_lib("steam_api64.dll")

            self.__install_bin("mimalloc-redirect.dll")
            if self.settings.build_type == "Debug":
                self.__install_bin("mimalloc-debug.dll")
            else:
                self.__install_bin("mimalloc.dll")

        else: # not Windows
            if self.settings.build_type == "Debug":
                self.__install_lib("libfmodL.so.13")
            else:
                self.__install_lib("libfmod.so.13")

            self.__install_bin("crashpad_handler")
            #if self.options.steam:
            #    self.__install_lib("steam_api64.dll")#TODO: fix conan package and then lib name

            if self.settings.build_type == "Debug":
                self.__install_lib("libmimalloc-debug.so.2.0")
                self.__install_lib("libmimalloc-debug.so")
            else:
                self.__install_lib("libmimalloc.so.2.0")
                self.__install_lib("libmimalloc.so")

        self.__write_watermark();


    def __write_watermark(self):
        with open(str(self.options.watermark_file), 'w') as f:
            f.write("#pragma once\n#define STORM_BUILD_WATERMARK ")
            f.write(self.__generate_watermark())
            f.write("\n")

    def __generate_watermark(self):
        git = tools.Git()
        try:
            if git.is_pristine():
                return "%s(%s)" % (git.get_branch(), git.get_revision())
            else:
                return "%s(%s)-DIRTY(%032x)" % (git.get_branch(), git.get_revision(), getrandbits(128))
        except:
            return "Unknown"

    def __install_bin(self, name):
        self.copy(name, dst=self.__dest, src="bin")

    def __install_lib(self, name):
        self.copy(name, dst=self.__dest, src="lib")

    def __install_folder(self, src, dst):
        copy_tree(self.recipe_folder + src, self.__dest + dst)
