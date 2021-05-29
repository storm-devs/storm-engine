cmake_minimum_required(VERSION 3.17)

project(storm-engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Setup third-party dependencies
include(conan)
set(ENV{CONAN_REVISIONS_ENABLED} 1)
conan_add_remote(NAME bincrafters
    URL https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
    VERIFY_SSL True
)
conan_add_remote(NAME storm
    URL https://storm.jfrog.io/artifactory/api/conan/conan-remote
    VERIFY_SSL True
)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)
conan_output_dirs_setup()

include(AutoSetup)

option(STORM_ENABLE_STEAM "Enable Steam integration" OFF)

set(STORM_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(STORM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(STORM_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")
set(STORM_EXTERN_RENDERING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/rendering")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(${STORM_EXTERN_RENDERING_DIR})


include_directories(
${BGFX_DIR}/examples/common
${BGFX_DIR}/3rdparty 
${BGFX_DIR}/3rdparty/dxsdk/include 
${BGFX_DIR}/3rdparty/khronos
$<BUILD_INTERFACE:${BGFX_DIR}/include>
$<BUILD_INTERFACE:${BIMG_DIR}/include>
$<BUILD_INTERFACE:${BX_DIR}/include>
$<BUILD_INTERFACE:${BX_DIR}/3rdparty>)

link_libraries(bgfx dear-imgui meshoptimizer )
link_libraries(example-common )

add_definitions("-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS" "-DENTRY_CONFIG_IMPLEMENT_MAIN=1")



add_subdirectory(src)
