cmake_minimum_required(VERSION 3.17)

project(storm-engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Setup third-party dependencies
include(conan)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)

option(STORM_ENABLE_STEAM "Enable Steam integration" ON)

set(STORM_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(STORM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(STORM_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)
