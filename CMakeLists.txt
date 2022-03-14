cmake_minimum_required(VERSION 3.18)
project(storm-engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(conan)
include(StormSetup)

option(STORM_ENABLE_CRASH_REPORTS "Enable automatic crash reports" OFF)
option(STORM_ENABLE_STEAM "Enable Steam integration" OFF)

set(BGFX_BUILD_EXAMPLES OFF)
set(BGFX_INSTALL_EXAMPLES OFF)
set(BGFX_BUILD_TOOLS OFF)

set(STORM_EXTERN_RENDERING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/rendering")
set(STORM_EXTERN_MATH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern/math")

### Set up output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

set(STORM_WATERMARK_FILE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/watermark.hpp CACHE FILEPATH "Include file containing build revision, etc." FORCE)

### Set up third-party dependencies
set(ENV{CONAN_REVISIONS_ENABLED} 1)
conan_add_remote(NAME bincrafters
    URL https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
    VERIFY_SSL True
)
conan_add_remote(NAME storm
    URL https://storm.jfrog.io/artifactory/api/conan/conan-remote
    VERIFY_SSL True
)
normalize_booleans(STORM_ENABLE_CRASH_REPORTS STORM_ENABLE_STEAM)
conan_cmake_run(CONANFILE conanfile.py
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
    OPTIONS
        output_directory=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        watermark_file=${STORM_WATERMARK_FILE}
        crash_reports=${STORM_ENABLE_CRASH_REPORTS}
        steam=${STORM_ENABLE_STEAM}
)

### Define library ALIASes for use without CONAN_PKG:: prefix
foreach (conan_target ${CONAN_TARGETS})
    string(REPLACE "CONAN_PKG::" "" unprefixed_target ${conan_target})
	add_library(${unprefixed_target} ALIAS ${conan_target})
endforeach()
###

### Set up build scripts
set(SRC_DIRS "src" CACHE STRING "File locations for public header files" FORCE)
set(PUBLIC_INCLUDE_DIRS "include" CACHE STRING "File locations for source files" FORCE)
set(TESTSUITE_DIRS "testsuite" CACHE STRING "File locations for tests" FORCE)
set(RESOURCE_DIRS "rsrc" CACHE STRING "File locations for tests" FORCE)

set(SOURCE_FILE_EXTENSIONS "*.cpp" CACHE STRING "File extensions for source files" FORCE)
set(HEADER_FILE_EXTENSIONS "*.h" "*.hpp" CACHE STRING "File extensions for header files" FORCE)
###

include_directories(${STORM_EXTERN_RENDERING_DIR}/bgfx/examples/common
						${STORM_EXTERN_RENDERING_DIR}/bgfx/3rdparty 
						${STORM_EXTERN_RENDERING_DIR}/bgfx/3rdparty/dxsdk/include 
						${STORM_EXTERN_RENDERING_DIR}/bgfx/3rdparty/khronos
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bgfx/include>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bgfx/examples/common>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bgfx/tools/brtshaderc>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bimg/include>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bx/include>
						$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bx/3rdparty>
						${STORM_EXTERN_MATH_DIR}/glm/glm
						${GLSL-OPTIMIZER_INCLUDES}
						${GLSLLANG_INCLUDES}
						${SPIRV_CROSS_INCLUDES}
						${SPIRV_TOOLS_INCLUDES}
						${WEBGPU_INCLUDES})

# Build system specific configurations
if( MINGW )
	include_directories(
			$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bx/include/compat/mingw>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/mingw> )
elseif( WIN32 )
	include_directories(
			$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bx/include/compat/msvc>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/msvc> )
elseif( APPLE )
	include_directories(
			$<BUILD_INTERFACE:${STORM_EXTERN_RENDERING_DIR}/bx/include/compat/osx>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/compat/osx> )
endif()

include_directories()
add_definitions("-D_CRT_SECURE_NO_WARNINGS" "-D__STDC_FORMAT_MACROS")

add_subdirectory(src)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine)

target_link_libraries(engine PRIVATE bgfx dear-imgui meshoptimizer brtshaderc fcpp bx bimg bgfx-vertexlayout bgfx-shader-spirv fcpp glsl-optimizer glslang spirv-cross spirv-tools webgpu example-common)

add_subdirectory(${STORM_EXTERN_RENDERING_DIR})
add_subdirectory(${STORM_EXTERN_MATH_DIR}/glm)
