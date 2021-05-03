cmake_minimum_required(VERSION 3.17)

project(storm-engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Setup third-party dependencies
include(conan)
set(ENV{CONAN_REVISIONS_ENABLED} 1)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)
conan_add_remote(NAME bincrafters
    URL https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
    VERIFY_SSL True
)
conan_add_remote(NAME storm
    URL https://storm.jfrog.io/artifactory/api/conan/conan-remote
    VERIFY_SSL True
)

include(AutoSetup)

option(STORM_ENABLE_STEAM "Enable Steam integration" OFF)

set(STORM_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(STORM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(STORM_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)
