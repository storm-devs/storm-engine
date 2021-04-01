cmake_minimum_required(VERSION 3.17)

project(storm-engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Setup third-party dependencies
include(conan)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
    ENV CONAN_CMAKE_GENERATOR="${CMAKE_GENERATOR}"
    ENV CONAN_MAKE_PROGRAM="${CMAKE_MAKE_PROGRAM}"
    ENV CC="${CMAKE_C_COMPILER}"
    ENV CXX="${CMAKE_CXX_COMPILER}"
)

include(AutoSetup)

option(Config_STORM_ENABLE_STEAM "Enable Steam integration" OFF)

set(STORM_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(STORM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(STORM_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)
